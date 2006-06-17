CROSS := arm-rtems-
#CROSS := arm-elf-

CC        := $(CROSS)gcc
LD        := $(CROSS)ld
AS        := $(CROSS)as
OBJCOPY   := $(CROSS)objcopy
OBJDUMP   := $(CROSS)objdump

CFLAGS  = -pipe -msoft-float -g -O6 -march=armv4
LDFLAGS = -g -O6 -nostartfiles
ASFLAGS = -mfpu=softfpa

.PHONY : clean

all: hello.exe
	ln -sf $< TargetMem_1.mem
	ln -sf $< TargetMem_2.mem

hello.elf: hello.o boot.o

%.exe: %.elf
	$(OBJCOPY) -O binary $< $@

%.elf: %.o
	$(LD) $(LDFLAGS) -T mpsim.ld -o $@ $^

%.dis: %.elf
	$(OBJDUMP) -S $< > $@

clean:
	$(RM) *.o *.dis *~ *.elf *.mem *.txt *.exe

