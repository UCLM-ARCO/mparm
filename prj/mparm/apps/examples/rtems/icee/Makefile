RTEMS_MAKEFILE_PATH=/usr/lib/rtems4.6/arm-rtems/arm_bare_bsp

EXE=client.exe server.exe
MANAGERS=io

client.exe: CXXSRCS = client.cc MyObj.cc Application.cc compat.cc
client.exe: CXXOBJS = $(CXXSRCS:.cc=.o)

server.exe: CXXSRCS = server.cc MyObj.cc Application.cc compat.cc
server.exe: CXXOBJS = $(CXXSRCS:.cc=.o)

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(PROJECT_ROOT)/make/leaf.cfg

SRCS=$(H_FILES)
LINK_OBJS= $(COBJS) $(CXXOBJS) $(ASOBJS)
LINK_LIBS=-lIceE -lstdc++

LDFLAGS += -L../../../icee/lib
CXXFLAGS += -ftemplate-depth-128 -Wall -D_REENTRANT \
	    -I. -I../../../icee/include \
	    -I$(RTEMS_MAKEFILE_PATH)/lib/include \
	    -DNDEBUG -Dsocklen_t=int

CLEAN_ADDITIONS = *.txt *.mem *~ *.exe $(EXE:.exe=) MyObj.{h,cc}

all: $(EXE) 
	ln -sf client.exe TargetMem_1.mem
	ln -sf server.exe TargetMem_2.mem

%.h %.cc: %.ice
	slice2cppe --source-ext=cc $<

%.exe: %.o
	$(LINK.cc) -qnolinkcmds -Tlinkcmds -D_REENTRANT -o $(basename $@)\
		$(LINK_OBJS) $(LINK_LIBS) 
	$(SIZE) $(basename $@)
	$(OBJCOPY) -O binary $(basename $@) $(basename $@).exe	    

client.exe: client.o MyObj.o Application.o compat.o ../../../icee/lib/libIceE.a
server.exe: server.o MyObj.o Application.o compat.o
client.o: client.cc MyObj.h
server.o: server.cc MyObj.h
MyObj.o: MyObj.cc MyObj.h
