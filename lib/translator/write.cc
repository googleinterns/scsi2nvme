#include "write.h"
#include "common.h"

#include "absl/types/span.h"

namespace translator {

// anonymous namespace for helper functions and variables
namespace {
}

StatusCode Write6ToNvme(absl::span<const uint8_t> scsi_cmd,
                        nvme::GenericQueueEntryCmd& nvme_cmd,
                        Allocation& allocation) {
  scsi::Write6Command write_cmd;
  if (!ReadValue(scsi_cmd, write_cmd)) {
    DebugLog("Malformed Write6 Command");
    return StatusCode::kInvalidInput;
  }
}

StatusCode Write10ToNvme(absl::span<const uint8_t> scsi_cmd,
                        nvme::GenericQueueEntryCmd& nvme_cmd,
                        Allocation& allocation) {
  scsi::Write10Command write_cmd;
  if (!ReadValue(scsi_cmd, write_cmd)) {
    DebugLog("Malformed Write10 Command");
    return StatusCode::kInvalidInput;
  }
}

StatusCode Write12ToNvme(absl::span<const uint8_t> scsi_cmd,
                        nvme::GenericQueueEntryCmd& nvme_cmd,
                        Allocation& allocation) {
  scsi::Write12Command write_cmd;
  if (!ReadValue(scsi_cmd, write_cmd)) {
    DebugLog("Malformed Write12 Command");
    return StatusCode::kInvalidInput;
  }
}

StatusCode Write16ToNvme(absl::span<const uint8_t> scsi_cmd,
                        nvme::GenericQueueEntryCmd& nvme_cmd,
                        Allocation& allocation) {
  scsi::Write16Command write_cmd;
  if (!ReadValue(scsi_cmd, write_cmd)) {
    DebugLog("Malformed Write16 Command");
    return StatusCode::kInvalidInput;
  }
}
}  // namespace translator
