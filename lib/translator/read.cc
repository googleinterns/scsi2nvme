#include "read.h"

#ifdef __KERNEL__
#include <linux/byteorder/generic.h>
#else
#include <netinet/in.h>
#endif
#include <byteswap.h>

namespace translator {

namespace {  // anonymous namespace for helper functions

// Section 5.3
// https://www.nvmexpress.org/wp-content/uploads/NVM-Express-SCSI-Translation-Reference-1_1-Gold.pdf
StatusCode BuildPrinfo(uint8_t rd_protect, uint8_t& prinfo) {
  bool pract;     // Protection Information Action 1 bit
  uint8_t prchk;  // Protection Information Check 3 bits

  switch (rd_protect) {  // 3 bits
    case 0b0:
      pract = 0b1;
      prchk = 0b111;
      break;
    case 0b001:
    case 0b101:
      pract = 0b0;
      prchk = 0b111;
      break;
    case 0b010:
      pract = 0b0;
      prchk = 0b011;
      break;
    case 0b011:
      pract = 0b0;
      prchk = 0b000;
      break;
    case 0b100:
      pract = 0b0;
      prchk = 0b100;
      break;
    default:
      // Should result in SCSI command termination with status: CHECK CONDITION,
      //  sense key: ILLEGAL REQUEST, additional sense code: LLEGAL FIELD IN CDB
      DebugLog("RDPROTECT with value %d has no translation to PRINFO",
               rd_protect);
      return StatusCode::kInvalidInput;
  }

  prinfo = prchk | (pract << 3);

  return StatusCode::kSuccess;
}

// Builds NVMe cdw 12 for Read10, Read12, Read16 translations
uint32_t BuildCdw12(uint16_t transfer_length, uint8_t prinfo, bool fua) {
  // cdw12 nlb bits 15:00 (zero based field), prinfo bits 29:26, fua bit 30
  return (static_cast<uint32_t>(fua) << 30) |
         (static_cast<uint32_t>(prinfo) << 26) | (transfer_length - 1);
}

// Translates fields common to all Read commands
// Named Legacy because it is called directly by Read6, an obsolete command
// lacking fields common to other Read commands
StatusCode LegacyRead(NvmeCmdWrapper& nvme_wrapper, Allocation& allocation,
                      uint32_t nsid, uint16_t transfer_length,
                      uint32_t lba_size, Span<const uint8_t> buffer_in,
                      uint32_t& alloc_len) {
  nvme_wrapper.cmd = nvme::GenericQueueEntryCmd{
      .opc = static_cast<uint8_t>(nvme::NvmOpcode::kRead),
      .psdt = 0,  // PRPs are used for data transfer
      .nsid = nsid};

  alloc_len = transfer_length * lba_size;

  if (buffer_in.size() < alloc_len) {
    DebugLog("Not enough memory allocated for Read buffer");
    return StatusCode::kFailure;
  }

  nvme_wrapper.buffer_len = alloc_len;
  nvme_wrapper.cmd.dptr.prp.prp1 = reinterpret_cast<uint64_t>(buffer_in.data());

  nvme_wrapper.is_admin = false;

  return StatusCode::kSuccess;
}

// Translates fields common to Read10, Read12, Read16
StatusCode Read(uint8_t rd_protect, bool fua, uint32_t transfer_length,
                NvmeCmdWrapper& nvme_wrapper, Allocation& allocation,
                uint32_t nsid, uint32_t lba_size, Span<const uint8_t> buffer_in,
                uint32_t& alloc_len) {
  if (transfer_length == 0) {
    DebugLog("NVMe read command does not support transfering zero blocks");
    return StatusCode::kNoTranslation;
  }

  // Since NVMe has a protocol limit of 16 bits on transfer size,
  // we can enforce a transfer limit of 16 bits on our SCSI device
  if (transfer_length > 0xffff) {
    DebugLog("Transfer length exceeds limit of 16 bits");
    return StatusCode::kInvalidInput;
  }
  transfer_length &= 0xffff;  // truncate to 16 bits

  StatusCode status =
      LegacyRead(nvme_wrapper, allocation, nsid, transfer_length, lba_size,
                 buffer_in, alloc_len);
  if (status != StatusCode::kSuccess) {
    return status;
  }

  uint8_t prinfo;  // Protection Information field 4 bits;
  status = BuildPrinfo(rd_protect, prinfo);
  if (status != StatusCode::kSuccess) {
    return status;
  }

  nvme_wrapper.cmd.cdw[2] = htoll(BuildCdw12(transfer_length, prinfo, fua));

  return StatusCode::kSuccess;
}

}  // namespace

StatusCode Read6ToNvme(Span<const uint8_t> scsi_cmd,
                       NvmeCmdWrapper& nvme_wrapper, Allocation& allocation,
                       uint32_t nsid, uint32_t lba_size,
                       Span<const uint8_t> buffer_in, uint32_t& alloc_len) {
  scsi::Read6Command read_cmd;
  if (!ReadValue(scsi_cmd, read_cmd)) {
    DebugLog("Malformed Read6 command");
    return StatusCode::kInvalidInput;
  }
  // Transfer Length set to 0 specifies 256 logical blocks to be read
  // Section 3.15 Seagate SCSI specs
  uint16_t updated_transfer_length =
      read_cmd.transfer_length == 0
          ? 256
          : static_cast<uint16_t>(read_cmd.transfer_length);

  StatusCode status =
      LegacyRead(nvme_wrapper, allocation, nsid, updated_transfer_length,
                 lba_size, buffer_in, alloc_len);
  if (status != StatusCode::kSuccess) {
    return status;
  }
  // cdw10 Starting lba bits 31:00
  uint32_t host_endian_lba = (read_cmd.logical_block_address_1 << 16) +
                             ntohs(read_cmd.logical_block_address_2);
  nvme_wrapper.cmd.cdw[0] = htoll(host_endian_lba);

  // nlb is a zero-based field
  // cdw12 nlb bits 15:00
  nvme_wrapper.cmd.cdw[2] =
      htoll(static_cast<uint32_t>(updated_transfer_length) - 1);

  return StatusCode::kSuccess;
}

StatusCode Read10ToNvme(Span<const uint8_t> scsi_cmd,
                        NvmeCmdWrapper& nvme_wrapper, Allocation& allocation,
                        uint32_t nsid, uint32_t lba_size,
                        Span<const uint8_t> buffer_in, uint32_t& alloc_len) {
  scsi::Read10Command read_cmd;
  if (!ReadValue(scsi_cmd, read_cmd)) {
    DebugLog("Malformed Read10 command");
    return StatusCode::kInvalidInput;
  }

  // Transform logical_block_address and transfer_length to host endian
  StatusCode status =
      Read(read_cmd.rd_protect, read_cmd.fua, ntohs(read_cmd.transfer_length),
           nvme_wrapper, allocation, nsid, lba_size, buffer_in, alloc_len);
  if (status != StatusCode::kSuccess) {
    return status;
  }

  nvme_wrapper.cmd.cdw[0] = bswap_32(read_cmd.logical_block_address);

  return StatusCode::kSuccess;
}

StatusCode Read12ToNvme(Span<const uint8_t> scsi_cmd,
                        NvmeCmdWrapper& nvme_wrapper, Allocation& allocation,
                        uint32_t nsid, uint32_t lba_size,
                        Span<const uint8_t> buffer_in, uint32_t& alloc_len) {
  scsi::Read12Command read_cmd;
  if (!ReadValue(scsi_cmd, read_cmd)) {
    DebugLog("Malformed Read12 command");
    return StatusCode::kInvalidInput;
  }

  // Transform logical_block_address and transfer_length to host endian
  StatusCode status =
      Read(read_cmd.rd_protect, read_cmd.fua, ntohl(read_cmd.transfer_length),
           nvme_wrapper, allocation, nsid, lba_size, buffer_in, alloc_len);
  if (status != StatusCode::kSuccess) {
    return status;
  }

  nvme_wrapper.cmd.cdw[0] = bswap_32(read_cmd.logical_block_address);

  return StatusCode::kSuccess;
}

StatusCode Read16ToNvme(Span<const uint8_t> scsi_cmd,
                        NvmeCmdWrapper& nvme_wrapper, Allocation& allocation,
                        uint32_t nsid, uint32_t lba_size,
                        Span<const uint8_t> buffer_in, uint32_t& alloc_len) {
  scsi::Read16Command read_cmd;
  if (!ReadValue(scsi_cmd, read_cmd)) {
    DebugLog("Malformed Read16 command");
    return StatusCode::kInvalidInput;
  }

  // Transform logical_block_address to network endian
  StatusCode status =
      Read(read_cmd.rd_protect, read_cmd.fua, ntohl(read_cmd.transfer_length),
           nvme_wrapper, allocation, nsid, lba_size, buffer_in, alloc_len);
  if (status != StatusCode::kSuccess) {
    return status;
  }

  uint64_t host_endian_lba = ntohll(read_cmd.logical_block_address);
  nvme_wrapper.cmd.cdw[0] = htoll(static_cast<uint32_t>(host_endian_lba));
  nvme_wrapper.cmd.cdw[1] = htoll(static_cast<uint32_t>(host_endian_lba >> 32));

  return StatusCode::kSuccess;
}

}  // namespace translator
