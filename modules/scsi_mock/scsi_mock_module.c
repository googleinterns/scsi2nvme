#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/scatterlist.h>

#include <scsi/scsi_host.h>
#include <scsi/scsi.h>

#define NAME="SCSI2NVMe Mock"
#define QUEUE_COUNT=1

static struct bus_type pseudo_bus;

static struct device* pseudo_root_dev;

static struct device pseudo_adapter;

static struct device_driver scsi_mock_driverfs = {
  .name = NAME,
  .bus = &pseudo_bus
};

static int scsi_queuecommand(struct Scsi_Host* host, struct scsi_cmnd *cmd) {
  return 0;
}

static int scsi_abort(struct scsi_cmd* cmd) {
  return SUCCESS;
}

static struct scsi_host_template scsi_mock_template = {
  .module = THIS_MODULE,
  .name = NAME,
  .queuecommand = scsi_queuecommand,
  .eh_abort_handler = scsi_abort,
  .proc_name = NAME,
  .can_queue = 64,
  .this_id = 7,
  .sg_tablesize = SG_MAX_SEGMENTS,
  .cmd_per_lun = 1,
};

static int bus_match(struct device *dev, struct device_driver *driver) {
  return 1;
}

static int bus_driver_probe(struct device *dev) {
  int err;
  struct Scsi_Host* scsi_host;
  scsi_host = scsi_host_alloc(&scsi_mock_template, 0);
  if (!scsi_host) {
    printk("SCSI Host failed to allocate");
    return -ENODEV;
  }
  scsi_host->nr_hw_queues = QUEUE_COUNT;
  err = scsi_add_host(scsi_host, NULL);
  if (err) {
    scsi_host_put(scsi_host);
    return err;
  }
  dev_set_drvdata(dev, scsi_host);
  scsi_scan_host(scsi_host);
  return 0;
}

static int bus_remove(struct device *dev) {
  struct Scsi_Host* scsi_host = dev_get_drvdata(dev);
  scsi_remove_host(scsi_host);
  scsi_host_put(scsi_host);
  return 0;
}

static struct bus_type pseudo_bus = {
  .name = "scsi2nvme_pseudo_bus",
  .match = bus_match,
  .probe = bus_driver_probe,
  .remove = bus_remove
};

static void scsi_mock_release_device(struct device *dev) {}

static int scsi_mock_add_device(void) {
  pseudo_adapter.parent = pseudo_primary;
  pseudo_adapter.bus = &pseudo_bus;
  pseudo_adapter.release = &scsi_mock_release_device;
  return device_register(&pseudo_adapter);
}

static int __init scsi_mock_init(void) {
  printk("HELLO!\n");
  pseudo_root_dev = root_device_register("pseudo_scsi_root");
  bus_register(&pseudo_bus);
  driver_register(&scsi_mock_driverfs);
  scsi_mock_add_device();
  return 0;
}

static int __exit scsi_mock_exit(void) {
  device_unregister(&pseudo_adapter);
  driver_unregister(&scsi_mock_driverfs);
  bus_unregister(&pseudo_bus);
  root_device_unregister(pseudo_root_dev);
  printk("GOODBYE!\n");
  return 0;
}

device_initcall(scsi_mock_init);
module_exit(scsi_debug_exit);
