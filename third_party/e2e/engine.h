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

// The engine is responsible for orchestrating end-to-end translation
// and provides one function ScsiToNvme()
//
// Translation Flow:
// 1. scsi_mock_module.c receives a SCSI command calls ScsiToNvme()
// 2. ScsiToNvme calls Translation::Begin()
// 3. ScsiToNvme calls Translation::GetNvmeWrappers()
// 4. ScsiToNvme sends NVMe commands to nvme_driver.c
// 5. ScsiToNvme sends NVMe responses to Translation::Complete()
// 6. ScsiToNVme returns a ScsiToNvmeResponse to scsi_mock_module.c

#ifndef ENGINE_H
#define ENGINE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __KERNEL__
#include <linux/types.h>
#endif

struct ScsiToNvmeResponse {
  int return_code;
  int alloc_len;
};

void SetEngineCallbacks(void);

struct ScsiToNvmeResponse ScsiToNvme(
    unsigned char* cmd_buf, unsigned short cmd_len, unsigned long long lun,
    unsigned char* sense_buf, unsigned short sense_len, unsigned char* data_buf,
    unsigned short data_len, bool isDataIn);

#ifdef __cplusplus
}
#endif

#endif
