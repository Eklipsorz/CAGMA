obj-m := DiskNotifier.o
version :=$(shell uname -r)
tempobj=DiskNotifier.ko DiskNotifier.mod.c DiskNotifier.mod.o DiskNotifier.o 
all:
	rm -rf $(tempobj)
	make -C /lib/modules/$(version)/build M=$(shell pwd) DiskNotifier.ko

