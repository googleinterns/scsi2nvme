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

#ifndef LIB_LUN_TRANSLATOR_H
#define LIB_LUN_TRANSLATOR_H

#include "common.h"

namespace translator {

StatusCode ReportLunsToNvme(Span<const uint8_t> scsi_cmd,
                            NvmeCmdWrapper& nvme_wrapper, uint32_t page_size,
                            Allocation& allocation, uint32_t& alloc_len);

StatusCode ReportLunsToScsi(const nvme::GenericQueueEntryCmd& identify_cmd,
                            Span<uint8_t> buffer);

};  // namespace translator

#endif
