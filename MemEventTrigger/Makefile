obj-m := supCenter.o
ignoreObj = supCenter.c,Makefile,test_module
version :=$(shell uname -r)
tempobj=supCenter.ko supCenter.mod.c supCenter.mod.o supCenter.o 
all:
	@rm -rf $(tempobj)
	@make -C /lib/modules/$(version)/build M=$(PWD) supCenter.ko
clean:
	@rm -rf `ls --ignore={$(ignoreObj)}`
