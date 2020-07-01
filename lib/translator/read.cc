#include "read.h"

#include <cmath>

#include "absl/types/span.h"
#include "common.h"

namespace translator {

namespace {  // anonymous namespace for helper functions

uint8_t GetPrinfo(uint8_t rd_protect) {
  bool pract;      // Protection Information Action 1 bit
  uint8_t prchk;   // Protection Information Check 3 bits
  uint8_t prinfo;  // 4 bits

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

void SetLogicalBlockTags(uint32_t eilbrt, uint16_t elbat, uint16_t elbatm,
                         nvme_defs::GenericQueueEntryCmd& queue_entry) {
  // expected initial logical block reference
  queue_entry.cdw[4] = eilbrt;
  // expected logical block application tag
  queue_entry.cdw[5] |= elbat;
  // bits  31:16 expected logical block application tag mask
  queue_entry.cdw[5] |= (elbatm << 16);
}

void LegacyRead(uint32_t lba, uint16_t transfer_length,
                nvme_defs::GenericQueueEntryCmd& queue_entry) {
  queue_entry.opc = static_cast<uint8_t>(nvme_defs::NvmOpcode::kRead);
  queue_entry.fuse = 0b00;  // Normal operation, not fused
  queue_entry.psdt = 0b00;  // PRPs are used for data transfer
  queue_entry.mptr =
      reinterpret_cast<std::uint64_t>(AllocPages(1));  // metadata pointer
  queue_entry.nsid = 0x0;  // not used for this command, cleared to 0

  queue_entry.dptr.prp.prp1 = reinterpret_cast<std::uint64_t>(AllocPages(1));

  // cdw10 Starting lba bits 31:00
  queue_entry.cdw[0] = lba;
  // cdw12 nlb bits 15:00
  queue_entry.cdw[2] |= transfer_length;
}

void Read(uint8_t rd_protect, bool fua, uint32_t lba, uint16_t transfer_length,
          nvme_defs::GenericQueueEntryCmd queue_entry) {
  LegacyRead(lba, transfer_length, queue_entry);

  // cdw12 prinfo bits 29:26
  queue_entry.cdw[2] |= (GetPrinfo(rd_protect) << 26);
  // cdw12 fua bit 30;
  queue_entry.cdw[2] |= (fua << 30);
}

void ReadMultiple(uint8_t rd_protect, bool fua, uint32_t lba,
                  uint16_t transfer_length, bool set_lba_tags, uint16_t eilbrt,
                  uint16_t elbat, uint16_t lbatm,
                  absl::Span<nvme_defs::GenericQueueEntryCmd> queue_entries) {
  uint8_t num_calls = transfer_length / pow(2, 16);

  Read(rd_protect, fua, lba, transfer_length % (uint16_t)pow(2, 16),
       queue_entries[0]);

  for (int i = 1; i <= num_calls; i++) {
    Read(rd_protect, fua, lba, pow(2, 16), queue_entries[i]);
    if (set_lba_tags) {
      SetLogicalBlockTags(eilbrt, elbat, lbatm, queue_entries[i]);
    }
  }
}

}  // namespace

void Read6ToNvme(scsi_defs::Read6Command cmd,
                 nvme_defs::GenericQueueEntryCmd queue_entry) {
  return LegacyRead(cmd.logical_block_address, cmd.transfer_length,
                    queue_entry);
}

void Read10ToNvme(scsi_defs::Read10Command cmd,
                  nvme_defs::GenericQueueEntryCmd queue_entry) {
  return Read(cmd.rd_protect, cmd.fua, cmd.logical_block_address,
              cmd.transfer_length, queue_entry);
}

void Read12ToNvme(scsi_defs::Read12Command cmd,
                  absl::Span<nvme_defs::GenericQueueEntryCmd> queue_entries) {
  ReadMultiple(cmd.rd_protect, cmd.fua, cmd.logical_block_address,
               cmd.transfer_length, false, 0, 0, 0, queue_entries);
}

void Read16ToNvme(scsi_defs::Read16Command cmd,
                  absl::Span<nvme_defs::GenericQueueEntryCmd> queue_entries) {
  ReadMultiple(cmd.rd_protect, cmd.fua, cmd.logical_block_address,
               cmd.transfer_length, false, 0, 0, 0, queue_entries);
}

void Read32ToNvme(scsi_defs::Read32Command cmd,
                  absl::Span<nvme_defs::GenericQueueEntryCmd> queue_entries) {
  ReadMultiple(cmd.rd_protect, cmd.fua, cmd.logical_block_address,
               cmd.transfer_length, true, cmd.eilbrt, cmd.elbat, cmd.lbatm,
               queue_entries);
}

}  // namespace translator
