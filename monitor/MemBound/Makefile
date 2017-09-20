obj-m := MemDataCollector.o
version :=$(shell uname -r)
tempobj=MemDataCollector.ko MemDataCollector.mod.c MemDataCollector.mod.o MemDataCollector.o 
all:
	rm -rf $(tempobj)
	make -C /lib/modules/$(version)/build M=$(shell pwd) MemDataCollector.ko

