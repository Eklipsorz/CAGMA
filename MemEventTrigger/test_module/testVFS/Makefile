obj-m := testVFS.o
version :=$(shell uname -r)
tempobj=testVFS.ko testVFS.mod.c testVFS.mod.o testVFS.o 
all:
	rm -rf $(tempobj)
	make -C /lib/modules/$(version)/build M=$(shell pwd) testVFS.ko

