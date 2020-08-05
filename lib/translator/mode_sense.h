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

#ifndef LIB_TRANSLATOR_MODE_SENSE_H
#define LIB_TRANSLATOR_MODE_SENSE_H

#include "common.h"

#include "third_party/spdk/nvme.h"

namespace translator {

// Mode sense 6 translates to any superset of [Identify, GetFeatures]
// Identify always comes first in the nvme_cmds span
StatusCode ModeSense6ToNvme(Span<const uint8_t> scsi_cmd,
                            Span<NvmeCmdWrapper> nvme_wrappers,
                            Allocation& allocation, uint32_t nsid,
                            uint32_t& cmd_count, uint32_t& alloc_len);

// Mode sense 10 translates to any superset of [Identify, GetFeatures]
// Identify always comes first in the nvme_cmds span
StatusCode ModeSense10ToNvme(Span<const uint8_t> scsi_cmd,
                             Span<NvmeCmdWrapper> nvme_wrappers,
                             Allocation& allocation, uint32_t nsid,
                             uint32_t& cmd_count, uint32_t& alloc_len);

StatusCode ModeSense6ToScsi(Span<const uint8_t> scsi_cmd,
                            const nvme::GenericQueueEntryCmd& identify,
                            uint32_t get_features_result, Span<uint8_t> buffer);

StatusCode ModeSense10ToScsi(Span<const uint8_t> scsi_cmd,
                             const nvme::GenericQueueEntryCmd& identify,
                             uint32_t get_features_result,
                             Span<uint8_t> buffer);

}  // namespace translator

#endif
