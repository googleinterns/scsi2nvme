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


#include <linux/types.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/blkdev.h>
#include <linux/genhd.h>
#include <linux/bio.h>
#include <linux/completion.h>
#include <linux/nvme_ioctl.h>

// #include <host/nvme.h>

#define NAME "NVMe Mock"


// struct nvme_passthru_cmd64 {
// 	__u8	opcode;
// 	__u8	flags;
// 	__u16	rsvd1;
// 	__u32	nsid;
// 	__u32	cdw2;
// 	__u32	cdw3;
// 	__u64	metadata;
// 	__u64	addr;
// 	__u32	metadata_len;
// 	__u32	data_len;
// 	__u32	cdw10;
// 	__u32	cdw11;
// 	__u32	cdw12;
// 	__u32	cdw13;
// 	__u32	cdw14;
// 	__u32	cdw15;
// 	__u32	timeout_ms;
// 	__u32   rsvd2;
// 	__u64	result;
// };


static int __init nvme_mock_init(void) {
  int err;

  printk("HELLO!\n");

  struct block_device *bdev;
  bdev = lookup_bdev("/dev/nvme0");
  struct block_device_operations *fops;
  fops = bdev->bd_disk->fops;
  struct nvme_passthru_cmd64 pass_thru = {
    .opcode = 0x1,
    .flags = 0,
    .rsvd1 = 0,
    .nsid = 0,
    .cdw2 = 0,
    .cdw3 = 0,
    .metadata = 0,
    .addr = 0,
    .metadata_len = 0,
    .data_len = 0,
    .cdw10 = 0,
    .cdw11 = 0,
    .cdw12 = 0,
    .cdw13 = 0,
    .cdw14 = 0,
    .cdw15 = 0,
    .timeout_ms = 0,
    .rsvd2 = 0,
    .result = 0,
  };
  fops->ioctl(bdev, 0, NVME_IOCTL_SUBMIT_IO, &pass_thru);
  printk("Status is: %d", pass_thru.result);
  return 0;
}

static void __exit nvme_mock_exit(void) {
  printk("GOODBYE!\n");
}

module_init(nvme_mock_init);
module_exit(nvme_mock_exit);
MODULE_LICENSE("GPL");
