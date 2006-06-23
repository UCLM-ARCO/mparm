RTEMS_MAKEFILE_PATH=/usr/lib/rtems4.6/arm-rtems/arm_bare_bsp

EXE= test-recmutex.exe test-mutex.exe test-smutex.exe

vpath %.cpp ../IceUtil

all: $(EXE)
	ln -sf $< TargetMem_1.mem
	ln -sf $< TargetMem_2.mem
	ln -sf $< TargetMem_3.mem

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(PROJECT_ROOT)/make/leaf.cfg

CXXFLAGS += -I.. -DNDEBUG
MANAGERS=io msg signal event region partition timer sem
GCCSPECS=-B. -specs bsp_specs -B$(PROJECT_RELEASE)/lib/ -T linkcmds
CLEAN_ADDITIONS = *.txt *.mem *~ *.exe $(EXE:.exe=)

%.exe: %.o
	$(LINK.cc) -o $(basename $@) $^ $(LINK_OBJS) -lrtems++
	$(SIZE) $(basename $@)
	$(OBJCOPY) -O binary $(basename $@) $(basename $@).exe	    

test-mutex.exe: test-mutex.o compat.o
test-smutex.exe: test-smutex.o compat.o \
	ThreadException.o ExceptionBase.o SafeStdio.o
test-recmutex.exe: test-recmutex.o compat.o
