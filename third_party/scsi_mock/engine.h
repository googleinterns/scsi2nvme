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

#include <cstdint>

void ScsiToNvme(unsigned char* cmd_buf, unsigned short cmd_len,
  uint64_t lun, unsigned char* sense_buffer, unsigned short sense_buffer_len,
  unsigned char* data, unsigned short data_len, bool isDataIn);

#endif
