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

#ifndef NVME2SCSI_SCSI_MOCK_H
#define NVME2SCSI_SCSI_MOCK_H

#define VERSION "0.1"

#include <linux/device.h>

static const char kName[] = "SCSI2NVMe SCSI Mock";
static const int kQueueCount = 1;

static struct bus_type pseudo_bus;
static struct device* pseudo_root_dev;
static struct device pseudo_adapter;
static struct device_driver scsi_mock_driverfs = {.name = kName,
                                                  .bus = &pseudo_bus};

#endif
