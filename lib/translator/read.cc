#include "read.h"

#ifdef __KERNEL__
#include <linux/byteorder/generic.h>
#else
#include <netinet/in.h>
#endif
#include <byteswap.h>

#include "absl/types/span.h"

#include "common.h"

namespace translator {

namespace {  // anonymous namespace for helper functions

// Section 5.3
// https://www.nvmexpress.org/wp-content/uploads/NVM-Express-SCSI-Translation-Reference-1_1-Gold.pdf
StatusCode BuildPrinfo(uint8_t rdprotect, uint8_t& prinfo) {
  bool pract;     // Protection Information Action 1 bit
  uint8_t prchk;  // Protection Information Check 3 bits

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

  prinfo = prchk | (pract << 3);

  return StatusCode::kSuccess;
}

// Translates fields common to all Read commands
// Named Legacy because it is called directly by Read6, an obsolete command
// lacking fields common to other Read commands
StatusCode LegacyRead(uint64_t lba, nvme::GenericQueueEntryCmd& nvme_cmd,
                      Allocation& allocation) {
  StatusCode status = allocation.SetPages(1, 1);

  if (status != StatusCode::kSuccess) {
    return status;
  }

  nvme_cmd = nvme::GenericQueueEntryCmd{
      .opc = static_cast<uint8_t>(nvme::NvmOpcode::kRead),
      .psdt = 0  // PRPs are used for data transfer
  };

  nvme_cmd.mptr = __bswap_64(allocation.mdata_addr);
  nvme_cmd.dptr.prp.prp1 = __bswap_64(allocation.data_addr);
  nvme_cmd.cdw[0] = __bswap_32(lba);  // cdw10 Starting lba bits 31:00
  nvme_cmd.cdw[1] =
      __bswap_32((uint32_t)(lba >> 32));  // cdw11 starting lba bits 63:32

  return status;
}

// Translates fields common to Read10, Read12, Read16, Read32
StatusCode Read(uint8_t rdprotect, bool fua, uint64_t lba,
                uint32_t transfer_length, nvme::GenericQueueEntryCmd& nvme_cmd,
                Allocation& allocation) {
  if (transfer_length == 0) {
    DebugLog("NVMe read command does not support transfering zero blocks");
    return StatusCode::kNoTranslation;
  }

  StatusCode status = LegacyRead(lba, nvme_cmd, allocation);

  if (status != StatusCode::kSuccess) {
    return status;
  }

  uint8_t prinfo;  // Protection Information field 4 bits;
  status = BuildPrinfo(rdprotect, prinfo);

  if (status != StatusCode::kSuccess) {
    return status;
  }

  // cdw12 nlb bits 15:00
  // cdw12 printfo bits 29:26
  // cdw12 fua bit 30
  // Since NVMe has a protocol limit of 16 bits on transfer size,
  // we can enforce a transfer limit of 16 bits on our SCSI device
  uint32_t cdw12 =
      ((uint16_t)transfer_length - 1) | (prinfo << 26) | (fua << 30);
  nvme_cmd.cdw[2] = __bswap_32(cdw12);  // overwrite cdw12

  return status;
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

  // Ensure Big Endian
  uint32_t lba = htonl((uint32_t)read_cmd.logical_block_address);

  StatusCode status = LegacyRead(lba, nvme_cmd, allocation);

  if (status != StatusCode::kSuccess) {
    return status;
  }

  // cdw12 nlb bits 15:00, zero based field
  // Transfer Length set to 0 specifies 256 logical blocks to be read
  // Section 3.15 Seagate SCSI specs
  uint16_t updated_transfer_length =
      read_cmd.transfer_length == 0 ? 255 : read_cmd.transfer_length - 1;
  nvme_cmd.cdw[2] = __bswap_16(updated_transfer_length);

  return status;
}

StatusCode Read10ToNvme(absl::Span<const uint8_t> scsi_cmd,
                        nvme::GenericQueueEntryCmd& nvme_cmd,
                        Allocation& allocation) {
  scsi::Read10Command read_cmd;
  if (!ReadValue(scsi_cmd, read_cmd)) {
    DebugLog("Malformed Read10 command");
    return StatusCode::kInvalidInput;
  }

  // Ensure Big Endian
  uint32_t lba = htonl(read_cmd.logical_block_address);
  uint32_t transfer_length = htons(read_cmd.transfer_length);

  return Read(read_cmd.rdprotect, read_cmd.fua, lba, transfer_length, nvme_cmd,
              allocation);
}

StatusCode Read12ToNvme(absl::Span<const uint8_t> scsi_cmd,
                        nvme::GenericQueueEntryCmd& nvme_cmd,
                        Allocation& allocation) {
  scsi::Read12Command read_cmd;
  if (!ReadValue(scsi_cmd, read_cmd)) {
    DebugLog("Malformed Read12 command");
    return StatusCode::kInvalidInput;
  }

  // Ensure Big Endian
  read_cmd.logical_block_address = htonl(read_cmd.logical_block_address);
  read_cmd.transfer_length = htonl(read_cmd.transfer_length);

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

  // Ensure Big Endian
  read_cmd.logical_block_address = htonll(read_cmd.logical_block_address);
  read_cmd.transfer_length = htonl(read_cmd.transfer_length);

  return Read(read_cmd.rdprotect, read_cmd.fua, read_cmd.logical_block_address,
              read_cmd.transfer_length, nvme_cmd, allocation);
}

}  // namespace translator
