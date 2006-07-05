CROSS := arm-rtems-
#CROSS := arm-elf-

vpath %.cpp ../../IceUtil
vpath %.cpp ../legacy
vpath %.cpp ../../MPS/ScrMem
vpath %.cc ..
vpath %.S asm

CC        := $(CROSS)gcc
CXX       := $(CROSS)g++
LD        := $(CROSS)ld
AS        := $(CROSS)as
OBJCOPY   := $(CROSS)objcopy
OBJDUMP   := $(CROSS)objdump

CFLAGS  = -pipe -msoft-float -g -O6 -march=armv4
CXXFLAGS = $(CFLAGS) -fno-rtti
CXXFLAGS += -I. -I../.. -I../legacy -DNDEBUG
LDFLAGS = -g -O6 -nostartfiles
ASFLAGS = -I. -D__ASSEMBLY__ -msoft-float

.PHONY : clean

all: mutex.scn sem.scn raw.scn ringbuf.scn queue.scn

include ../Make.rules


old.elf:     old.o TestCase.o compat.o delay.o memcpy.o appsupport.o boot.o
sem.elf:     sem.o TestCase.o compat.o delay.o memcpy.o boot.o
mutex.elf:   mutex.o TestCase.o compat.o delay.o memcpy.o boot.o
raw.elf:     raw.o TestCase.o compat.o delay.o memcpy.o boot.o
ringbuf.elf: ringbuf.o TestCase.o compat.o delay.o memcpy.o boot.o
queue.elf:   queue.o TestCase.o compat.o delay.o memcpy.o boot.o

%.scn: %.exe
	ln -sf $< TargetMem_1.mem
	ln -sf $< TargetMem_2.mem
	mpsim -c2 -C 2>&1 | tee $@

%.exe: %.elf
	$(OBJCOPY) -O binary $< $@

%.elf: %.o
	$(LD) $(LDFLAGS) -T mpsim.ld -o $@ $^

%.dis: %.elf
	$(OBJDUMP) -S $< > $@

clean:
	$(RM) *.o *.dis *~ *.elf *.mem *.txt *.exe

