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

#ifndef NVME2SCSI_UTIL_H
#define NVME2SCSI_UTIL_H

#include <linux/kernel.h>

#ifdef __cplusplus
extern "C" {
#endif

void Print(const char* msg);

uint64_t AllocPages(uint32_t page_size, uint16_t count);

void DeallocPages(uint64_t addr, uint16_t count);

#ifdef __cplusplus
}
#endif

#endif
