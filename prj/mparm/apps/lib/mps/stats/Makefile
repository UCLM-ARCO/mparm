RTEMS_MAKEFILE_PATH=/usr/lib/rtems4.6/arm-rtems/arm_bare_bsp

EXE= mutex.exe sem.exe old.exe

vpath %.cpp ../IceUtil
vpath %.cpp legacy
vpath %.c legacy

all: $(EXE:.exe=.scn)

include $(RTEMS_MAKEFILE_PATH)/Makefile.inc
include $(RTEMS_CUSTOM)
include $(PROJECT_ROOT)/make/leaf.cfg

CXXFLAGS += -I.. -Ilegacy -DNDEBUG
MANAGERS=io msg signal event region partition timer sem
GCCSPECS=-B. -specs bsp_specs -B$(PROJECT_RELEASE)/lib/ -T linkcmds
CLEAN_ADDITIONS = *.txt *.mem *~ *.exe $(EXE:.exe=) *.scn */*~

%.exe: %.o
	$(LINK.cc) -o $(basename $@) $^ $(LINK_OBJS) -lrtems++
	$(SIZE) $(basename $@)
	$(OBJCOPY) -O binary $(basename $@) $(basename $@).exe	    

%.scn: %.exe
	ln -sf $< TargetMem_1.mem
	ln -sf $< TargetMem_2.mem
	mpsim -c2 -C 2>&1 | tee $@

mutex.exe: mutex.o TestCase.o compat.o
sem.exe:   sem.o TestCase.o compat.o
old.exe:   old.o TestCase.o compat.o appsupport.o ss_semaphore.o
