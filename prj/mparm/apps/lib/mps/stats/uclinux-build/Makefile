AR=arm-elf-ar
CC=arm-elf-gcc
CXX=arm-elf-g++

CXXFLAGS += -Dlinux -D__linux__ -Dunix -D__uClinux__ -DEMBED
CXXFLAGS += -I. -I../.. -I../legacy -DNDEBUG
LDFLAGS  += -Wl,-elf2flt

EXE= mutex sem old ringbuf raw queue

vpath %.cpp ../../IceUtil
vpath %.cpp ../legacy
vpath %.cc ..

all: $(EXE:=.scn)

include ../Make.rules

%.scn: %
	cp $< ../../../../../runtime/romfs/bin/sh
	$(MAKE) -C ../../../../../runtime TargetMem_1.mem
	ln -sf ../../../../../runtime/TargetMem_1.mem TargetMem_1.mem
	ln -sf ../../../../../runtime/TargetMem_1.mem TargetMem_2.mem
	mpsim -c2 -C --intc=a 2>&1 | tee $@

mutex.scn:   mutex
sem.scn:     sem     
old.scn:     old     
ringbuf.scn: ringbuf 
raw.scn:     raw     
queue.scn:   queue   

mutex:   mutex.o TestCase.o compat.o
sem:     sem.o TestCase.o compat.o
old:     old.o TestCase.o compat.o appsupport.o
ringbuf: ringbuf.o TestCase.o compat.o
raw:     raw.o TestCase.o compat.o
queue:   queue.o TestCase.o compat.o

clean:
	$(RM) *~ *.o *.a *.flt *.elf2flt *.gdb *.mem *.txt
	$(RM) $(EXE) $(EXE:=.scn)
