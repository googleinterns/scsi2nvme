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

// #include <host/nvme.h>

#define NAME "NVMe Mock"


static int __init nvme_mock_init(void) {
  int err;

  printk("HELLO!\n");

  struct block_device *bdev;
  bdev = lookup_bdev("/dev/nvme0");
  struct block_device_operations *fops;
  fops = bdev->bd_disk->fops;
 //fops->ioctl
  return 0;
}

static void __exit nvme_mock_exit(void) {
  printk("GOODBYE!\n");
}

module_init(nvme_mock_init);
module_exit(nvme_mock_exit);
MODULE_LICENSE("GPL");
