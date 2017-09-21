#
# Unix/Linux GCC Makefile for Levenberg - Marquardt minimization
# Under windows, use Makefile.vc for MSVC
#

CC=gcc
CONFIGFLAGS=#-ULINSOLVERS_RETAIN_MEMORY
#ARCHFLAGS=-march=pentium4 # YOU MIGHT WANT TO UNCOMMENT THIS FOR P4
CFLAGS=$(CONFIGFLAGS) $(ARCHFLAGS) -O3 -funroll-loops -Wall #-g #-ffast-math #-pg
LAPACKLIBS_PATH=/usr/lib64 # WHEN USING LAPACK, CHANGE THIS TO WHERE YOUR COMPILED LIBS ARE!
LDFLAGS=-L$(LAPACKLIBS_PATH) -L.
ACCDISKRCS=accDisk.c
GAMEGATERCS=GameGate.c


all: ACCDISK GAMEGATE

ACCDISK: $(ACCDISKRCS)
	test -e /usr/lib/libtolbox.so.0 || make -C toolbox
	gcc -lpthread -lm -ltolbox -Wall -O2 -g -o accDisk $(ACCDISKRCS)
GAMEGATE: $(GAMEGATERCS)
	test -e /usr/lib/libtolbox.so.0 || make -C toolbox
	gcc -lpthread -lm -ltolbox -Wall -O2 -g -o GameGate $(GAMEGATERCS)

