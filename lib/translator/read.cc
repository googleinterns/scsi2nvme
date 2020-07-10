#include "read.h"

#include "absl/types/span.h"

#include "common.h"

namespace translator {

namespace {  // anonymous namespace for helper functions

// Section 5.3
// https://www.nvmexpress.org/wp-content/uploads/NVM-Express-SCSI-Translation-Reference-1_1-Gold.pdf
StatusCode BuildPrinfo(uint8_t rdprotect, uint8_t& prinfo) {
  bool pract;     // Protection Information Action 1 bit
  uint8_t prchk;  // Protection Information Check 3 bits
  prinfo = 0;     // Protection Information field 4 bits

  switch (rdprotect) {  // 3 bits
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
               rdprotect);
      return StatusCode::kInvalidInput;
  }

  prinfo |= prchk;
  prinfo |= (pract << 3);

  return StatusCode::kSuccess;
}

void SetLbaTags(uint32_t eilbrt, uint16_t elbat, uint16_t elbatm,
                nvme::GenericQueueEntryCmd& nvme_cmd) {
  // cdw14 expected initial logical block reference
  nvme_cmd.cdw[4] = eilbrt;
  // cdw15 bits 15:0 expected logical block application tag
  nvme_cmd.cdw[5] |= elbat;
  // cdw15 bits 31:16 expected logical block application tag mask
  nvme_cmd.cdw[5] |= (elbatm << 16);
}

StatusCode LegacyRead(uint32_t lba, uint16_t transfer_length,
                      nvme::GenericQueueEntryCmd& nvme_cmd,
                      Allocation& allocation) {
  StatusCode status_code = allocation.SetPages(1, 1);

  if (status_code != StatusCode::kSuccess) {
    return status_code;
  }

  nvme_cmd = nvme::GenericQueueEntryCmd{
      .opc = static_cast<uint8_t>(nvme::NvmOpcode::kRead),
      .psdt = 0b00  // PRPs are used for data transfer
  };

  nvme_cmd.mptr = allocation.mdata_addr;
  nvme_cmd.dptr.prp.prp1 = allocation.data_addr;
  nvme_cmd.cdw[0] = lba;              // cdw10 Starting lba bits 31:00
  nvme_cmd.cdw[2] = transfer_length;  // cdw12 nlb bits 15:00

  return StatusCode::kSuccess;
}

StatusCode Read(uint8_t rdprotect, bool fua, uint32_t lba,
                uint16_t transfer_length, nvme::GenericQueueEntryCmd& nvme_cmd,
                Allocation& allocation) {
  StatusCode status = LegacyRead(lba, transfer_length, nvme_cmd, allocation);

  if (status != StatusCode::kSuccess) {
    return status;
  }

  uint8_t prinfo;  // Protection Information field 4 bits;
  status = BuildPrinfo(rdprotect, prinfo);

  if (status != StatusCode::kSuccess) {
    return status;
  }

  nvme_cmd.cdw[2] |= (prinfo << 26);  // cdw12 prinfo bits 29:26
  nvme_cmd.cdw[2] |= (fua << 30);     // cdw12 fua bit 30;

  return StatusCode::kSuccess;
}

}  // namespace

StatusCode Read6ToNvme(absl::Span<const uint8_t> scsi_cmd,
                       nvme::GenericQueueEntryCmd& nvme_cmd,
                       Allocation& allocation) {
  scsi::Read6Command read_cmd;
  if (!ReadValue(scsi_cmd, read_cmd)) {
    DebugLog("Malformed Read6 command");
    return StatusCode::kInvalidInput;
  }

  return LegacyRead(read_cmd.logical_block_address, read_cmd.transfer_length,
                    nvme_cmd, allocation);
}

StatusCode Read10ToNvme(absl::Span<const uint8_t> scsi_cmd,
                        nvme::GenericQueueEntryCmd& nvme_cmd,
                        Allocation& allocation) {
  scsi::Read10Command read_cmd;
  if (!ReadValue(scsi_cmd, read_cmd)) {
    DebugLog("Malformed Read10 command");
    return StatusCode::kInvalidInput;
  }

  return Read(read_cmd.rdprotect, read_cmd.fua, read_cmd.logical_block_address,
              read_cmd.transfer_length, nvme_cmd, allocation);
}

StatusCode Read12ToNvme(absl::Span<const uint8_t> scsi_cmd,
                        nvme::GenericQueueEntryCmd& nvme_cmd,
                        Allocation& allocation) {
  scsi::Read12Command read_cmd;
  if (!ReadValue(scsi_cmd, read_cmd)) {
    DebugLog("Malformed Read12 command");
    return StatusCode::kInvalidInput;
  }

  return Read(read_cmd.rdprotect, read_cmd.fua, read_cmd.logical_block_address,
              read_cmd.transfer_length, nvme_cmd, allocation);
}

StatusCode Read16ToNvme(absl::Span<const uint8_t> scsi_cmd,
                        nvme::GenericQueueEntryCmd& nvme_cmd,
                        Allocation& allocation) {
  scsi::Read16Command read_cmd;
  if (!ReadValue(scsi_cmd, read_cmd)) {
    DebugLog("Malformed Read16 command");
    return StatusCode::kInvalidInput;
  }

  return Read(read_cmd.rdprotect, read_cmd.fua, read_cmd.logical_block_address,
              read_cmd.transfer_length, nvme_cmd, allocation);
}

StatusCode Read32ToNvme(absl::Span<const uint8_t> scsi_cmd,
                        nvme::GenericQueueEntryCmd& nvme_cmd,
                        Allocation& allocation) {
  scsi::Read32Command read_cmd;
  if (!ReadValue(scsi_cmd, read_cmd)) {
    DebugLog("Malformed Read32 command");
    return StatusCode::kInvalidInput;
  }

  StatusCode status =
      Read(read_cmd.rdprotect, read_cmd.fua, read_cmd.logical_block_address,
           read_cmd.transfer_length, nvme_cmd, allocation);

  if (status != StatusCode::kSuccess) {
    return status;
  }

  SetLbaTags(read_cmd.eilbrt, read_cmd.elbat, read_cmd.lbatm, nvme_cmd);
  return StatusCode::kSuccess;
}

}  // namespace translator
