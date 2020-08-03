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

#include "engine.h"
#include "nvme_driver.h"

#include <linux/device.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/scatterlist.h>
#include <scsi/scsi.h>
#include <scsi/scsi_cmnd.h>
#include <scsi/scsi_host.h>

static const char kName[] = "SCSI2NVMe SCSI Mock";
static const int kQueueCount = 1;
static const int kCanQueue = 64;
static const int kCmdPerLun = 1;

static struct bus_type pseudo_bus;
static struct device* pseudo_root_dev;
static struct device pseudo_adapter;
static struct device_driver scsi_mock_driverfs = {.name = kName,
                                                  .bus = &pseudo_bus};

static int respond(struct scsi_cmnd* cmd, u32 resp_code) {
  cmd->result = resp_code;
  cmd->scsi_done(cmd);
  return 0;
}

static int scsi_queuecommand(struct Scsi_Host* host, struct scsi_cmnd* cmd) {
  u64 lun = cmd->device->lun;
  unsigned char* cmd_buf = cmd->cmnd;
  u16 cmd_len = cmd->cmd_len;
  u16 data_len = scsi_bufflen(cmd);
  unsigned char* sense_buf = cmd->sense_buffer;
  unsigned short sense_len = SCSI_SENSE_BUFFERSIZE;
  bool is_data_in = cmd->sc_data_direction == DMA_FROM_DEVICE;
  unsigned char data_buf[data_len];
  
  scsi_sg_copy_to_buffer(cmd, data_buf, scsi_bufflen(cmd));
  printk("RECIEVED COMMAND");
  struct ScsiToNvmeResponse resp = ScsiToNvme(cmd_buf, cmd_len, lun, sense_buf, 
    sense_len, data_buf, data_len, is_data_in);
  // Copy response to SGL buffer
  if (is_data_in) {
    printk("ALLOC_LEN %u", resp.alloc_len);
    printk("ADDITIONAL DATA %u", data_buf[5]);
    printk("PRODUCT ID: %.6s\n", &data_buf[12]);
    struct scsi_data_buffer* sdb = &cmd->sdb;
    int sdb_len = sg_copy_from_buffer(sdb->table.sgl, sdb->table.nents, data_buf, resp.alloc_len);
    scsi_set_resid(cmd, data_len - sdb_len);
  }
  return respond(cmd, resp.return_code);
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
    .can_queue = kCanQueue,
    .this_id = 7,
    .sg_tablesize = SG_MAX_SEGMENTS,
    .cmd_per_lun = kCmdPerLun};

static int bus_match(struct device* dev, struct device_driver* driver) {
  return 1;
}

static int bus_driver_probe(struct device* dev) {
  int err;
  struct Scsi_Host* scsi_host;

  printk("REGISTERING NEW DEVICE!");
  if (dev != &pseudo_adapter)
    return -1;

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
  SetEngineCallbacks();
  nvme_driver_init();
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
