obj-m += nvme_mock_module.o
ccflags-y := -I $(srctree)/drivers/nvme

all:
	make -C LINUX_SRC_PATH M=$(PWD) modules

install:
	make -C LINUX_SRC_PATH M=$(PWD) modules_install

clean:
	make -C LINUX_SRC_PATH M=$(PWD) clean
