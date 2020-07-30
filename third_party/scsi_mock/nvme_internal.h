#ifndef _NVME_INTERNAL_H
#define _NVME_INTERNAL_H

#include <linux/version.h>
#include <linux/nvme.h>
#include <linux/pci.h>
#include <linux/kref.h>
#include <linux/blk-mq.h>

enum nvme_ctrl_state {
	NVME_CTRL_NEW,
	NVME_CTRL_LIVE,
	NVME_CTRL_RESETTING,
	NVME_CTRL_RECONNECTING,
	NVME_CTRL_DELETING,
	NVME_CTRL_DEAD,
};

// What is going on here is probably one big antipattern.

struct nvme_ctrl {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,0,0)
	bool comp_seen;
#endif

	enum nvme_ctrl_state state;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,11,0)
	bool identified;
#endif

	spinlock_t lock;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,0,0)
	struct mutex scan_lock;
#endif

	const struct nvme_ctrl_ops *ops;
	struct request_queue *admin_q;
	struct request_queue *connect_q;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,4,0)
	struct request_queue *fabrics_q;
#endif

	struct device *dev;
	struct kref kref;
	int instance;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,0,0)
	int numa_node;
#endif

	struct blk_mq_tag_set *tagset;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,14,0)
	struct blk_mq_tag_set *admin_tagset;
#endif

	struct list_head namespaces;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0)
	struct rw_semaphore namespaces_rwsem;
#else
	struct mutex namespaces_mutex;
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,15,0)
	struct device ctrl_device;
#endif

	struct device *device;	/* char device */

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,15,0)
	//struct cdev cdev;
#else
	struct list_head node;
	struct ida ns_ida;
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,13,0)
	struct work_struct reset_work;
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,15,0)
	struct work_struct delete_work;

	struct nvme_subsystem *subsys;
	struct list_head subsys_entry;
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,11,0)
	struct opal_dev *opal_dev;
#endif

	char name[12];
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
	char serial[20];
	char model[40];
	char firmware_rev[8];
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,13,0)
	char subnqn[NVMF_NQN_SIZE];
#endif
#endif

	u16 cntlid;

	u32 ctrl_config;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,14,0)
	u16 mtfa;
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,13,0)
	u32 queue_count;

	u64 cap;
#endif
	u32 page_size;
	u32 max_hw_sectors;
	u16 oncs;
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
	u16 vid;
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,11,0)
	u16 oacs;
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,13,0)
	u16 nssa;
	u16 nr_streams;
#endif

	atomic_t abort_limit;
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
	u8 event_limit;
#endif
	u8 vwc;
	u32 vs;
	u32 sgls;
	u16 kas;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,11,0)
	u8 npss;
	u8 apsta;
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,15,0)
	u32 aen_result;
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,14,0)
	unsigned int shutdown_timeout;
#endif

	unsigned int kato;
	bool subsystem;
	unsigned long quirks;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,11,0)
	struct nvme_id_power_state psd[32];
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,15,0)
	struct nvme_effects_log *effects;
#endif
	struct work_struct scan_work;
	struct work_struct async_event_work;
	struct delayed_work ka_work;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,16,0)
	struct nvme_command ka_cmd;
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,14,0)
	struct work_struct fw_act_work;
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,12,0)
	/* Power saving configuration */
	u64 ps_max_latency_us;
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,13,0)
	bool apst_enabled;

	/* PCIe only: */
	u32 hmpre;
	u32 hmmin;
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,14,0)
	u32 hmminds;
	u16 hmmaxd;

#endif
	/* Fabrics only */
	u16 sqsize;
	u32 ioccsz;
	u32 iorcsz;
	u16 icdoff;
	u16 maxcmd;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,13,0)
	int nr_reconnects;
#endif
	struct nvmf_ctrl_options *opts;
};

struct nvme_ns {
	struct list_head list;

	struct nvme_ctrl *ctrl;
	struct request_queue *queue;
	struct gendisk *disk;
#ifdef CONFIG_NVME_MULTIPATH
	enum nvme_ana_state ana_state;
	u32 ana_grpid;
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,15,0)
	struct list_head siblings;
#endif
	struct nvm_dev *ndev;
	struct kref kref;
	int instance;

	u8 eui[8];
	u8 uuid[16];

	unsigned ns_id;
	int lba_shift;
	u16 ms;
	bool ext;
	u8 pi_type;
	unsigned long flags;

#define NVME_NS_REMOVING 0
#define NVME_NS_DEAD     1
#define NVME_NS_ANA_PENDING	2

	u64 mode_select_num_blocks;
	u32 mode_select_block_len;
};

#endif /* _NVME_H */
