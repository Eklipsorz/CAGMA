obj-m := DiskDataCollector.o
version :=$(shell uname -r)
tempobj=DiskDataCollector.ko DiskDataCollector.mod DiskDataCollector.mod.o DiskDataCollector.o 
all:
	rm -rf $(tempobj)
	make -C /lib/modules/$(version)/build M=$(shell pwd) DiskDataCollector.ko

