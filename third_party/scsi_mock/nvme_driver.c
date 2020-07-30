#include "nvme_driver.h"

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

#define BITS_PER_SLICE 	6
#define BITS_PER_WU 	7
#define BITS_PER_DIE	6

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

    bio->bi_disk = disk;
    if (!bio->bi_disk) {
      printk("bdget_disk failed?.\n");
      ret = -ENODEV;
      goto out_unmap;
    }
  }

  printk("Before block request execution.\n");
  blk_execute_rq(req->q, disk, req, 0);
  printk(KERN_INFO "status %d\n", nvme_req(req)->status);
  printk(KERN_INFO "req flags %d\n", nvme_req(req)->flags);

out_unmap:
  if (bio) {
    if (disk && bdev) bdput(bdev);
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

int nvme_driver_init(void) {
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
  }

  printk("Gendisk registered\n");

  ns = bdev->bd_disk->private_data;
  if (IS_ERR_OR_NULL(ns)) {
    printk("nvme_ns is null?.\n");
  }
  printk("NVMe device registered!\n");

  int buff_size = sizeof(struct nvme_id_ctrl);
  void *ret_buf = NULL;
  ret_buf = kzalloc(buff_size, GFP_ATOMIC | GFP_KERNEL);
  if (!ret_buf)
  {
    printk("Failed to malloc?.\n");
  }
  char * cbuff = ret_buf;
  cbuff[0] = 'a';
  cbuff[1] = 'b';
  cbuff[2] = 'c';
  cbuff[3] = '\n';

  struct nvme_command *ncmd = NULL;
  ncmd = kzalloc (sizeof (struct nvme_command), GFP_KERNEL);

  memset(ncmd, 0, sizeof(&ncmd));
  ncmd->common.opcode = nvme_cmd_write;
  ncmd->rw.nsid = cpu_to_le32(1);
  ncmd->rw.length = cpu_to_le16(1);

  u32 code_result = 0;

  int status = submit_io_command(ncmd, ret_buf, buff_size, &code_result, 0);
  printk("Status of IO is: %d\n", status);

  // uncomment while reading the value
  u8 written_value = 0;
  memcpy(&written_value, &ret_buf, 1);
  printk("Value written is: %d\n", written_value);
  return 0;
}
