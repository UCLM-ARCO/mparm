RTEMS_MAKEFILE_PATH=/usr/lib/rtems4.6/arm-rtems/arm_bare_bsp
EXE=hello.exe
MANAGERS=io

CXXSRCS = hello.cc
CXXOBJS = $(CXXSRCS:.cc=.o)

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(PROJECT_ROOT)/make/leaf.cfg

GCCSPECS=-B. -specs bsp_specs -B$(PROJECT_RELEASE)/lib/ -T linkcmds

SRCS=$(H_FILES)
OBJS= $(COBJS) $(CXXOBJS) $(ASOBJS)

CLEAN_ADDITIONS = *.txt *.mem *~ *.exe $(EXE:.exe=)

all: $(EXE) 
	ln -sf $< TargetMem_1.mem
	ln -sf $< TargetMem_2.mem

%.exe: %.o
	$(LINK.cc) -v -o $(basename $@) $(LINK_OBJS) $(LINK_LIBS) -lrtems++
	$(SIZE) $(basename $@)
	$(OBJCOPY) -O binary $(basename $@) $(basename $@).exe	    

