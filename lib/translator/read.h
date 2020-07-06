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

#ifndef LIB_TRANSLATOR_READ_H
#define LIB_TRANSLATOR_READ_H

#include "common.h"
#include "lib/scsi_defs.h"
#include "third_party/spdk_defs/nvme_defs.h"

namespace translator {}  // namespace translator
void Read6ToNvme(uint32_t nsid, scsi_defs::Read6Command cmd,
                 nvme_defs::GenericQueueEntryCmd queue_entry);

void Read10ToNvme(uint32_t nsid, scsi_defs::Read10Command cmd,
                  nvme_defs::GenericQueueEntryCmd queue_entry);
void Read12ToNvme(uint32_t nsid, scsi_defs::Read12Command cmd,
                  absl::Span<nvme_defs::GenericQueueEntryCmd> queue_entries);

void Read16ToNvme(uint32_t nsid, scsi_defs::Read16Command cmd,
                  absl::Span<nvme_defs::GenericQueueEntryCmd> queue_entries);

void Read32ToNvme(uint32_t nsid, scsi_defs::Read32Command cmd,
                  absl::Span<nvme_defs::GenericQueueEntryCmd> queue_entries);
#endif
