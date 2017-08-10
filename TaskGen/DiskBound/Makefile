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
LIBOBJS=lm.o Axb.o misc.o lmlec.o lmbc.o lmblec.o lmbleic.o
LIBSRCS=lm.c Axb.c misc.c lmlec.c lmbc.c lmblec.c lmbleic.c
DEMOBJS=lmdemo.o
DEMOSRCS=lmdemo.c
SIMUSRCS=accMem.c 623drandomNum.c PFaMstore.c CMUcalc.c
DISKBOUNDGENRCS= DiskBoundGen.c
ACCMEMRCS=accMem.c  PFaMstore.c
FIXEDACCMEM=FixedAccMem.c PFaMstore.c
CONSUMERRCS=consumer.c  PFaMstore.c
ACCDISKRCS=accDisk.c
GAMEGATERCS=GameGate.c
TESTQURCS=testqueue.c PFaMstore.c
COLLECTORCS=collector.c PFaMstore.c
AR=ar
RANLIB=ranlib
LAPACKLIBS=-llapack -lblas -lf2c -lgfortran -lm -lpthread -lrt
# comment this line if you are not using LAPACK.
# On systems with a FORTRAN (not f2c'ed) version of LAPACK, -lf2c is
# not necessary; on others, -lf2c is equivalent to -lF77 -lI77

#LAPACKLIBS=-L/usr/local/atlas/lib -llapack -lcblas -lf77blas -latlas -lf2c # This works with the ATLAS updated lapack and Linux_P4SSE2
                                                                            # from http://www.netlib.org/atlas/archives/linux/

#LAPACKLIBS=-llapack -lgoto2 -lpthread -lf2c # This works with GotoBLAS
                                             # from http://www.tacc.utexas.edu/research-development/tacc-projects/

#LAPACKLIBS=-L/opt/intel/mkl/8.0.1/lib/32/ -lmkl_lapack -lmkl_ia32 -lguide -lf2c # This works with MKL 8.0.1 from
                                            # http://www.intel.com/cd/software/products/asmo-na/eng/perflib/mkl/index.htm
                                            # check the MKL link line advisor for more information:
                                            # http://software.intel.com/en-us/articles/intel-mkl-link-line-advisor/

LIBS=$(LAPACKLIBS)
BenchMark=NPB3.3.1/NPB3.3-SER

NPBFTOBJS= $(BenchMark)/FT/ft.o $(BenchMark)/common/randi8.o \
$(BenchMark)/common/print_results.o $(BenchMark)/common/timers.o \
$(BenchMark)/common/wtime.o

NPBISOBJS= $(BenchMark)/IS/is.o $(BenchMark)/common/c_print_results.o \
$(BenchMark)/common/c_timers.o $(BenchMark)/common/c_wtime.o



#SIMUOBJS=accMem.o PFaMstore.o CMUcalc.o serverINMy.o 623drandomNum.o 

#include $(BenchMark)/config/make.def
#include $(BenchMark)/sys/


#all: liblevmar.a lmdemo CMUcalc
all: ACCDISK GAMEGATE FixedACMem
liblevmar.a: $(LIBOBJS)
	$(AR) crv liblevmar.a $(LIBOBJS)
	$(RANLIB) liblevmar.a

lmdemo: $(DEMOBJS) liblevmar.a
	$(CC) $(LDFLAGS) $(DEMOBJS) -o lmdemo -llevmar $(LIBS) -lm

CMUcalc: CMUcalc.c liblevmar.a
	$(CC) $(LDFLAGS) CMUcalc.c -o CMUcalc -llevmar $(LIBS) -lm -lpthread -ltolbox
	#$(CC) $(LDFLAGS) -c $(SIMUSRCS) -llevmar $(LIBS) 
	#$(MAKE) -C $(BenchMark) IS CLASS=S
	#$(CC) $(LDFLAGS) -mcmodel=medium $(SIMUOBJS) $(NPBISOBJS) -o CMUcalc -llevmar $(LIBS)
# IS version begin
	#$(MAKE) -C $(BenchMark) IS CLASS=W
	#$(CC) $(LDFLAGS) -fopenmp -mcmodel=medium $(SIMUOBJS) $(NPBISOBJS) -o CMUcalc -llevmar $(LIBS)
# IS version end	
# FT version begin
#	$(MAKE) -C $(BenchMark) FT CLASS=S
#	$(CC) $(LDFLAGS) -fopenmp -mcmodel=medium $(SIMUOBJS) $(NPBFTOBJS) -o CMUcalc -llevmar $(LIBS)
# FT version end


# test begin
qtest: $(TESTQURCS) 
	gcc -o qtest $^

ACCDISK: $(ACCDISKRCS)
	test -e /usr/lib/libtolbox.so.0 || make -C toolbox
	gcc -lpthread -lm -ltolbox -Wall -O2 -g -o accDisk $(ACCDISKRCS)
GAMEGATE: $(GAMEGATERCS)
	test -e /usr/lib/libtolbox.so.0 || make -C toolbox
	gcc -lpthread -lm -ltolbox -Wall -O2 -g -o GameGate $(GAMEGATERCS)
WorkGen: $(DISKBOUNDGENRCS)
	test -e /usr/lib/libtolbox.so.0 || make -C toolbox
	gcc -lm -ltolbox -Wall -O2 -g -o DiskBoundGen $(DISKBOUNDGENRCS)
# the target is based on signle process which consumes its heap memory	
FixedACMem: $(FIXEDACCMEM)
	test -e /usr/lib/libtolbox.so.0 || make -C toolbox
	gcc -lpthread -lm -ltolbox -Wall -O2 -g -o FixedAccMem $(FIXEDACCMEM)
ACCMEM: $(ACCMEMRCS) 
	test -e /usr/lib/libtolbox.so.0 || make -C toolbox
	gcc -lpthread -lm -ltolbox -Wall -O2 -g -o accMem $(ACCMEMRCS)
genPattern: $(GENPATTERNRCS)
	test -e /usr/lib/libtolbox.so.0 || make -C toolbox
	gcc -lpthread -lm -ltolbox -Wall -O2 -o genPattern $(GENPATTERNRCS)
collector: $(COLLECTORCS)
	gcc -o collector -lm -ltolbox -Wall -O2 $(COLLECTORCS)
# test end
		
# CMUcalc: that is for compiling LM algorith, which can solve nonlinear
# least square curve fitting. After obtaining the soluting, the compiled 
# program will sent a packet to process in kernel space(i.e., running module
# ). Finally,it need to communicate with Xen hypervisor via the xenstore
# when the process in kernel space receives that packet  

lm.o: lm.c lm_core.c levmar.h misc.h compiler.h
Axb.o: Axb.c Axb_core.c levmar.h misc.h
misc.o: misc.c misc_core.c levmar.h misc.h
lmlec.o: lmlec.c lmlec_core.c levmar.h misc.h
lmbc.o: lmbc.c lmbc_core.c levmar.h misc.h compiler.h
lmblec.o: lmblec.c lmblec_core.c levmar.h misc.h
lmbleic.o: lmbleic.c lmbleic_core.c levmar.h misc.h

lmdemo.o: levmar.h
#CMUcalc.o:levmar.h
clean:
	@rm -f $(LIBOBJS) $(DEMOBJS)

cleanall: clean
	@rm -f lmdemo
	@rm -f liblevmar.a

depend:
	makedepend -f Makefile $(LIBSRCS) $(DEMOSRCS)

# DO NOT DELETE THIS LINE -- make depend depends on it.

