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

#include "scsi_mock_module.h"

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/scatterlist.h>
#include <scsi/scsi.h>
#include <scsi/scsi_host.h>

static int scsi_queuecommand(struct Scsi_Host* host, struct scsi_cmnd* cmd) {
  printk("RECIEVED COMMAND");
  return 0;
}

static int scsi_abort(struct scsi_cmnd* cmd) { return SUCCESS; }

static const char* scsi_mock_info(struct Scsi_Host* host) {
  return "SCSI Mock Host, Version " VERSION;
}

static struct scsi_host_template scsi_mock_template = {
    .info = scsi_mock_info,
    .module = THIS_MODULE,
    .name = kName,
    .queuecommand = scsi_queuecommand,
    .eh_abort_handler = scsi_abort,
    .proc_name = kName,
    .can_queue = 64,
    .this_id = 7,
    .sg_tablesize = SG_MAX_SEGMENTS,
    .cmd_per_lun = 1};

static int bus_match(struct device* dev, struct device_driver* driver) {
  return 1;
}

static int bus_driver_probe(struct device* dev) {
  int err;
  struct Scsi_Host* scsi_host;
  scsi_host = scsi_host_alloc(&scsi_mock_template, 0);
  if (!scsi_host) {
    printk("SCSI Host failed to allocate");
    return -ENODEV;
  }
  scsi_host->nr_hw_queues = kQueueCount;
  err = scsi_add_host(scsi_host, NULL);
  if (err) {
    scsi_host_put(scsi_host);
    return err;
  }
  dev_set_drvdata(dev, scsi_host);
  scsi_scan_host(scsi_host);
  return 0;
}

static int bus_remove(struct device* dev) {
  struct Scsi_Host* scsi_host = dev_get_drvdata(dev);
  scsi_remove_host(scsi_host);
  scsi_host_put(scsi_host);
  return 0;
}

static struct bus_type pseudo_bus = {.name = "scsi2nvme_pseudo_bus",
                                     .match = bus_match,
                                     .probe = bus_driver_probe,
                                     .remove = bus_remove};

static void scsi_mock_release_device(struct device* dev) {}

static int scsi_mock_add_device(void) {
  pseudo_adapter.parent = pseudo_root_dev;
  pseudo_adapter.bus = &pseudo_bus;
  pseudo_adapter.release = &scsi_mock_release_device;
  dev_set_name(&pseudo_adapter, "scsi_mock_adapter");
  printk("Running device_register\n");
  return device_register(&pseudo_adapter);
}

static int __init scsi_mock_init(void) {
  int err;
  printk("Registering root device\n");
  pseudo_root_dev = root_device_register("pseudo_scsi_root");
  if (IS_ERR(pseudo_root_dev)) {
    printk("Error registering root dev\n");
    return -EINVAL;
  }
  printk("Registering bus\n");
  err = bus_register(&pseudo_bus);
  if (err) {
    printk("Error registering bus\n");
    root_device_unregister(pseudo_root_dev);
    return -EINVAL;
  }
  printk("Registering mock driver\n");
  err = driver_register(&scsi_mock_driverfs);
  if (err) {
    printk("Error registering driver\n");
    root_device_unregister(pseudo_root_dev);
    bus_unregister(&pseudo_bus);
    return -EINVAL;
  }
  printk("Registering mock device\n");
  err = scsi_mock_add_device();
  if (err) {
    printk("Error regsitering mock device\n");
    root_device_unregister(pseudo_root_dev);
    bus_unregister(&pseudo_bus);
    driver_unregister(&scsi_mock_driverfs);
    return -EINVAL;
  }
  printk("SUCCESS!");
  return 0;
}

static void __exit scsi_mock_exit(void) {
  device_unregister(&pseudo_adapter);
  driver_unregister(&scsi_mock_driverfs);
  bus_unregister(&pseudo_bus);
  root_device_unregister(pseudo_root_dev);
  printk("GOODBYE!\n");
}

device_initcall(scsi_mock_init);
module_exit(scsi_mock_exit);
MODULE_LICENSE("GPL");
MODULE_VERSION(VERSION);
