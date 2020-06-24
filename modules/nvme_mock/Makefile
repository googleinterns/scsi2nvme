obj-m += nvme_mock_module.o
ccflags-y := -I $(srctree)/drivers/nvme

all:
	make -C ~/Desktop/workspace/scsi2nvme/modules/nvme_mock/linux-5.6.14 M=$(PWD) modules

install:
	make -C ~/Desktop/workspace/scsi2nvme/modules/nvme_mock/linux-5.6.14 M=$(PWD) modules_install

clean:
	make -C ~/Desktop/workspace/scsi2nvme/modules/nvme_mock/linux-5.6.14 M=$(PWD) clean
