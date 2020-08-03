#include "write.h"

#include "common.h"

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
      break;
  }

  pr_info = prchk | (pract << 3);
  return StatusCode::kSuccess;
}

// Converts transfer_length in units of logical blocks to units of pages
uint32_t GetTransferLengthPages(uint16_t transfer_length, uint32_t page_size,
                                uint32_t lba_size) {
  uint64_t transfer_length_bytes = transfer_length * lba_size;
  return transfer_length_bytes / page_size +
         ((transfer_length_bytes % page_size == 0) ? 0 : 1);
}

// This function populates the nvme::GenericQueueEntryCmd object with fields
// that are common to all Write Commands (6, 10, 12, 16) Refer to Section 5.7
// (https://nvmexpress.org/wp-content/uploads/NVM_Express_-_SCSI_Translation_Reference-1_5_20150624_Gold.pdf)
StatusCode LegacyWrite(nvme::GenericQueueEntryCmd& nvme_cmd,
                       Allocation& allocation, uint32_t nsid,
                       uint32_t transfer_length_pages) {
  StatusCode status_code = allocation.SetPages(transfer_length_pages, 0);

  if (status_code != StatusCode::kSuccess) {
    return status_code;
  }

  nvme_cmd = {.opc = static_cast<uint8_t>(nvme::NvmOpcode::kWrite),
              .psdt = 0,  // prps are used
              .nsid = nsid};

  nvme_cmd.dptr.prp.prp1 = allocation.data_addr;

  return status_code;
}

StatusCode Write(bool fua, uint8_t wrprotect, uint32_t transfer_length,
                 nvme::GenericQueueEntryCmd& nvme_cmd, Allocation& allocation,
                 uint32_t nsid, uint32_t page_size, uint32_t lba_size) {
  if (transfer_length == 0) {
    DebugLog("NVMe read command does not support transfering zero blocks\n");
    return StatusCode::kNoTranslation;
  }

  transfer_length &= 0xffff;  // truncate to 16 bits
  StatusCode status_code =
      LegacyWrite(nvme_cmd, allocation, nsid,
                  GetTransferLengthPages(transfer_length, page_size, lba_size));

  if (status_code != StatusCode::kSuccess) {
    return status_code;
  }

  uint8_t pr_info;
  status_code = BuildPRInfo(wrprotect, pr_info);

  if (status_code != StatusCode::kSuccess) {
    return status_code;
  }
  nvme_cmd.cdw[2] = (transfer_length - 1 | pr_info << 26 | fua << 30);

  return status_code;
}

}  // namespace

StatusCode Write6ToNvme(Span<const uint8_t> scsi_cmd,
                        nvme::GenericQueueEntryCmd& nvme_cmd,
                        Allocation& allocation, uint32_t nsid,
                        uint32_t page_size, uint32_t lba_size) {
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
  StatusCode status_code = LegacyWrite(
      nvme_cmd, allocation, nsid,
      GetTransferLengthPages(updated_transfer_length, page_size, lba_size));

  if (status_code != StatusCode::kSuccess) {
    return status_code;
  }

  // cdw10 Starting lba bits 31:00
  uint32_t host_endian_lba = (write_cmd.logical_block_address_1 << 16) +
                             ntohs(write_cmd.logical_block_address);

  nvme_cmd.cdw[0] = htoll(host_endian_lba);
  nvme_cmd.cdw[2] = htoll(static_cast<uint32_t>(updated_transfer_length - 1));
  return status_code;
}

StatusCode Write10ToNvme(Span<const uint8_t> scsi_cmd,
                         nvme::GenericQueueEntryCmd& nvme_cmd,
                         Allocation& allocation, uint32_t nsid,
                         uint32_t page_size, uint32_t lba_size) {
  scsi::Write10Command write_cmd = {};
  if (!ReadValue(scsi_cmd, write_cmd)) {
    DebugLog("Malformed Write10 Command");
    return StatusCode::kInvalidInput;
  }

  StatusCode status_code =
      Write(write_cmd.fua, write_cmd.wr_protect, write_cmd.transfer_length,
            nvme_cmd, allocation, nsid, page_size, lba_size);

  if (status_code != StatusCode::kSuccess) {
    return status_code;
  }

  nvme_cmd.cdw[0] = __bswap_32(write_cmd.logical_block_address);
  return status_code;
}
StatusCode Write12ToNvme(Span<const uint8_t> scsi_cmd,
                         nvme::GenericQueueEntryCmd& nvme_cmd,
                         Allocation& allocation, uint32_t nsid,
                         uint32_t page_size, uint32_t lba_size) {
  scsi::Write12Command write_cmd = {};
  if (!ReadValue(scsi_cmd, write_cmd)) {
    DebugLog("Malformed Write12 Command");
    return StatusCode::kInvalidInput;
  }

  StatusCode status_code =
      Write(write_cmd.fua, write_cmd.wr_protect, write_cmd.transfer_length,
            nvme_cmd, allocation, nsid, page_size, lba_size);

  if (status_code != StatusCode::kSuccess) {
    return status_code;
  }

  nvme_cmd.cdw[0] = __bswap_32(write_cmd.logical_block_address);
  return status_code;
}

StatusCode Write16ToNvme(Span<const uint8_t> scsi_cmd,
                         nvme::GenericQueueEntryCmd& nvme_cmd,
                         Allocation& allocation, uint32_t nsid,
                         uint32_t page_size, uint32_t lba_size) {
  scsi::Write16Command write_cmd = {};
  if (!ReadValue(scsi_cmd, write_cmd)) {
    DebugLog("Malformed Write16 Command");
    return StatusCode::kInvalidInput;
  }

  StatusCode status_code =
      Write(write_cmd.fua, write_cmd.wr_protect, write_cmd.transfer_length,
            nvme_cmd, allocation, nsid, page_size, lba_size);

  if (status_code != StatusCode::kSuccess) {
    return status_code;
  }

  uint64_t updated_lba = ntohll(write_cmd.logical_block_address);
  nvme_cmd.cdw[0] = htoll(write_cmd.logical_block_address);
  nvme_cmd.cdw[1] = htoll(write_cmd.logical_block_address >> 32);

  return status_code;
}
}  // namespace translator
