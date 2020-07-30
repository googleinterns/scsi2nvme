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

#ifdef __KERNEL__
#include "nvme_internal.h"
#else
struct nvme_command {
  uint8_t opcode;
  uint8_t flags;
  uint16_t command_id;
  uint32_t nsid;
  uint32_t cdw2[2];
  uint64_t metadata;
  struct {
    uint64_t prp1;
    uint64_t prp2;
  };
  uint32_t cdw3[6];
};
using u32 = uint32_t;
#endif

// TODO:basimsahaf - Need a mapping for multiple NVME devices but for now one
// fixed device is enough for an MVP
#define NVME_DEVICE_PATH "/dev/nvme0n1"

int nvme_driver_init(void);

int submit_admin_command(struct nvme_command* nvme_cmd, void* buffer,
                         unsigned bufflen, u32* result, unsigned timeout);
int submit_io_command(struct nvme_command* nvme_cmd, void* buffer,
                      unsigned bufflen, u32* result, unsigned timeout);

#endif  // NVME2SCSI_NVME_H
