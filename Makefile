# Constants

NAME := scsi2nvme

MODULE_SRC_DIR := third_party/scsi_mock
TRANSLATE_SRC_DIR := lib/translator
NVME_SRC_DIR := third_party/spdk
SCSI_SRC_DIR := lib
TRANSLATION_SRC_DIR := lib/translator

OBJS := $(MODULE_SRC_DIR)/scsi_mock_module.o \
	$(MODULE_SRC_DIR)/util.o \
	$(MODULE_SRC_DIR)/engine.cc.o \
	$(MODULE_SRC_DIR)/nvme_driver.o \
	$(TRANSLATION_SRC_DIR)/common.cc.o \
	$(TRANSLATION_SRC_DIR)/inquiry.cc.o \
	$(TRANSLATION_SRC_DIR)/read_capacity_10.cc.o \
	$(TRANSLATION_SRC_DIR)/request_sense.cc.o \
	$(TRANSLATION_SRC_DIR)/status.cc.o \
	$(TRANSLATION_SRC_DIR)/translation.cc.o

INC := -I$(PWD)/third_party -I$(PWD)

$(NAME)-y := $(OBJS)

#Flags & Feature Disable

cxxflags = $(FLAGS) \
	   $(KBUILD_CFLAGS) \
	   -Iinclude \
	   -fno-builtin \
	   -nostdlib \
	   -fno-rtti \
	   -fno-exceptions \
	   -std=c++17

obj-m += $(NAME).o

cc-flags-y += $(FLAGS)
ccflags-y := -std=gnu99 -Wno-declaration-after-statement

# Function def

.PHONY: $(NAME).ko
$(NAME).ko:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

.PHONY: clean
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

# C++ compiler

CC := gcc

cxx-prefix := " $(HOSTCXX) [M] "

%.cc.o: %.cc
	@echo $(cxx-prefix)$@
	@$(HOSTCXX) $(cxxflags) $(INC) -c $< -o $@
