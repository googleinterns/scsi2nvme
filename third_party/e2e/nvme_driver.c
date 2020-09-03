#include "nvme_driver.h"
#include "nvme_internal.h"

#include <linux/bio.h>
#include <linux/blk-mq.h>
#include <linux/blk_types.h>
#include <linux/blkdev.h>
#include <linux/completion.h>
#include <linux/fs.h>
#include <linux/genhd.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/nvme.h>
#include <linux/nvme_ioctl.h>

#define MY_BDEV_MODE (FMODE_READ | FMODE_WRITE)

#define BITS_PER_SLICE 6
#define BITS_PER_WU 7
#define BITS_PER_DIE 6

struct block_device* bdev;
struct gendisk* bd_disk;
struct nvme_ns* ns;

struct nvme_request {
  struct nvme_command* cmd;
  union nvme_result result;
  u8 retries;
  u8 flags;
  u16 status;
  void* ctrl;
};

static inline struct nvme_request* nvme_req(struct request* request) {
  return blk_mq_rq_to_pdu(request);
}

static void submit_req_done(struct request* request) {
  struct nvme_request* nvme_request = nvme_req(request);

  if (request) {
    blk_mq_free_request(request);
  }
}

struct request* nvme_alloc_request(struct request_queue* queue,
                                   struct nvme_command* cmd) {
  struct request* request;
  unsigned op = nvme_is_write(cmd) ? REQ_OP_DRV_OUT : REQ_OP_DRV_IN;

  request = blk_mq_alloc_request(queue, op, 0);

  if (IS_ERR(request)) return request;

  request->cmd_flags |= REQ_FAILFAST_DRIVER;
  nvme_req(request)->retries = 0;
  nvme_req(request)->flags = 0;
  request->rq_flags |= RQF_DONTPREP;
  nvme_req(request)->cmd = cmd;

  return request;
}

int nvme_submit_user_cmd(struct gendisk* disk, struct request_queue* queue,
                         struct nvme_command* cmd, void* buffer,
                         unsigned bufflen, struct NvmeCompletion* cpl,
                         unsigned timeout) {
  struct request* request;
  struct bio* bio = NULL;
  int ret;

  if (!queue) {
    printk("Request queue is nullptr");
    printk("Identification status: %u", ns->ctrl->identified);
    printk("Queue Count: %u", ns->ctrl->queue_count);
    return ret;
  }

  request = nvme_alloc_request(queue, cmd);
  if (IS_ERR(request)) {
    printk("nvme_alloc_request failed?.");
    return PTR_ERR(request);
  }

  request->timeout = timeout ? timeout : 60 * HZ;
  request->special = cpl;

  if (buffer && bufflen) {
    ret = blk_rq_map_kern(queue, request, buffer, bufflen, GFP_KERNEL);
    if (ret) {
      printk("blk_rq_map_kern failed?.");
      goto out;
    }
    bio = request->bio;

    bio->bi_disk = disk;
    if (!bio->bi_disk) {
      printk("bdget_disk failed?.");
      ret = -ENODEV;
      goto out_unmap;
    }
  }

  blk_execute_rq(request->q, disk, request, 0);

  u16 status = nvme_req(request)->status;
  if (status == 0) {
    printk(KERN_INFO "status 0: SUCCESS");
  } else if (status < 0) {
    printk(KERN_INFO "Linux error code %d: ", status);
  } else {
    printk(KERN_INFO "NVMe error code %d: ", status);
  }
  printk(KERN_INFO "req flags %d ", nvme_req(request)->flags);
  goto out;

out_unmap:
  if (bio) {
    if (disk && bdev) bdput(bdev);
  }
out:
  submit_req_done(request);
  return ret;
}

int submit_admin_command(struct NvmeCommand* nvme_cmd, void* buffer,
                         unsigned bufflen, struct NvmeCompletion* cpl,
                         unsigned timeout) {
  struct nvme_command kernel_nvme_cmd;
  memcpy(&kernel_nvme_cmd, nvme_cmd, sizeof(kernel_nvme_cmd));
  return nvme_submit_user_cmd(bd_disk, ns->ctrl->admin_q, &kernel_nvme_cmd,
                              buffer, bufflen, cpl, timeout);
}

int submit_io_command(struct NvmeCommand* nvme_cmd, void* buffer,
                      unsigned bufflen, struct NvmeCompletion* cpl,
                      unsigned timeout) {
  struct nvme_command kernel_nvme_cmd;
  memcpy(&kernel_nvme_cmd, nvme_cmd, sizeof(kernel_nvme_cmd));
  return nvme_submit_user_cmd(bd_disk, ns->queue, &kernel_nvme_cmd, buffer,
                              bufflen, cpl, timeout);
}

int nvme_driver_init(void) {
  printk(KERN_INFO "Started NVMe Communication Module Insertion");

  bdev = blkdev_get_by_path(NVME_DEVICE_PATH, MY_BDEV_MODE, NULL);
  if (IS_ERR(bdev)) {
    printk(KERN_ERR "No such block device. %ld", PTR_ERR(bdev));
    return -1;
  }

  printk("Block device registered");

  bd_disk = bdev->bd_disk;
  if (IS_ERR_OR_NULL(bd_disk)) {
    printk("bd_disk is null?.");
    return -1;
  }

  printk("Gendisk registered");

  ns = bdev->bd_disk->private_data;
  if (IS_ERR_OR_NULL(ns)) {
    printk("nvme_ns is null?.");
    return -1;
  }
  printk("CTRL State: %u", ns->ctrl->state);
  printk("Connects_q: %p", ns->ctrl->connect_q);
  printk("Admin_q address: %p", ns->ctrl->admin_q);

  printk("CTRL POINTER %p, NS POINTER %p", ns->ctrl, ns);

  printk("NVMe device registered!");
  return 0;
}
