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

#ifndef LIB_TRANSLATOR_INQUIRY_H
#define LIB_TRANSLATOR_INQUIRY_H

#include <cstdio>

#include "third_party/spdk/nvme.h"

#include "common.h"

namespace translator {

// Preconditions:
// scsi_cmd is a pointer to a Inquiry Command without the OpCode
// identify_ns and identify_ctrl refers to nvme_cmds_ in the Translation object
// alloc_len refers to the allocation length field of the response object

// Postconditions:
// Fills out GenericQueueEntryCmd with appropriate Identify parameters and
// allocates PRPs for responses
StatusCode InquiryToNvme(Span<const uint8_t> scsi_cmd,
                         nvme::GenericQueueEntryCmd& identify_ns,
                         nvme::GenericQueueEntryCmd& identify_ctrl,
                         uint32_t& alloc_len, uint32_t nsid,
                         Span<Allocation> allocations);

// Preconditions:
// scsi_cmd is a pointer to a Inquiry Command without the OpCode
// nvme_cmds[0] has a PRP that points to an Identify Controller response
// nvme_cmds[1] has a PRP that points to an Identify Namespace response

// Postconditions:
// buffer contains SCSI response based on scsi_cmd parameters
StatusCode InquiryToScsi(Span<const uint8_t> scsi_cmd, Span<uint8_t> buffer,
                         Span<const nvme::GenericQueueEntryCmd> nvme_cmds);

};  // namespace translator
#endif
