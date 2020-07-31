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

#include "common.h"

namespace translator {

// Reports if the user is using the API correctly.
enum class ApiStatus { kSuccess, kFailure };

struct BeginResponse {
  ApiStatus status;
  uint32_t alloc_len;  // Defines size of buffer passed to Translation::Complete
};

class Translation {
 public:
  Translation()
      : pipeline_status_(StatusCode::kUninitialized),
        nvme_cmd_count_(0),
        allocations_() {}
  // Translates from SCSI to NVMe. Translated commands available through
  // GetNvmeCmds()
  BeginResponse Begin(Span<const uint8_t> scsi_cmd, scsi::LunAddress lun);
  // Translates from NVMe to SCSI. Writes SCSI response data to buffer.
  ApiStatus Complete(Span<const nvme::GenericQueueEntryCpl> cpl_data,
                     Span<uint8_t> buffer);
  // Returns a span containing translated NVMe commands.
  Span<const NvmeCmdWrapper> GetNvmeCmds();
  // Aborts a given pipeline sequence and cleans up memory
  void AbortPipeline();

 private:
  // Releases memory vended to the translation object
  void FlushMemory();

 private:
  StatusCode pipeline_status_;
  Span<const uint8_t> scsi_cmd_;
  uint32_t nvme_cmd_count_;
  NvmeCmdWrapper nvme_cmds_[kMaxCommandRatio];
  Allocation allocations_[kMaxCommandRatio];
};

}  // namespace translator

#endif
