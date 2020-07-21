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

#include "third_party/spdk/nvme.h"

#include "common.h"

namespace translator {

// SCSI has 4 Read commands: Read(6), Read(10), Read(12), Read(16)
// Each translation function takes in a raw SCSI command in bytes,
// casts it to scsi::Read[6,10,12,16]Command,
// ensures SCSI command is layed out in Big Endian using hton[sl](),
// and builds an NVMe Read command

// Read(6) is obsolete, but may still be implemented on some devices.
// As such, it will call the LegacyRead() translation function

// Read(10), Read(12), and Read(16) have essentially the same fields but with
// different memory layouts. They all call the Read() translation function,
// which calls LegacyRead() and handles some additional fields

StatusCode Read6ToNvme(Span<const uint8_t> scsi_cmd,
                       nvme::GenericQueueEntryCmd& nvme_cmd,
                       Allocation& allocation, uint32_t nsid);

StatusCode Read10ToNvme(Span<const uint8_t> scsi_cmd,
                        nvme::GenericQueueEntryCmd& nvme_cmd,
                        Allocation& allocation, uint32_t nsid);

StatusCode Read12ToNvme(Span<const uint8_t> scsi_cmd,
                        nvme::GenericQueueEntryCmd& nvme_cmd,
                        Allocation& allocation, uint32_t nsid);

StatusCode Read16ToNvme(Span<const uint8_t> scsi_cmd,
                        nvme::GenericQueueEntryCmd& nvme_cmd,
                        Allocation& allocation, uint32_t nsid);

}  // namespace translator

#endif
