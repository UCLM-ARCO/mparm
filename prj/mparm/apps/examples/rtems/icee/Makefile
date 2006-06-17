RTEMS_MAKEFILE_PATH=/usr/lib/rtems4.6/arm-rtems/arm_bare_bsp

EXE=client.exe server.exe
MANAGERS=io

client.exe: CXXSRCS = client.cc Application.cc MyObj.cc compat.cc
client.exe: CXXOBJS = $(CXXSRCS:.cc=.o)

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

LINK_LIBS += -lIceE

CLEAN_ADDITIONS = *.txt *.mem *~ *.exe $(EXE:.exe=) MyObj.{h,cc}

%.exe: %.o
	$(LINK.cc) -o $(basename $@) $(LINK_OBJS) $(LINK_LIBS) -lrtems++
	$(SIZE) $(basename $@)
	$(OBJCOPY) -O binary $(basename $@) $(basename $@).exe	    

%.h %.cc: %.ice
	slice2cppe --source-ext cc $<

client.exe: client.o Application.o MyObj.o compat.o
client.o: client.cc Application.h MyObj.h
