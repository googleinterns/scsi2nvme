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

#ifndef LIB_NVME_H
#define LIB_NVME_H

#include <stdint.h>
#include <linux/types.h>
#include <linux/nvme_ioctl.h>
#include <sys/ioctl.h>

#include "third_party/spdk_defs/nvme_defs.h"


namespace nvme {

// Sends a passthrough command to the underlying NVMe device and returns the
// status code of the command. If the command has a data passback, the addr field 
// points towards that data on successful completion of the command. 
// Parameters:
// 1. file_descriptor: this should be an open file descriptor. This integer can
//    be retrieved by calling the open() function on the device file.
// 2. ioctl_command: this argument is the ioctl operation command which tells the
//    kernel to perform a particular IO function. These are defined in the
//    nvme_ioctl library included in the linux kernel
// 3. The rest of the arguments are the fields of the nvme_passthru command
//    described below.
//    struct nvme_passthru_cmd {
//    	__u8	opcode;
//    	__u8	flags;
//    	__u16	rsvd1;
//    	__u32	nsid;
//    	__u32	cdw2;
//    	__u32	cdw3;
//    	__u64	metadata;
//    	__u64	addr;
//    	__u32	metadata_len;
//    	__u32	data_len;
//    	__u32	cdw10;
//    	__u32	cdw11;
//    	__u32	cdw12;
//    	__u32	cdw13;
//    	__u32	cdw14;
//    	__u32	cdw15;
//    	__u32	timeout_ms;
//    	__u32	result;
//    };

int send_passthru(int file_descriptor, unsigned long ioctl_command, uint8_t opcode, uint8_t flags, uint16_t rsvd1, uint32_t nsid,
                uint32_t cdw2, uint32_t cdw3, void* metadata, void* addr,
                uint32_t metadata_len, uint32_t data_len, uint32_t cdw10,
                uint32_t cdw11, uint32_t cdw12, uint32_t cdw13, uint32_t cdw14,
                uint32_t cdw15, uint32_t timeout_ms, uint32_t* result);


} // nvme namespace


#endif // LIB_NVME_H
