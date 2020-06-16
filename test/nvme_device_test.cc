// Copyright 2020 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <fcntl.h>
#include <iostream>

#include "lib/nvme.h"
#include "third_party/spdk_defs/nvme_defs.h"
#include "absl/base/casts.h"
#include "gtest/gtest.h"


// Tests

namespace {

// TEST(NVMeDevice, ShouldReturnZeroOnSuccessfulPassthru) {
//     int file_descriptor = open("", O_RDWR);
//     std::cout << file_descriptor << std::endl;
//     int nvme_code = NVME_IOCTL_IO_CMD;
//     uint8_t opcode = (uint8_t) nvme_defs::NvmOpcode::kRead;
//     uint8_t flags = 0x0;
//     uint16_t rsvd1 = 0x0;
//     uint32_t nsid = 0x0;
//     uint32_t cdw2 = 0x0;
//     uint32_t cdw3 = 0x0; 
//     void* metadata = NULL;
//     void* addr = NULL;
//     uint32_t metadata_len = 0x0;
//     uint32_t data_len = 0x0;
//     uint32_t cdw10 = 0x0;
//     uint32_t cdw11 = 0x0;
//     uint32_t cdw12 = 0x0;
//     uint32_t cdw13 = 0x0;
//     uint32_t cdw14 = 0x0;
//     uint32_t cdw15 = 0x0;
//     uint32_t timeout_ms = 0x0;
//     uint32_t* result = 0;

//     int status = nvme::send_passthru(file_descriptor, nvme_code, opcode, flags, rsvd1, nsid, 
//     cdw2, cdw3, metadata, addr, metadata_len, data_len, cdw10, cdw11, cdw12, 
//     cdw13, cdw14, cdw15, timeout_ms, result);

//     EXPECT_EQ(status, 0);
}

};  // namespace
