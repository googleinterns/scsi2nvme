// Copyright 2020 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef LIB_TRANSLATOR_TRANSLATION_H
#define LIB_TRANSLATOR_TRANSLATION_H

#include "absl/types/span.h"
#include "common.h"

namespace translator {

enum class ApiStatus { kSuccess, kFailure };

struct BeginResponse {
  ApiStatus status;
  uint32_t alloc_len;
};

class Translation {
 public:
  // Translates from SCSI to NVMe. Translated commands available through
  // GetNvmeCmds()
  BeginResponse Begin(absl::Span<const uint8_t> scsi_cmd,
                      scsi_defs::LunAddress lun);
  // Translates from NVMe to SCSI. Writes SCSI response data to buffer.
  ApiStatus Complete(absl::Span<const nvme_defs::GenericQueueEntryCpl> cpl_data,
                     absl::Span<uint8_t> buffer);
  // Returns a span containing translated NVMe commands.
  absl::Span<const nvme_defs::GenericQueueEntryCmd> GetNvmeCmds();
  // Aborts a given pipeline sequence and cleans up memory
  void AbortPipeline();

 private:
  StatusCode pipeline_status_ = StatusCode::kUninitialized;
  absl::Span<const uint8_t> scsi_cmd_;
  uint32_t nvme_cmd_count_;
  nvme_defs::GenericQueueEntryCmd nvme_cmds_[3];
};

}  // namespace translator

#endif