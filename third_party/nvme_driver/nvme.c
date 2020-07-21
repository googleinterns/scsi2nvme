#include "nvme.h"

MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
MODULE_AUTHOR("wnukowski@google.com")
MODULE_DESCRIPTION("Kernel module to talk to NVME devices")

#define MY_BDEV_MODE (FMODE_READ | FMODE_WRITE)
struct block_device* bdev;
struct gendisk* bd_disk;
struct nvme_ns* ns;

union nvme_result {
  __le16 u16;
  __le32 u32;
  __le64 u64;
};

struct nvme_request {
  struct nvme_command* cmd;
  union nvme_result result;
  u8 retries;
  u8 flags;
  u16 status;
  void* ctrl;
};

struct request* nvme_alloc_request(struct request_queue* q,
                                   struct nvme_command* cmd) {
  struct request* req;

  req = blk_mq_alloc_request(q, nvme_is_write(cmd), 0);
  if (IS_ERR(req)) return req;

  req->cmd_type = REQ_TYPE_DRV_PRIV;
  req->cmd = (unsigned char*)cmd;
  req->cmd_len = sizeof(struct nvme_command);
  req->errors = 0;

  return req;
}

static inline struct nvme_request* nvme_req(struct request* req) {
  return blk_mq_rq_to_pdu(req);
}

int nvme_submit_user_cmd(struct gendisk* disk, struct request_queue* q,
                         struct nvme_command* cmd, void* buffer,
                         unsigned bufflen, u32* result, unsigned timeout) {
  struct nvme_completion cqe;
  struct request* req;
  struct bio* bio = NULL;
  int ret;
  req = nvme_alloc_request(q, cmd);
  if (IS_ERR(req)) {
    printk("nvme_alloc_request failed?.\n");
    return PTR_ERR(req);
  }

  req->timeout = timeout ? timeout : 60 * HZ;
  req->special = &cqe;

  if (buffer && bufflen) {
    ret = blk_rq_map_kern(q, req, buffer, bufflen, GFP_KERNEL);
    if (ret) {
      printk("blk_rq_map_kern failed?.\n");
      goto out;
    }
    bio = req->bio;

    bio->bi_bdev = bdget_disk(disk, 0);
    if (!bio->bi_bdev) {
      printk("bdget_disk failed?.\n");
      ret = -ENODEV;
      goto out_unmap;
    }
  }

  printk("Before block request execution.\n");
  int req_res = blk_execute_rq(req->q, disk, req, 0);
  printk(KERN_INFO "req_res %d\n", req_res);
  printk(KERN_INFO "status %d\n", nvme_req(req)->status);
  printk(KERN_INFO "req flags %d\n", nvme_req(req)->flags);
  ret = req->errors;
  if (result) *result = le32_to_cpu(cqe.result);
out_unmap:
  if (bio) {
    if (disk && bio->bi_bdev) bdput(bio->bi_bdev);
  }
out:
  blk_mq_free_request(req);
  return ret;
}

int submit_admin_command(struct nvme_command* nvme_cmd, void* buffer,
                         unsigned bufflen, u32* result, unsigned timeout) {
  return nvme_submit_user_cmd(bd_disk, ns->ctrl->admin_q, nvme_cmd, buffer,
                              bufflen, result, timeout);
}

int submit_io_command(struct nvme_command* nvme_cmd, void* buffer,
                      unsigned bufflen, u32* result, unsigned timeout) {
  return nvme_submit_user_cmd(bd_disk, ns->queue, nvme_cmd, buffer, bufflen,
                              result, timeout);
}

static int __init nvme_communication_init(void) {
  printk(KERN_INFO "Started NVMe Communication Module Insertion\n");

  bdev = blkdev_get_by_path(NVME_DEVICE_PATH, MY_BDEV_MODE, NULL);
  if (IS_ERR(bdev)) {
    printk(KERN_ERR "No such block device. %ld\n", PTR_ERR(bdev));
    return -1;
  }

  printk("Block device registered\n");

  bd_disk = bdev->bd_disk;
  if (IS_ERR_OR_NULL(bd_disk)) {
    printk("bd_disk is null?.\n");
    goto err;
  }

  printk("Gendisk registered\n");

  ns = bdev->bd_disk->private_data;
  if (IS_ERR_OR_NULL(ns)) {
    printk("nvme_ns is null?.\n");
    goto err;
  }

  printk("Nvme_ns registered\n");

err:
  if (ncmd) {
    kfree(ncmd);
  }
  if (ret_buf) {
    kfree(ret_buf);
  }
  return 0;
}
static void __exit nvme_communication_exit(void) {
  printk(KERN_INFO "Exiting NVMe Communication module\n");
}

module_init(nvme_communication_init);
module_exit(nvme_communication_exit);
