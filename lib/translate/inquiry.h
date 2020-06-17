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

#ifndef LIB_INQUIRY_H
#define LIB_INQUIEY_H

#include "../../third_party/spdk_defs/nvme_defs.h"
#include "../scsi_defs.h"
#include <cstdio>

namespace inquiry {

// scsi_defs::InquiryCommand* raw_to_scsi_command(uint32_t* raw_command, int length);
// uint32_t* scsi_to_nvme_command(scsi_defs::InquiryCommand*);
// uint32_t exec_nvme(uint32_t* nvme_command);
// uint32_t* nvme_to_scsi_result_SPECIFIER(uint32_t*, uint32_t*);



};

#endif