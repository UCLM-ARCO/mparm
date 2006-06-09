AR=arm-elf-ar
CC=arm-elf-gcc
CXX=arm-elf-g++
CFLAGS=-static -msoft-float -O2 -DEMBED -D__uClinuc__ -Dlinux -D__linux__
CXXFLAGS= $(CFLAGS) -I/usr/local/arm-elf/include -I/usr/local/arm-elf/stlport
LDFLAGS=$(CFLAGS) -Wl,-elf2flt

#AR=arm-uclinux-ar
#CC=arm-uclinux-gcc
#CFLAGS=-g -static -msoft-float -mcpu=arm610 -O0
#LDFLAGS=$(CFLAGS) -Wl,-elf2flt

#AR=arm-linux-uclibc-ar
#LD=arm-linux-uclibc-ld
#CC=arm-linux-uclibc-gcc
#CFLAGS=-g -static -msoft-float -mcpu=arm7 -O0
#LDFLAGS=
#LDLIBS=-lc

SRCS =	scratch_semaphore.h config.h \
	scratch_queue.h scratch_queue.c \
	appsupport.h appsupport.c

EXE = producer consumer testque testshm id

CSRC = $(filter %.c,$(SRCS))
OBJ  =$(CSRC:.c=.o)


all: TargetMem_1.mem

libque.a: $(OBJ)
	$(AR) rcs $@ $(filter %.o,$^)

producer: LDLIBS=-lstdc++
producer: producer.o

consumer: LDLIBS=-L. -lque
consumer: consumer.o libque.a

testque: LDLIBS=-lstdc++ -L. -lque
testque: testque.o

testshm: LDLIBS=-lstdc++
testshm: testshm.o appsupport.o

TargetMem_1.mem: $(EXE)
	cp $(EXE) ../../runtime/romfs/bin/
	$(MAKE) -C ../../runtime $@
	ln -sf ../../runtime/$@ .

clean:
	$(RM) *~ *.o *.a *.flt *.elf2flt *.gdb *.mem $(EXE)


%.elf2flt: %.o
	$(LD) $(LDFLAGS) -r -d -o $@ $^

%.elf: %.elf2flt
	$(LD) $(LDFLAGS) -T elf2flt.ld -Ur -o $@ $<

%.gdb: %.elf2flt
	$(LD) $(LDFLAGS) -T elf2flt.ld -o $@ $< $(LDLIBS)

%.flt: %.elf
	elf2flt -v $(FLTFLAGS) -o $@ -r $<
