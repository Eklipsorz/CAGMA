obj-m := supCenter.o
version :=$(shell uname -r)
tempobj=supCenter.ko supCenter.mod.c supCenter.mod.o supCenter.o 
all:
	rm -rf $(tempobj)
	make -C /lib/modules/$(version)/build M=$(PWD) supCenter.ko

