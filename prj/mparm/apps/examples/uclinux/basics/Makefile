AR=arm-elf-ar
CC=arm-elf-gcc
CXX=arm-elf-g++

CFLAGS   += -Dlinux -D__linux__ -Dunix -D__uClinux__ -DEMBED
CXXFLAGS += $(CFLAGS)
LDFLAGS  += -Wl,-elf2flt

EXE=basics

all: $(EXE) TargetMem_1.mem

TargetMem_1.mem: $(EXE)
	cp $(EXE) ../../../../runtime/romfs/bin/sh
	$(MAKE) -C ../../../../runtime $@
	ln -sf ../../../../runtime/$@ .

basics: basics.o compat.o

clean:
	$(RM) *~ *.o *.a *.flt *.elf2flt *.gdb *.mem $(EXE) stats*.txt

