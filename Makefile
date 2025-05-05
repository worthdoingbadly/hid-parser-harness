# SPDX-License-Identifier: Apache-2.0

obj-m := hid-core.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
ccflags-y := -fno-sanitize=shadow-call-stack -fsanitize=address -fsanitize-coverage=trace-cmp -fno-stack-protector

default:
	$(MAKE) -C $(KDIR) M=$(PWD) SUBDIRS=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) SUBDIRS=$(PWD) clean
