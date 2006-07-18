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
LDFLAGS = -g -O6 -nostartfiles \
	--section-start .shared=0x19000000 \
	--section-start .shsem=0x20000000 \
	--section-start .scr1=0x22000000 \
	--section-start .scr2=0x22100000 \
	--section-start .scr3=0x22200000 \
	--section-start .scr4=0x22300000 \
	--section-start .scr5=0x22400000 \
	--section-start .scr6=0x22500000 \
	--section-start .scr7=0x22600000 \
	--section-start .scr8=0x22700000 \
	--section-start .scrsem1=0x22080000 \
	--section-start .scrsem2=0x22180000 \
	--section-start .scrsem3=0x22280000 \
	--section-start .scrsem4=0x22380000 \
	--section-start .scrsem5=0x22480000 \
	--section-start .scrsem6=0x22580000 \
	--section-start .scrsem7=0x22680000 \
	--section-start .scrsem8=0x22780000
ASFLAGS = -I. -D__ASSEMBLY__ -msoft-float

.PHONY : clean

all: mutex.scn sem.scn raw.scn ringbuf.scn queue.scn

include ../Make.rules

simple.elf: simple.o compat.o memcpy.o boot.o

old.elf:     old.o TestCase.o compat.o appsupport.o memcpy.o boot.o
sem.elf:     sem.o TestCase.o compat.o memcpy.o boot.o
mutex.elf:   mutex.o TestCase.o compat.o memcpy.o boot.o
raw.elf:     raw.o TestCase.o compat.o memcpy.o boot.o
ringbuf.elf: ringbuf.o TestCase.o compat.o memcpy.o boot.o
queue.elf:   queue.o TestCase.o compat.o memcpy.o boot.o

%.scn: %.exe
	ln -sf $< TargetMem_1.mem
	ln -sf $< TargetMem_2.mem
	mpsim -c2 -C --intc=a 2>&1 | tee $@

%.exe: %.elf
	$(OBJCOPY) -O binary $< $@

%.elf: %.o
	$(LD) $(LDFLAGS) -T ldscript -Map $(@:.elf=.map) -o $@ $^



simple.out: OBJS=simple.o compat.o memcpy.o boot.o
simple.out: simple.o compat.o memcpy.o boot.o

define throughput
 $(1)$(3)$(2).out: OBJS=$(1).o compat.o memcpy.o boot.o
 $(1)$(3)$(2).out: CXXFLAGS+=-DSize=$(2) -DCopy_From_$(3)
 $(1)$(3)$(2).out: $(1).o compat.o memcpy.o boot.o
endef

CHUNK_SIZES=4 8 16 32 64 128 256 512 1024 2048 4096
$(foreach size,$(CHUNK_SIZES),$(eval $(call throughput,raw,$(size),Priv)))
$(foreach size,$(CHUNK_SIZES),$(eval $(call throughput,raw,$(size),Scratch)))
$(foreach size,$(CHUNK_SIZES),$(eval $(call throughput,ringbuf,$(size),)))
$(foreach size,$(CHUNK_SIZES),$(eval $(call throughput,queue,$(size),)))


# The following target locates the stack at the end of the local scratch memory
TargetMem_%.elf:
	TARGET=$@ ; \
	$(LD) $(LDFLAGS) --section-start .stack=$$(printf %x $$(( 0x$${TARGET:10:1}80000 + 0x21f00000 )) ) \
		-T ldscript -Map $(@:.elf=.map) -o $@ $(OBJS)

TargetMem_%.mem: TargetMem_%.elf
	$(OBJCOPY) -O binary $< $@

%.out: TargetMem_1.mem TargetMem_2.mem
	mpsim -c2 -C --sfile=$(@:.out=.stats) --intc=a 2>&1 | tee $@

%.dis: %.elf
	$(OBJDUMP) -S $< > $@

clean:
	$(RM) *.o *.dis *~ *.elf *.mem *.txt *.exe
