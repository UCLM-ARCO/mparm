RTEMS_MAKEFILE_PATH=/usr/lib/rtems4.6/arm-rtems/arm_bare_bsp

EXE=basics.exe
MANAGERS=io msg signal event region partition

CXXSRCS = basics.cc
CXXOBJS = $(CXXSRCS:.cc=.o)

all: $(EXE) 
	ln -sf $< TargetMem_1.mem
	ln -sf $< TargetMem_2.mem


include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(PROJECT_ROOT)/make/leaf.cfg

SRCS=$(H_FILES)
OBJS= $(COBJS) $(CXXOBJS) $(ASOBJS)

CLEAN_ADDITIONS = *.txt *.mem *~ *.exe $(EXE:.exe=)

LINK_LIBS=-lstdc++

%.exe: %.o
	$(LINK.cc) -qnolinkcmds -Tlinkcmds -o $(basename $@)\
		$(LINK_OBJS) $(LINK_LIBS) 
	$(SIZE) $(basename $@)
	$(OBJCOPY) -O binary $(basename $@) $(basename $@).exe	    

