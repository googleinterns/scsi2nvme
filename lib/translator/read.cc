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

  switch (rd_protect) {
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
      // shouldn't reach here because we covered all possible values of
      // rd_protect
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
  // expected logical block application tag
  nvme_cmd.cdw[5] |= elbat;
  // bits  31:16 expected logical block application tag mask
  nvme_cmd.cdw[5] |= (elbatm << 16);
}

void LegacyRead(uint32_t nsid, uint32_t lba, uint16_t transfer_length,
                nvme_defs::GenericQueueEntryCmd &nvme_cmd) {
  nvme_cmd.opc = static_cast<uint8_t>(nvme_defs::NvmOpcode::kRead);
  nvme_cmd.fuse = 0b00;  // Normal operation, not fused
  nvme_cmd.psdt = 0b00;  // PRPs are used for data transfer
  nvme_cmd.mptr = reinterpret_cast<std::uint64_t>(AllocPages(1));

  nvme_cmd.nsid = nsid;

  nvme_cmd.dptr.prp.prp1 = reinterpret_cast<std::uint64_t>(AllocPages(1));

  // cdw10 Starting lba bits 31:00
  nvme_cmd.cdw[0] = lba;
  // cdw12 nlb bits 15:00
  nvme_cmd.cdw[2] = transfer_length;
}

void Read(uint32_t nsid, uint8_t rd_protect, bool fua, uint32_t lba,
          uint16_t transfer_length, nvme_defs::GenericQueueEntryCmd &nvme_cmd) {
  LegacyRead(nsid, lba, transfer_length, nvme_cmd);

  // cdw12 prinfo bits 29:26
  nvme_cmd.cdw[2] |= (GetPrinfo(rd_protect) << 26);
  // cdw12 fua bit 30;
  nvme_cmd.cdw[2] |= (fua << 30);
}

void ReadMultiple(uint32_t nsid, uint8_t rd_protect, bool fua, uint32_t lba,
                  uint32_t transfer_length, bool set_lba_tags, uint16_t eilbrt,
                  uint16_t elbat, uint16_t lbatm, uint32_t &nvme_cmd_count,
                  nvme_defs::GenericQueueEntryCmd nvme_cmds[]) {
  int num_calls = transfer_length / pow(2, 16) + 1;
  nvme_cmd_count = num_calls;
  //>>  queue_entires = absl::MakeSpan(nvme_cmds, nvme_cmd_count);

  absl::Span<nvme_defs::GenericQueueEntryCmd> span =
      absl::MakeSpan(nvme_cmds, nvme_cmd_count);

  nvme_defs::GenericQueueEntryCmd temp[nvme_cmd_count];
  nvme_cmds = temp;

  for (int i = 0; i < num_calls; i++) {
    uint16_t updated_transfer_length =
        (i == 0) ? transfer_length % (uint16_t)pow(2, 16)
                 : (uint16_t)pow(2, 16);
    Read(nsid, rd_protect, fua, lba, (uint16_t)pow(2, 16), nvme_cmds[i]);
    if (set_lba_tags) {
      SetLbaTags(eilbrt, elbat, lbatm, nvme_cmds[i]);
    }
  }
}

}  // namespace

StatusCode Read6ToNvme(uint32_t nsid, absl::Span<const uint8_t> raw_cmd,
                       nvme_defs::GenericQueueEntryCmd &nvme_cmd) {
  if (raw_cmd.size() != sizeof(scsi_defs::Read6Command)) {
    DebugLog("Malformed Read6 command");
    return StatusCode::kInvalidInput;
  }
  scsi_defs::Read6Command cmd;
  ReadValue(raw_cmd, cmd);
  LegacyRead(nsid, cmd.logical_block_address, cmd.transfer_length, nvme_cmd);
  return StatusCode::kSuccess;
}

StatusCode Read10ToNvme(uint32_t nsid, absl::Span<const uint8_t> raw_cmd,
                        nvme_defs::GenericQueueEntryCmd &nvme_cmd) {
  // scsi_defs::Read10Command cmd =
  // absl::bit_cast<scsi_defs::Read10Command>(raw_cmd);
  if (raw_cmd.size() != sizeof(scsi_defs::Read10Command)) {
    DebugLog("Malformed Read10 command");
    return StatusCode::kInvalidInput;
  }
  scsi_defs::Read10Command cmd;
  ReadValue(raw_cmd, cmd);
  Read(nsid, cmd.rd_protect, cmd.fua, cmd.logical_block_address,
       cmd.transfer_length, nvme_cmd);
  return StatusCode::kSuccess;
}

StatusCode Read12ToNvme(uint32_t nsid, absl::Span<const uint8_t> raw_cmd,
                        uint32_t &nvme_cmd_count,
                        nvme_defs::GenericQueueEntryCmd nvme_cmds[]) {
  if (raw_cmd.size() != sizeof(scsi_defs::Read12Command)) {
    DebugLog("Malformed Read12 command");
    return StatusCode::kInvalidInput;
  }
  scsi_defs::Read12Command cmd;
  ReadValue(raw_cmd, cmd);

  ReadMultiple(nsid, cmd.rd_protect, cmd.fua, cmd.logical_block_address,
               cmd.transfer_length, false, 0, 0, 0, nvme_cmd_count, nvme_cmds);
  return StatusCode::kSuccess;
}

StatusCode Read16ToNvme(uint32_t nsid, absl::Span<const uint8_t> raw_cmd,
                        uint32_t &nvme_cmd_count,
                        nvme_defs::GenericQueueEntryCmd nvme_cmds[]) {
  if (raw_cmd.size() != sizeof(scsi_defs::Read16Command)) {
    DebugLog("Malformed Read16 command");
    return StatusCode::kInvalidInput;
  }
  scsi_defs::Read16Command cmd;
  ReadValue(raw_cmd, cmd);
  ReadMultiple(nsid, cmd.rd_protect, cmd.fua, cmd.logical_block_address,
               cmd.transfer_length, false, 0, 0, 0, nvme_cmd_count, nvme_cmds);
  return StatusCode::kSuccess;
}

StatusCode Read32ToNvme(uint32_t nsid, absl::Span<const uint8_t> raw_cmd,
                        uint32_t &nvme_cmd_count,
                        nvme_defs::GenericQueueEntryCmd nvme_cmds[]) {
  if (raw_cmd.size() != sizeof(scsi_defs::Read32Command)) {
    DebugLog("Malformed Read32 command");
    return StatusCode::kInvalidInput;
  }
  scsi_defs::Read32Command cmd;
  memcpy(&cmd, &raw_cmd, sizeof(scsi_defs::Read32Command));
  ReadMultiple(nsid, cmd.rd_protect, cmd.fua, cmd.logical_block_address,
               cmd.transfer_length, true, cmd.eilbrt, cmd.elbat, cmd.lbatm,
               nvme_cmd_count, nvme_cmds);
  return StatusCode::kSuccess;
}

}  // namespace translator
