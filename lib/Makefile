# Constants

NAME := scsi2nvme

OBJS := module.o \
        cpp_entry.cc.o \
        print.o

# Mapping

$(NAME)-y := $(OBJS)

# Flags & Feature disables

cxxflags = $(FLAGS) \
           $(KBUILD_CFLAGS) \
           -fno-builtin \
           -nostdlib \
           -fno-rtti \
           -fno-exceptions \
           -std=c++0x

obj-m += $(NAME).o

ccflags-y += $(FLAGS)

# Make function def & impl

.PHONY: $(NAME).ko
$(NAME).ko:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

.PHONY: clean
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

# Define c++ compiler for .cc files

cxx-prefix := " $(HOSTCXX) [M]  "

%.cc.o: %.cc
	@echo $(cxx-prefix)$@
	@$(HOSTCXX) $(cxxflags) -c $< -o $@
