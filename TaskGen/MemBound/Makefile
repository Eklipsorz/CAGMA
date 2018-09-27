
CC=gcc
CONFIGFLAGS=#-ULINSOLVERS_RETAIN_MEMORY
#ARCHFLAGS=-march=pentium4 # YOU MIGHT WANT TO UNCOMMENT THIS FOR P4
CFLAGS=$(CONFIGFLAGS) $(ARCHFLAGS) -O3 -funroll-loops -Wall #-g #-ffast-math #-pg
LAPACKLIBS_PATH=/usr/lib64 # WHEN USING LAPACK, CHANGE THIS TO WHERE YOUR COMPILED LIBS ARE!
FIXEDACCMEM=FixedAccMem.c
FileSuffexSetterRCS=FileSuffexSetter.c

all: FixedACMem FILESUFFEXSETTER
FixedACMem: $(FIXEDACCMEM)
	test -e /usr/lib/libtolbox.so.0 || make -C toolbox
	gcc -lpthread -lm -ltolbox -Wall -O2 -g -o FixedAccMem $(FIXEDACCMEM)
FILESUFFEXSETTER: $(FileSuffexSetterRCS)
	test -e /usr/lib/libtolbox.so.0 || make -C toolbox
	gcc -lpthread -lm -ltolbox -Wall -O2 -g -o FileSuffexSetter $(FileSuffexSetterRCS)
		
