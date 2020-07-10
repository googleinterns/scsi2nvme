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

#ifndef LIB_TRANSLATOR_WRITE_H_
#define LIB_TRANSLATOR_WRITE_H_

#include "third_party/spdk_defs/nvme_defs.h"

#include "common.h"
#include "lib/scsi.h"

namespace translator {

StatusCode Write6ToNvme(absl::span<const uint8_t> scsi_cmd,
                        nvme::GenericQueueEntryCmd& nvme_cmd,
                        Allocation& allocation);

StatusCode Write10ToNvme(absl::span<const uint8_t> scsi_cmd,
                         nvme::GenericQueueEntryCmd& nvme_cmd,
                         Allocation& allocation);

StatusCode Write12ToNvme(absl::span<const uint8_t> scsi_cmd,
                         nvme::GenericQueueEntryCmd& nvme_cmd,
                         Allocation& allocation);

StatusCode Write16ToNvme(absl::span<const uint8_t> scsi_cmd,
                         nvme::GenericQueueEntryCmd& nvme_cmd,
                         Allocation& allocation);

}  // namespace translator

#endif  // _WRITE_H_
