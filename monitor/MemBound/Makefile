obj-m := MemNotifier.o
version :=$(shell uname -r)
tempobj=MemNotifier.ko MemNotifier.mod.c MemNotifier.mod.o MemNotifier.o 
all:
	rm -rf $(tempobj)
	make -C /lib/modules/$(version)/build M=$(shell pwd) MemNotifier.ko

