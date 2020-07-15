#include "write.h"

#include "common.h"

#include "absl/types/span.h"

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
    case (0b001 || 0b101):
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

  pr_info |= prchk | (pract << 3);
  return StatusCode::kSuccess;
}

// This function is populates the nvme::GenericQueueEntryCmd object with fields
// that are common to all Write Commands (6, 10, 12, 16) Refer to Section 5.7
// (https://nvmexpress.org/wp-content/uploads/NVM_Express_-_SCSI_Translation_Reference-1_5_20150624_Gold.pdf)
StatusCode LegacyWrite(uint64_t lba, nvme::GenericQueueEntryCmd& nvme_cmd,
                       Allocation& allocation) {
  StatusCode status_code = allocation.SetPages(1, 1);

  if (status_code != StatusCode::kSuccess) {
    return status_code;
  }

  nvme_cmd = {
      .opc = static_cast<uint8_t>(nvme::NvmOpcode::kWrite),
      .psdt = 0,
      .mptr = allocation.mdata_addr,
  };

  nvme_cmd.dptr.prp.prp1 = allocation.data_addr;

  // TODO: convert to Little-Endian as lba is in Big-Endian because it is taken
  // from SCSI Write struct.
  nvme_cmd.cdw[0] |= lba;
  nvme_cmd.cdw[1] |= (lba >> 32);

  return status_code;
}

StatusCode Write(bool fua, uint8_t wrprotect, uint64_t lba,
                 uint32_t transfer_length, nvme::GenericQueueEntryCmd& nvme_cmd,
                 Allocation& allocation) {
  StatusCode status_code = LegacyWrite(lba, nvme_cmd, allocation);

  if (status_code != StatusCode::kSuccess) {
    return status_code;
  }

  uint8_t pr_info;
  status_code = BuildPRInfo(wrprotect, pr_info);

  if (status_code != StatusCode::kSuccess) {
    return status_code;
  }
  nvme_cmd.cdw[2] |= (transfer_length | pr_info << 26 | fua << 30);
  return status_code;
}

}  // namespace

StatusCode Write6ToNvme(absl::Span<const uint8_t> scsi_cmd,
                        nvme::GenericQueueEntryCmd& nvme_cmd,
                        Allocation& allocation) {
  scsi::Write6Command write_cmd;
  if (!ReadValue(scsi_cmd, write_cmd)) {
    DebugLog("Malformed Write6 Command");
    return StatusCode::kInvalidInput;
  }

  StatusCode status_code =
      LegacyWrite(write_cmd.logical_block_address, nvme_cmd, allocation);

  nvme_cmd.cdw[2] |= write_cmd.transfer_length;
  return status_code;
}

StatusCode Write10ToNvme(absl::Span<const uint8_t> scsi_cmd,
                         nvme::GenericQueueEntryCmd& nvme_cmd,
                         Allocation& allocation) {
  scsi::Write10Command write_cmd;
  if (!ReadValue(scsi_cmd, write_cmd)) {
    DebugLog("Malformed Write10 Command");
    return StatusCode::kInvalidInput;
  }

  StatusCode status_code = Write(
      write_cmd.fua, write_cmd.wr_protect, write_cmd.logical_block_address,
      write_cmd.transfer_length, nvme_cmd, allocation);

  return status_code;
}

StatusCode Write12ToNvme(absl::Span<const uint8_t> scsi_cmd,
                         nvme::GenericQueueEntryCmd& nvme_cmd,
                         Allocation& allocation) {
  scsi::Write12Command write_cmd;
  if (!ReadValue(scsi_cmd, write_cmd)) {
    DebugLog("Malformed Write12 Command");
    return StatusCode::kInvalidInput;
  }

  StatusCode status_code = Write(
      write_cmd.fua, write_cmd.wr_protect, write_cmd.logical_block_address,
      write_cmd.transfer_length, nvme_cmd, allocation);

  return status_code;
}

StatusCode Write16ToNvme(absl::Span<const uint8_t> scsi_cmd,
                         nvme::GenericQueueEntryCmd& nvme_cmd,
                         Allocation& allocation) {
  scsi::Write16Command write_cmd;
  if (!ReadValue(scsi_cmd, write_cmd)) {
    DebugLog("Malformed Write16 Command");
    return StatusCode::kInvalidInput;
  }

  StatusCode status_code = Write(
      write_cmd.fua, write_cmd.wr_protect, write_cmd.logical_block_address,
      write_cmd.transfer_length, nvme_cmd, allocation);
  nvme_cmd.cdw[2] |= write_cmd.transfer_length;

  return status_code;
}
}  // namespace translator
