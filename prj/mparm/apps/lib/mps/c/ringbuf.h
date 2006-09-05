#ifndef MPS_RINGBUF_H
#define MPS_RINGBUF_H

#include <c/config.h>
#include <c/semaphore.h>

typedef struct MPSRingBufProducer_ MPSRingBufProducer;
struct MPSRingBufProducer_ {
    volatile void* data;
    volatile int* tail;
    MPSRingBufMutex* mutex;
    MPSShMemSemaphore* semP;
    MPSShMemSemaphore* semC;
};

typedef struct MPSRingBufConsumer_ MPSRingBufConsumer;
struct MPSRingBufConsumer_ {
    volatile void* data;
    volatile int* head;
    MPSRingBufMutex* mutex;
    MPSShMemSemaphore* semP;
    MPSShMemSemaphore* semC;
};

static inline
void
mps_ringbuf_producer_init(MPSRingBufProducer* self)
{
    *self->tail = 0;
}

static inline
void
mps_ringbuf_producer_activate(MPSRingBufProducer* self)
{
    mps_shmem_sem_init(self->semC, N);
    mps_shmem_sem_init(self->semP, 0);
    *self->head = *self->tail = 0;
}

static inline
void
mps_ringbuf_producer_put(MPSRingBufProducer* self, void* data)
{
    struct t__ {
	char buf[MPS_RINGBUF_DATA_SIZE];
    };

    int tail;

    mps_shmem_sem_wait(self->semC);
    mps_ringbuf_mutex_lock(self->tailMutex);

    tail = *self->tail;
    *self->tail = (tail + 1) % MPS_RINGBUF_N;
    *(struct t__*)(self->data)[tail] = *(struct t__*)data;

    mps_shmem_sem_post(self->semP);
    mps_ringbuf_mutex_unlock(self->tailMutex);
}


static inline
void
mps_ringbuf_consumer_init(MPSRingBufConsumer* self)
{
    *self->head = 0;
}

static inline
void
mps_ringbuf_consumer_activate(MPSRingBufProducer* self)
{
    mps_shmem_sem_init(self->semC, N);
    mps_shmem_sem_init(self->semP, 0);
    *self->head = *self->tail = 0;
}

static inline
void
mps_ringbuf_consumer_get(MPSRingBufConsumer* self, void* data)
{
    struct t__ {
	char buf[MPS_RINGBUF_DATA_SIZE];
    };

    mps_shmem_sem_wait(self->semP);
    mps_ringbuf_mutex_lock(self->headMutex);

    head = *self->head;
    *self->head = (head + 1) % MPS_RINGBUF_N;
    *(struct t__*)data = *(struct t__*)(self->data)[head];

    mps_shmem_sem_post(self->semC);
    mps_ringbuf_mutex_unlock(self->headMutex);
}

#endif
