#
# Makefile for led drivers.
#
#obj-$(CONFIG_MY_MAIN) += my_timer.o
FILENAME ?= client
ifeq ($(FILENAME),client)
	obj-m += mynotifier-client.o
else
	obj-m += mynotifier-server.o
endif

KERNELDIR := ~/study/cxsw6652/kernel/linux-3.10.y/
CURRENT_PATH := $(shell pwd)
ARCH := arm64
CROSS_COMPILE := ~/study/cxsw6652/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9/bin/aarch64-linux-android-
build: kernel_modules

kernel_modules:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules
clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean
