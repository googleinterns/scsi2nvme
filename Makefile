# Constants

NAME := scsi2nvme

MODULE_SRC_DIR := third_party/scsi_mock
TRANSLATE_SRC_DIR := lib/translator
NVME_SRC_DIR := third_party/spdk
SCSI_SRC_DIR := lib

OBJS := $(MODULE_SRC_DIR)/scsi_mock_module.o \
	$(MODULE_SRC_DIR)/engine.cc.o

INC := -I$(PWD)

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
