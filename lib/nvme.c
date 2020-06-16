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

#include "nvme.h"


int send_passthru(int file_descriptor, unsigned long ioctl_command, uint8_t opcode, uint8_t flags, uint16_t rsvd1, uint32_t nsid,
                uint32_t cdw2, uint32_t cdw3, void* metadata, void* addr,
                uint32_t metadata_len, uint32_t data_len, uint32_t cdw10,
                uint32_t cdw11, uint32_t cdw12, uint32_t cdw13, uint32_t cdw14,
                uint32_t cdw15, uint32_t timeout_ms, uint32_t* result) {
    
    struct nvme_passthru_cmd passthru_cmd = {
        .opcode = opcode,
        .flags = flags,
        .rsvd1 = rsvd1,
        .nsid = nsid,
        .cdw2 = cdw2,
        .cdw3 = cdw3,
        .metadata = (__u64)(uintptr_t) metadata,
        .addr = (__u64)(uintptr_t) addr,
        .metadata_len = metadata_len,
        .data_len = data_len,
        .cdw10 = cdw10,
        .cdw11 = cdw11,
        .cdw12 = cdw12,
        .cdw13 = cdw13,
        .cdw14 = cdw14,
        .cdw15 = cdw15,
        .timeout_ms = timeout_ms,
        .result = 0,
    };

    int error;

    error = ioctl(file_descriptor, ioctl_command, &passthru_cmd);
    if(!error && result) {
        *result = passthru_cmd.result;
    } 

    return error;
}
