AR=arm-linux-uclibc-ar
CC=arm-linux-uclibc-gcc
CXX=arm-linux-uclibc-g++

SRCS =	scratch_semaphore.h \
	scratch_queue.h scratch_queue.c

all: libque.a producer consumer

libque.a: scratch_queue.o
producer: producer.o
consumer: consumer.o

clean:
	$(RM) *~

