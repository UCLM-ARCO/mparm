RTEMS_MAKEFILE_PATH=/usr/lib/rtems4.6/arm-rtems/arm_bare_bsp

EXE=alloc.exe
MANAGERS=io

CSRCS = alloc.c
COBJS = $(CSRCS:.c=.o)

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(PROJECT_ROOT)/make/leaf.cfg

SRCS=$(H_FILES)
OBJS= $(COBJS) $(CXXOBJS) $(ASOBJS)

CLEAN_ADDITIONS = *.txt *.mem *~ *.exe $(EXE:.exe=)

all: $(EXE) 
	ln -sf $< TargetMem_1.mem
	ln -sf $< TargetMem_2.mem

%.exe: %.o
	$(make-exe)

