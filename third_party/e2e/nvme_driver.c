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

static inline struct nvme_request* nvme_req(struct request* req) {
  return blk_mq_rq_to_pdu(req);
}

static void submit_req_done(struct request* rq) {
  struct nvme_request* nvme_rq = nvme_req(rq);

  if (rq) {
    blk_mq_free_request(rq);
  }
}

struct request* nvme_alloc_request(struct request_queue* q,
                                   struct nvme_command* cmd) {
  struct request* req;
  unsigned op = nvme_is_write(cmd) ? REQ_OP_DRV_OUT : REQ_OP_DRV_IN;

  req = blk_mq_alloc_request(q, op, 0);

  if (IS_ERR(req)) return req;

  req->cmd_flags |= REQ_FAILFAST_DRIVER;
  nvme_req(req)->retries = 0;
  nvme_req(req)->flags = 0;
  req->rq_flags |= RQF_DONTPREP;
  nvme_req(req)->cmd = cmd;

  return req;
}

int nvme_submit_user_cmd(struct gendisk* disk, struct request_queue* q,
                         struct nvme_command* cmd, void* buffer,
                         unsigned bufflen, struct NvmeCompletion* cpl,
                         unsigned timeout) {
  struct request* req;
  struct bio* bio = NULL;
  int ret;

  if (!q) {
    printk("Request q is nullptr");
    printk("Identification status: %u", ns->ctrl->identified);
    printk("Queue Count: %u", ns->ctrl->queue_count);
    return ret;
  }

  req = nvme_alloc_request(q, cmd);
  if (IS_ERR(req)) {
    printk("nvme_alloc_request failed?.");
    return PTR_ERR(req);
  }

  req->timeout = timeout ? timeout : 60 * HZ;
  req->special = cpl;

  if (buffer && bufflen) {
    ret = blk_rq_map_kern(q, req, buffer, bufflen, GFP_KERNEL);
    if (ret) {
      printk("blk_rq_map_kern failed?.");
      goto out;
    }
    bio = req->bio;

    bio->bi_disk = disk;
    if (!bio->bi_disk) {
      printk("bdget_disk failed?.");
      ret = -ENODEV;
      goto out_unmap;
    }
  }

  blk_execute_rq(req->q, disk, req, 0);

  u16 status = nvme_req(req)->status;
  if (status == 0) {
    printk(KERN_INFO "status 0: SUCCESS");
  } else if (status < 0) {
    printk(KERN_INFO "Linux error code %d: ", status);
  } else {
    printk(KERN_INFO "NVMe error code %d: ", status);
  }
  printk(KERN_INFO "req flags %d ", nvme_req(req)->flags);
  goto out;

out_unmap:
  if (bio) {
    if (disk && bdev) bdput(bdev);
  }
out:
  submit_req_done(req);
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
