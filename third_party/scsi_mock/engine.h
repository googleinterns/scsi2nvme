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

#ifndef NVME2SCSI_ENGINE_H
#define NVME2SCSI_ENGINE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __KERNEL__
#include <linux/types.h>
#endif

void ScsiToNvme(unsigned char* cmd_buf, unsigned short cmd_len,
  unsigned long long lun, unsigned char* sense_buf, unsigned short sense_len,
  unsigned char* data_buf, unsigned short data_len, bool isDataIn);
 
#ifdef __cplusplus
}
#endif

#endif
