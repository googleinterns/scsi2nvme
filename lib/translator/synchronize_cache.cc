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

#include "synchronize_cache.h"

#include "third_party/spdk/nvme.h"

namespace translator {

// Section 5.5
// https://www.nvmexpress.org/wp-content/uploads/NVM-Express-SCSI-Translation-Reference-1_1-Gold.pdf
void SynchronizeCache10ToNvme(nvme::GenericQueueEntryCmd& nvme_cmd,
                              uint32_t nsid) {
  nvme_cmd = {.opc = static_cast<uint8_t>(nvme::NvmOpcode::kFlush),
              .nsid = nsid};
}

};  // namespace translator
