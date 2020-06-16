/*
 * Entry to kernel module
 */

#include <linux/kernel.h>
#include <linux/module.h>

#include "nvme.h"
#include "c_entry.h"

MODULE_DESCRIPTION("SCSI to NVME kernel module");
MODULE_VERSION("0.1");
MODULE_AUTHOR("Google");
MODULE_LICENSE("Apache 2.0");

static int __init moduleInit(void) {
  printk("Initializing scsi2nvme kernel module\n");
  init();
  return 0;
}

static void __exit moduleExit(void) {
  printk("Exiting scsi2nvme kernel module\n");
  release();
}

module_init(moduleInit);
module_exit(moduleExit);
