obj-m := MemEventTrigger.o
ignoreObj = MemEventTrigger.c,Makefile,test_module
version :=$(shell uname -r)
all:
	@rm -rf `ls --ignore={$(ignoreObj)}`
	@make -C /lib/modules/$(version)/build M=$(PWD) MemEventTrigger.ko
clean:
	@rm -rf `ls --ignore={$(ignoreObj)}`
