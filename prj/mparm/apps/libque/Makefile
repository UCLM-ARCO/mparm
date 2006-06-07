AR=arm-uclinux-ar
CC=arm-uclinux-gcc
CXX=arm-uclinux-g++
CFLAGS=-static -msoft-float -O2
LDFLAGS=$(CFLAGS) -Wl,-elf2flt

SRCS =	scratch_semaphore.h config.h \
	scratch_queue.h scratch_queue.c \
	appsupport.h appsupport.c

EXE = producer consumer

CSRC = $(filter %.c,$(SRCS))
OBJ  =$(CSRC:.c=.o)


all: TargetMem_1.mem

libque.a: $(OBJ)
	$(AR) rcs $@ $(filter %.o,$^)

producer: LDLIBS=-L. -lque
producer: producer.o libque.a

consumer: LDLIBS=-L. -lque
consumer: consumer.o libque.a


TargetMem_1.mem: $(EXE)
	cp $(EXE) ../../runtime/romfs/bin/
	$(MAKE) -C ../../runtime $@
	ln -sf ../../runtime/$@ .

clean:
	$(RM) *~ *.o *.a *.gdb *.mem $(EXE)

