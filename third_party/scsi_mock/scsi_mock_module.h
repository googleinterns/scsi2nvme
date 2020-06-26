#ifndef NVME2SCSI_SCSI_MOCK_H
#define NVME2SCSI_SCSI_MOCK_H

#include <linux/device.h>

static const char kName[] = "SCSI2NVMe SCSI Mock";
static const int kQueueCount = 1;

static struct bus_type pseudo_bus;
static struct device* pseudo_root_dev;
static struct device pseudo_adapter;
static struct device_driver scsi_mock_driverfs = {.name = kName,
	                                          .bus = &pseudo_bus};

#endif
