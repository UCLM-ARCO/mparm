RTEMS_MAKEFILE_PATH=/usr/lib/rtems4.6/arm-rtems/arm_bare_bsp

EXE=threads.exe
MANAGERS=io

CXXSRCS = threads.cc compat.cc
CXXOBJS = $(CXXSRCS:.cc=.o)

all: $(EXE)
	ln -sf $< TargetMem_1.mem
	ln -sf $< TargetMem_2.mem


include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(PROJECT_ROOT)/make/leaf.cfg

GCCSPECS=-B. -specs bsp_specs -B$(PROJECT_RELEASE)/lib/ -T linkcmds

SRCS=$(H_FILES)
OBJS= $(COBJS) $(CXXOBJS) $(ASOBJS)

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
	$(LINK.cc) -o $(basename $@) $(LINK_OBJS) $(LINK_LIBS) -lrtems++
	$(SIZE) $(basename $@)
	$(OBJCOPY) -O binary $(basename $@) $(basename $@).exe	    

threads.exe: threads.o compat.o
