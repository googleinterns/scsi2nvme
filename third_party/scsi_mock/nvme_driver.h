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

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __KERNEL__
#include <linux/types.h>
#else
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
#endif

struct NvmeCommand {
  u8 opcode;
  u8 flags;
  u16 command_id;
  u32 nsid;
  u32 cdw2[2];
  u64 metadata;
  struct {
    u64 prp1;
    u64 prp2;
  };
  u32 cdw3[6];
};

struct NvmeCompletion {
  u32 result;
  u32 rsvd;
  u16 sq_head;
  u16 sq_id;
  u16 command_id;
  u16 status;
};

// TODO:basimsahaf - Need a mapping for multiple NVME devices but for now one
// fixed device is enough for an MVP
#define NVME_DEVICE_PATH "/dev/nvme0n1"

int nvme_driver_init(void);

int submit_admin_command(struct NvmeCommand* nvme_cmd, void* buffer,
                         unsigned bufflen, struct NvmeCompletion* cpl, unsigned timeout);
int submit_io_command(struct NvmeCommand* nvme_cmd, void* buffer,
                      unsigned bufflen, struct NvmeCompletion* cpl, unsigned timeout);
                      
int send_sample_write_request(void);

#ifdef __cplusplus
}
#endif


#endif  // NVME2SCSI_NVME_H
