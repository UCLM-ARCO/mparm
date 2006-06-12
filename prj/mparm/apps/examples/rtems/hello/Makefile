RTEMS_MAKEFILE_PATH=/usr/lib/rtems4.6/arm-rtems/arm_bare_bsp

EXEC=hello.exe
PGM=${ARCH}/$(EXEC)
MANAGERS=io

H_FILES=system.h

CSRCS = init.c
COBJS_ = $(CSRCS:.c=.o)
COBJS = $(COBJS_:%=%)

CXXSRCS =
CXXOBJS_ = $(CXXSRCS:.cpp=.o)
CXXOBJS = $(CXXOBJS_:%=%)

ASSRCS =
ASOBJS_ = $(ASSRCS:.s=.o)
ASOBJS = $(ASOBJS_:%=%)

# Libraries
LIBS = -lrtemsall -lc

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(PROJECT_ROOT)/make/leaf.cfg

SRCS=$(H_FILES)
OBJS= $(COBJS) $(CXXOBJS) $(ASOBJS)

CLEAN_ADDITIONS = *.txt *.mem *~

all:    ${ARCH} $(SRCS) $(PGM) 
	ln -sf $(PGM) TargetMem_1.mem
	ln -sf $(PGM) TargetMem_2.mem

$(PGM): $(OBJS) 
	$(make-exe)

