RTEMS_MAKEFILE_PATH=/usr/lib/rtems4.6/arm-rtems/arm_bare_bsp

EXE=threads.exe
MANAGERS=io

CXXSRCS = threads.cc compat.cc
CXXOBJS = $(CXXSRCS:.cc=.o)

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(PROJECT_ROOT)/make/leaf.cfg

LINK_OBJS+= $(COBJS) $(CXXOBJS) $(ASOBJS)
LINK_LIBS+=

LDFLAGS += -L../../../icee/lib
CXXFLAGS += -ftemplate-depth-128 -Wall -D_REENTRANT \
	    -I. -I../../../icee/include \
	    -DNDEBUG -I$(RTEMS_MAKEFILE_PATH)/lib/include \
	    -Dsocklen_t=int

CLEAN_ADDITIONS = *.txt *.mem *~ *.exe $(EXE:.exe=)

all: $(EXE)
	ln -sf threads.exe TargetMem_1.mem
	ln -sf threads.exe TargetMem_2.mem

%.exe: %.o
	$(LINK.cc) -qnolinkcmds -Tlinkcmds -D_REENTRANT -o $(basename $@)\
		$(LINK_OBJS) $(LINK_LIBS) 
	$(SIZE) $(basename $@)
	$(OBJCOPY) -O binary $(basename $@) $(basename $@).exe	    

threads.exe: threads.o compat.o
