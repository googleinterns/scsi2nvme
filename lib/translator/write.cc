#include "write.h"

#ifdef __KERNEL__
#include <linux/byteorder/generic.h>
#else
#include <netinet/in.h>
#endif
#include <byteswap.h>

namespace translator {

// anonymous namespace for helper functions and variables
namespace {

StatusCode BuildPRInfo(uint8_t wrprotect, uint8_t& pr_info) {
  uint8_t pract, prchk;

  switch (wrprotect) {
    case 0b000:
      pract = 1;
      prchk = 0b000;
      break;
    case 0b001:
      pract = 0;
      prchk = 0b111;
      break;
    case 0b101:
      pract = 0;
      prchk = 0b111;
      break;
    case 0b010:
      pract = 0;
      prchk = 0b011;
      break;
    case 0b011:
      pract = 0;
      prchk = 0b00;
      break;
    case 0b100:
      pract = 0;
      prchk = 0b100;
      break;
    default:
      // All other codes shall result in command termination with CHECK
      // CONDITION status, ILLEGAL REQUEST sense key, and ILLEGAL FIELD IN CDB
      // additional sense code.
      DebugLog("Invalid WriteProtect Code for PRInfo.");
      return StatusCode::kFailure;
  }

  pr_info = prchk | (pract << 3);
  return StatusCode::kSuccess;
}

// Converts transfer_length in units of logical blocks to units of pages
StatusCode GetTransferLengthPages(uint32_t& transfer_length_pages,
                                  uint16_t transfer_length, uint32_t page_size,
                                  uint32_t lba_size) {
  if (page_size == 0) {
    DebugLog("Page size is 0 for Write Command");
    return StatusCode::kFailure;
  }

  uint64_t transfer_length_bytes = transfer_length * lba_size;
  transfer_length_pages = transfer_length_bytes / page_size +
                          ((transfer_length_bytes % page_size == 0) ? 0 : 1);

  return StatusCode::kSuccess;
}

// This function populates the nvme::GenericQueueEntryCmd object with fields
// that are common to all Write Commands (6, 10, 12, 16) Refer to Section 5.7
// (https://nvmexpress.org/wp-content/uploads/NVM_Express_-_SCSI_Translation_Reference-1_5_20150624_Gold.pdf)
StatusCode LegacyWrite(NvmeCmdWrapper& nvme_wrapper, Allocation& allocation,
                       uint32_t nsid, uint32_t transfer_length_pages,
                       Span<const uint8_t> buffer_out) {
  nvme_wrapper.cmd = {.opc = static_cast<uint8_t>(nvme::NvmOpcode::kWrite),
	.psdt = 0, //PRPs are used
                      .nsid = nsid};

  nvme_wrapper.cmd.dptr.prp.prp1 =
      reinterpret_cast<uint64_t>(buffer_out.data());

  return StatusCode::kSuccess;
}

// Builds NVMe cdw 12 for Write10, Write12, Write16 translations
uint32_t BuildCdw12(uint16_t transfer_length, uint8_t prinfo, bool fua) {
  // cdw12 nlb bits 15:00 (zero based field), prinfo bits 29:26, fua bit 30
  return (static_cast<uint32_t>(fua) << 30) |
         (static_cast<uint32_t>(prinfo) << 26) | (transfer_length - 1);
}

StatusCode Write(bool fua, uint8_t wrprotect, uint32_t transfer_length,
                 NvmeCmdWrapper& nvme_wrapper, Allocation& allocation,
                 uint32_t nsid, uint32_t page_size, uint32_t lba_size,
                 Span<const uint8_t> buffer_out) {
  if (transfer_length == 0) {
    DebugLog("NVMe write command does not support transfering zero blocks\n");
    return StatusCode::kNoTranslation;
  }

	transfer_length &=0xffff;

  uint32_t transfer_length_pages = 0;
  StatusCode status_code = GetTransferLengthPages(
      transfer_length_pages, transfer_length, page_size, lba_size);

  if (status_code != StatusCode::kSuccess) {
    return status_code;
  }

  status_code = LegacyWrite(nvme_wrapper, allocation, nsid,
                            transfer_length_pages, buffer_out);

  if (status_code != StatusCode::kSuccess) {
    return status_code;
  }

  uint8_t pr_info;
  status_code = BuildPRInfo(wrprotect, pr_info);

  if (status_code != StatusCode::kSuccess) {
    return status_code;
  }
  nvme_wrapper.cmd.cdw[2] = htoll(BuildCdw12(transfer_length, pr_info, fua));

  return status_code;
}

}  // namespace

StatusCode Write6ToNvme(Span<const uint8_t> scsi_cmd,
                        NvmeCmdWrapper& nvme_wrapper, Allocation& allocation,
                        uint32_t nsid, uint32_t page_size, uint32_t lba_size,
                        Span<const uint8_t> buffer_out) {
  scsi::Write6Command write_cmd = {};
  if (!ReadValue(scsi_cmd, write_cmd)) {
    DebugLog("Malformed Write6 Command");
    return StatusCode::kInvalidInput;
  }

  // A TRANSFER LENGTH field set to zero specifies that 256 logical blocks shall
  // be written. Any other value specifies the number of logical blocks that
  // shall be written (Section 3.59) of
  // https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
  uint16_t updated_transfer_length =
      (write_cmd.transfer_length == 0) ? 256 : write_cmd.transfer_length;

  uint32_t transfer_length_pages = 0;
  StatusCode status_code = GetTransferLengthPages(
      transfer_length_pages, updated_transfer_length, page_size, lba_size);

  if (status_code != StatusCode::kSuccess) {
    return status_code;
  }

  status_code = LegacyWrite(nvme_wrapper, allocation, nsid,
                            transfer_length_pages, buffer_out);

  if (status_code != StatusCode::kSuccess) {
    return status_code;
  }

  // cdw10 Starting lba bits 31:00
  uint32_t host_endian_lba = (write_cmd.logical_block_address_1 << 16) +
                             ntohs(write_cmd.logical_block_address_2);

  nvme_wrapper.cmd.cdw[0] = htoll(host_endian_lba);
  nvme_wrapper.cmd.cdw[2] =
      htoll(static_cast<uint32_t>(updated_transfer_length - 1));
  return status_code;
}

StatusCode Write10ToNvme(Span<const uint8_t> scsi_cmd,
                         NvmeCmdWrapper& nvme_wrapper, Allocation& allocation,
                         uint32_t nsid, uint32_t page_size, uint32_t lba_size,
                         Span<const uint8_t> buffer_out) {
  scsi::Write10Command write_cmd = {};
  if (!ReadValue(scsi_cmd, write_cmd)) {
    DebugLog("Malformed Write10 Command");
    return StatusCode::kInvalidInput;
  }

	DebugLog("transfer length %d\n", ntohs(write_cmd.transfer_length));
	DebugLog("starting lba %d\n", ntohl(write_cmd.logical_block_address));
	DebugLog("Fua %d\n", write_cmd.fua);
	DebugLog("wr_protect%d\n", write_cmd.wr_protect);

  StatusCode status_code = Write(
      write_cmd.fua, write_cmd.wr_protect, ntohs(write_cmd.transfer_length),
      nvme_wrapper, allocation, nsid, page_size, lba_size, buffer_out);

  if (status_code != StatusCode::kSuccess) {
    return status_code;
  }

  nvme_wrapper.cmd.cdw[0] = __bswap_32(write_cmd.logical_block_address);
	DebugLog("cdw 0 %x, cdw 2 %x\n", nvme_wrapper.cmd.cdw[0], nvme_wrapper.cmd.cdw[2]);
	DebugLog("Writing %u blocks", (nvme_wrapper.cmd.cdw[2] & 0xffff) +1);
	nvme_wrapper.is_admin = 0;
  return status_code;
}
StatusCode Write12ToNvme(Span<const uint8_t> scsi_cmd,
                         NvmeCmdWrapper& nvme_wrapper, Allocation& allocation,
                         uint32_t nsid, uint32_t page_size, uint32_t lba_size,
                         Span<const uint8_t> buffer_out) {
  scsi::Write12Command write_cmd = {};
  if (!ReadValue(scsi_cmd, write_cmd)) {
    DebugLog("Malformed Write12 Command");
    return StatusCode::kInvalidInput;
  }

  StatusCode status_code = Write(
      write_cmd.fua, write_cmd.wr_protect, ntohl(write_cmd.transfer_length),
      nvme_wrapper, allocation, nsid, page_size, lba_size, buffer_out);

  if (status_code != StatusCode::kSuccess) {
    return status_code;
  }

  nvme_wrapper.cmd.cdw[0] = __bswap_32(write_cmd.logical_block_address);
  return status_code;
}

StatusCode Write16ToNvme(Span<const uint8_t> scsi_cmd,
                         NvmeCmdWrapper& nvme_wrapper, Allocation& allocation,
                         uint32_t nsid, uint32_t page_size, uint32_t lba_size,
                         Span<const uint8_t> buffer_out) {
  scsi::Write16Command write_cmd = {};
  if (!ReadValue(scsi_cmd, write_cmd)) {
    DebugLog("Malformed Write16 Command");
    return StatusCode::kInvalidInput;
  }

  StatusCode status_code = Write(
      write_cmd.fua, write_cmd.wr_protect, ntohl(write_cmd.transfer_length),
      nvme_wrapper, allocation, nsid, page_size, lba_size, buffer_out);

  if (status_code != StatusCode::kSuccess) {
    return status_code;
  }

  uint64_t updated_lba = ntohll(write_cmd.logical_block_address);
  nvme_wrapper.cmd.cdw[0] = htoll(write_cmd.logical_block_address);
  nvme_wrapper.cmd.cdw[1] = htoll(write_cmd.logical_block_address >> 32);

  return status_code;
}
}  // namespace translator
