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

#ifndef LIB_TRANSLATOR_UNMAP_H
#define LIB_TRANSLATOR_UNMAP_H

#include "common.h"

#include "third_party/spdk/nvme.h"

namespace translator {

// Translates Unmap to the NVMe Dataset Management command
// Buffer_out is a SCSI data buffer containing variable-length cmd data.
StatusCode UnmapToNvme(Span<const uint8_t> scsi_cmd,
                       Span<const uint8_t> buffer_out,
                       NvmeCmdWrapper& nvme_wrapper, Allocation& allocation,
                       uint32_t nsid);

}  // namespace translator
#endif
