#include "read.h"

#ifdef __KERNEL__
#include <linux/byteorder/generic.h>
#else
#include <netinet/in.h>
#endif

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
  // cdw12 nlb bits 15:00 (zero based field), printfo bits 29:26, fua bit 30
  return (static_cast<uint32_t>(fua) << 30) |
         (static_cast<uint32_t>(prinfo) << 26) | (transfer_length - 1);
}

// Translates fields common to all Read commands
// Named Legacy because it is called directly by Read6, an obsolete command
// lacking fields common to other Read commands
StatusCode LegacyRead(uint64_t lba, nvme::GenericQueueEntryCmd& nvme_read,
                      Allocation& allocation, uint32_t nsid) {
  StatusCode status = allocation.SetPages(1, 1);

  if (status != StatusCode::kSuccess) {
    return status;
  }

  nvme_read = nvme::GenericQueueEntryCmd{
      .opc = static_cast<uint8_t>(nvme::NvmOpcode::kRead),
      .psdt = 0,  // PRPs are used for data transfer
      .nsid = nsid};

  nvme_read.mptr = allocation.mdata_addr;
  nvme_read.dptr.prp.prp1 = allocation.data_addr;

  // cdw10 Starting lba bits 31:00
  nvme_read.cdw[0] = static_cast<uint32_t>(lba);
  // cdw11 Starting lba bits 63:32
  nvme_read.cdw[1] = static_cast<uint32_t>(lba >> 32);

  return status;
}

// Translates fields common to Read10, Read12, Read16
StatusCode Read(uint8_t rd_protect, bool fua, uint64_t lba,
                uint32_t transfer_length, nvme::GenericQueueEntryCmd& nvme_read,
                Allocation& allocation, uint32_t nsid) {
  if (transfer_length == 0) {
    DebugLog("NVMe read command does not support transfering zero blocks");
    return StatusCode::kNoTranslation;
  }

  StatusCode status = LegacyRead(lba, nvme_read, allocation, nsid);

  if (status != StatusCode::kSuccess) {
    return status;
  }

  uint8_t prinfo;  // Protection Information field 4 bits;
  status = BuildPrinfo(rd_protect, prinfo);

  if (status != StatusCode::kSuccess) {
    return status;
  }

  // Since NVMe has a protocol limit of 16 bits on transfer size,
  // we can enforce a transfer limit of 16 bits on our SCSI device
  if (transfer_length > 0xffff) {
    DebugLog("Transfer length exceeds limit of 16 bits");
    return StatusCode::kInvalidInput;
  }

  nvme_read.cdw[2] =
      BuildCdw12(static_cast<uint16_t>(transfer_length), prinfo, fua);

  return status;
}

// Build Identify Namespace command to get NVMe block size
void BuildIdentifyNamespace(nvme::GenericQueueEntryCmd& identify_ns,
                            Allocation& allocation, uint32_t nsid) {
  allocation.SetPages(1, 0);
  identify_ns = nvme::GenericQueueEntryCmd{
      .opc = static_cast<uint8_t>(nvme::AdminOpcode::kIdentify), .nsid = nsid};
  identify_ns.dptr.prp.prp1 = allocation.data_addr;
  identify_ns.cdw[0] = 0x0;  // cns val
}

}  // namespace

StatusCode Read6ToNvme(Span<const uint8_t> scsi_cmd,
                       Span<nvme::GenericQueueEntryCmd> nvme_cmds,
                       Span<Allocation> allocations, uint32_t nsid) {
  scsi::Read6Command scsi_read;
  if (!ReadValue(scsi_cmd, scsi_read)) {
    DebugLog("Malformed Read6 command");
    return StatusCode::kInvalidInput;
  }

  nvme::GenericQueueEntryCmd& nvme_identify_ns = nvme_cmds[0];
  Allocation& alloc_identify_ns = allocations[0];
  BuildIdentifyNamespace(nvme_identify_ns, alloc_identify_ns, nsid);

  nvme::GenericQueueEntryCmd& nvme_read = nvme_cmds[1];
  Allocation& alloc_read = allocations[1];

  // Transform logical_block_address to network endian before casting to 64 bits
  StatusCode status = LegacyRead(htonl(scsi_read.logical_block_address),
                                 nvme_read, alloc_read, nsid);

  if (status != StatusCode::kSuccess) {
    return status;
  }

  // cdw12 nlb bits 15:00 (zero based field)
  // Transfer Length set to 0 specifies 256 logical blocks to be read
  // Section 3.15 Seagate SCSI specs
  uint16_t updated_transfer_length =
      scsi_read.transfer_length == 0 ? 255 : scsi_read.transfer_length - 1;
  nvme_read.cdw[2] = updated_transfer_length;

  return status;
}

StatusCode Read10ToNvme(Span<const uint8_t> scsi_cmd,
                        Span<nvme::GenericQueueEntryCmd> nvme_cmds,
                        Span<Allocation> allocations, uint32_t nsid) {
  scsi::Read10Command scsi_read;
  if (!ReadValue(scsi_cmd, scsi_read)) {
    DebugLog("Malformed Read10 command");
    return StatusCode::kInvalidInput;
  }

  nvme::GenericQueueEntryCmd& nvme_identify_ns = nvme_cmds[0];
  Allocation& alloc_identify_ns = allocations[0];
  BuildIdentifyNamespace(nvme_identify_ns, alloc_identify_ns, nsid);

  nvme::GenericQueueEntryCmd& nvme_read = nvme_cmds[1];
  Allocation& alloc_read = allocations[1];

  // Transform logical_block_address to network endian before casting to 64 bits
  return Read(scsi_read.rd_protect, scsi_read.fua,
              ntohl(scsi_read.logical_block_address), scsi_read.transfer_length,
              nvme_read, alloc_read, nsid);
}

StatusCode Read12ToNvme(Span<const uint8_t> scsi_cmd,
                        Span<nvme::GenericQueueEntryCmd> nvme_cmds,
                        Span<Allocation> allocations, uint32_t nsid) {
  scsi::Read12Command scsi_read;
  if (!ReadValue(scsi_cmd, scsi_read)) {
    DebugLog("Malformed Read12 command");
    return StatusCode::kInvalidInput;
  }

  nvme::GenericQueueEntryCmd& nvme_identify_ns = nvme_cmds[0];
  Allocation& alloc_identify_ns = allocations[0];
  BuildIdentifyNamespace(nvme_identify_ns, alloc_identify_ns, nsid);

  nvme::GenericQueueEntryCmd& nvme_read = nvme_cmds[1];
  Allocation& alloc_read = allocations[1];
  // Transform logical_block_address to network endian before casting to 64 bits
  return Read(scsi_read.rd_protect, scsi_read.fua,
              ntohl(scsi_read.logical_block_address), scsi_read.transfer_length,
              nvme_read, alloc_read, nsid);
}

StatusCode Read16ToNvme(Span<const uint8_t> scsi_cmd,
                        Span<nvme::GenericQueueEntryCmd> nvme_cmds,
                        Span<Allocation> allocations, uint32_t nsid) {
  scsi::Read16Command scsi_read;
  if (!ReadValue(scsi_cmd, scsi_read)) {
    DebugLog("Malformed Read16 command");
    return StatusCode::kInvalidInput;
  }

  nvme::GenericQueueEntryCmd& nvme_identify_ns = nvme_cmds[0];
  Allocation& alloc_identify_ns = allocations[0];
  BuildIdentifyNamespace(nvme_identify_ns, alloc_identify_ns, nsid);

  nvme::GenericQueueEntryCmd& nvme_read = nvme_cmds[1];
  Allocation& alloc_read = allocations[1];

  // Transform logical_block_address to network endian
  return Read(scsi_read.rd_protect, scsi_read.fua,
              ntohll(scsi_read.logical_block_address),
              scsi_read.transfer_length, nvme_read, alloc_read, nsid);
}

}  // namespace translator
