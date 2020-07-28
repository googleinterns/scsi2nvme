// Copyright 2020 Google LLC
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// version 2 as published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.

#ifndef NVME2SCSI_NVME_H
#define NVME2SCSI_NVME_H

#include "nvme_internal.h"

#include <linux/bio.h>
#include <linux/blk-mq.h>
#include <linux/blk_types.h>
#include <linux/blkdev.h>
#include <linux/completion.h>
#include <linux/fs.h>
#include <linux/genhd.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/nvme.h>
#include <linux/nvme_ioctl.h>

// TODO:basimsahaf - Need a mapping for multiple NVME devices but for now one
// fixed device is enough for an MVP
#define NVME_DEVICE_PATH "/dev/nvme0n1"

int nvme_driver_init(void);
void nvme_driver_exit(void);

int submit_admin_command(struct nvme_command* nvme_cmd, void* buffer,
                         unsigned bufflen, u32* result, unsigned timeout);
int submit_io_command(struct nvme_command* nvme_cmd, void* buffer,
                      unsigned bufflen, u32* result, unsigned timeout);

#endif  // NVME2SCSI_NVME_H
