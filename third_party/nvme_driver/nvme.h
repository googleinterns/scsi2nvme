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

#define MODULE_VERSION "0.1"
#define MODULE_AUTHOR "wnukowski@google.com"
#define MODULE_LICENSE "GPL"
#define MODULE_DESCRIPTION "Kernel module to communicate with NVMe devices."

// TODO:basimsahaf - Need a mapping for multiple NVME devices but for now one 
// fixed device is enough for an MVP
#define NVME_DEVICE_PATH "/dev/nvme0n1"

#endif // NVME2SCSI_NVME_H
