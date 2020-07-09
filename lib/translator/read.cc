#include "read.h"

#include <cmath>
#include <cstdio>

#include "absl/base/casts.h"
#include "absl/types/span.h"
#include "common.h"

namespace translator {

namespace {  // anonymous namespace for helper functions

uint8_t GetPrinfo(uint8_t rd_protect) {
  bool pract;      // Protection Information Action 1 bit
  uint8_t prchk;   // Protection Information Check 3 bits
  uint8_t prinfo = 0;  // 4 bits

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
    default:  // Should never reach here because we covered all possible values
      break;
  }

  prinfo |= prchk;
  prinfo |= (pract << 3);

  return prinfo;
}

void SetLbaTags(uint32_t eilbrt, uint16_t elbat, uint16_t elbatm,
                nvme_defs::GenericQueueEntryCmd &nvme_cmd) {
  // expected initial logical block reference
  nvme_cmd.cdw[4] = eilbrt;
  // bits 15:0 expected logical block application tag
  nvme_cmd.cdw[5] |= elbat;
  // bits 31:16 expected logical block application tag mask
  nvme_cmd.cdw[5] |= (elbatm << 16);
}

StatusCode LegacyRead(uint32_t lba, uint16_t transfer_length,
                      nvme_defs::GenericQueueEntryCmd& nvme_cmd) {
  nvme_cmd = nvme_defs::GenericQueueEntryCmd{
      .opc = static_cast<uint8_t>(nvme_defs::NvmOpcode::kRead),
      .fuse = 0b00,  // Normal operation, not fused
      .psdt = 0b00,  // PRPs are used for data transfer
  };

  uint64_t mptr= AllocPages(1);
  uint64_t prp = AllocPages(1);
  if (mptr == 0 || prp == 0) {
    DebugLog("Error when requesting a page of memory");
    return StatusCode::kFailure;
  }

  nvme_cmd.mptr= mptr;
  nvme_cmd.dptr.prp.prp1 = prp;
  nvme_cmd.cdw[0] = lba;              // cdw10 Starting lba bits 31:00
  nvme_cmd.cdw[2] = transfer_length;  // cdw12 nlb bits 15:00

  return StatusCode::kSuccess;
}

StatusCode Read(uint8_t rd_protect, bool fua, uint32_t lba,
                uint16_t transfer_length,
                nvme_defs::GenericQueueEntryCmd& nvme_cmd) {
  StatusCode status = LegacyRead(lba, transfer_length, nvme_cmd);

  if (status == StatusCode::kFailure) {
    return StatusCode::kFailure;
  }

  nvme_cmd.cdw[2] |= (GetPrinfo(rd_protect) << 26);  // cdw12 prinfo bits 29:26
  nvme_cmd.cdw[2] |= (fua << 30);  // cdw12 fua bit 30;

  return StatusCode::kSuccess;
}

}  // namespace

StatusCode Read6ToNvme(absl::Span<const uint8_t> raw_cmd,
                       nvme_defs::GenericQueueEntryCmd &nvme_cmd) {
  if (raw_cmd.size() != sizeof(scsi_defs::Read6Command)) {
    DebugLog("Malformed Read6 command");
    return StatusCode::kInvalidInput;
  }

  scsi_defs::Read6Command cmd;
  if (!ReadValue(raw_cmd, cmd)) {
    DebugLog("Unable to cast raw bytes to Read6 command");
    return StatusCode::kInvalidInput;
  }

  return LegacyRead(cmd.logical_block_address, cmd.transfer_length, nvme_cmd);
}

StatusCode Read10ToNvme(absl::Span<const uint8_t> raw_cmd,
                        nvme_defs::GenericQueueEntryCmd &nvme_cmd) {
  if (raw_cmd.size() != sizeof(scsi_defs::Read10Command)) {
    DebugLog("Malformed Read10 command");
    return StatusCode::kInvalidInput;
  }

  scsi_defs::Read10Command cmd;
  if (!ReadValue(raw_cmd, cmd)) {
    DebugLog("Unable to cast raw bytes to Read10 command");
    return StatusCode::kInvalidInput;
  }

  return Read(cmd.rd_protect, cmd.fua, cmd.logical_block_address,
              cmd.transfer_length, nvme_cmd);
}

StatusCode Read12ToNvme(absl::Span<const uint8_t> raw_cmd,
                        nvme_defs::GenericQueueEntryCmd &nvme_cmd) {
  if (raw_cmd.size() != sizeof(scsi_defs::Read12Command)) {
    DebugLog("Malformed Read12 command");
    return StatusCode::kInvalidInput;
  }

  scsi_defs::Read12Command cmd;
  if (!ReadValue(raw_cmd, cmd)) {
    DebugLog("Unable to cast raw bytes to Read12 command");
    return StatusCode::kInvalidInput;
  }

  return Read(cmd.rd_protect, cmd.fua, cmd.logical_block_address,
              cmd.transfer_length, nvme_cmd);
}

StatusCode Read16ToNvme(absl::Span<const uint8_t> raw_cmd,
                        nvme_defs::GenericQueueEntryCmd &nvme_cmd) {
  if (raw_cmd.size() != sizeof(scsi_defs::Read16Command)) {
    DebugLog("Malformed Read16 command");
    return StatusCode::kInvalidInput;
  }

  scsi_defs::Read16Command cmd;
  if (!ReadValue(raw_cmd, cmd)) {
    DebugLog("Unable to cast raw bytes to Read16 command");
    return StatusCode::kInvalidInput;
  }

  return Read(cmd.rd_protect, cmd.fua, cmd.logical_block_address,
              cmd.transfer_length, nvme_cmd);
}

StatusCode Read32ToNvme(absl::Span<const uint8_t> raw_cmd,
                        nvme_defs::GenericQueueEntryCmd &nvme_cmd) {
  if (raw_cmd.size() != sizeof(scsi_defs::Read32Command)) {
    DebugLog("Malformed Read32 command");
    return StatusCode::kInvalidInput;
  }

  scsi_defs::Read32Command cmd;
  if (!ReadValue(raw_cmd, cmd)) {
    DebugLog("Unable to cast raw bytes to Read32 command");
    return StatusCode::kInvalidInput;
  }

  StatusCode status = Read(cmd.rd_protect, cmd.fua, cmd.logical_block_address,
                           cmd.transfer_length, nvme_cmd);

  if (status == StatusCode::kFailure) {
    return StatusCode::kFailure;
  }

  SetLbaTags(cmd.eilbrt, cmd.elbat, cmd.lbatm, nvme_cmd);
  return StatusCode::kSuccess;
}

}  // namespace translator
