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

#ifndef LIB_TRANSLATOR_COMMON_H
#define LIB_TRANSLATOR_COMMON_H

#include "lib/scsi_defs.h"
#include "third_party/spdk_defs/nvme_defs.h"

namespace translator {

enum class StatusCode { kSuccess, kInvalidInput, kNoTranslation, kFailure };

void DebugLog(const char* format, ...);

void SetDebugCallback(void (*callback)(const char*));

struct ScsiContext {
  scsi_defs::LunAddress lun;
}

struct NvmeCompletionData {
  nvme_data::GenericQueueEntryCpl nvme_cpl[3];
}

class Translation {
 public:
  Translation() = default;
  StatusCode ScsiToNvme(absl::span<const uint8_t> scsi_cmd,
                        const ScsiContext& scsi_context);
  StatusCode NvmeToScsi(const NvmeCompletionData cpl_data, void* data_in);
  absl::span<const GenericSubmissionQueueCmd> GetNvmeCmds();

 private:
  absl::span<const uint8_t> scsi_cmd;
  ScsiContext scsi_context;
  uint32_t nvme_cmd_count;
  nvme_defs::GenericSubmissionQueueCmd nvme_cmds[3];
}

}  // namespace translator

#endif
