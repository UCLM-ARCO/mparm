#ifndef MPS_MUTEX_H
#define MPS_MUTEX_H

#include <c/mutex.h>

typedef struct MPSShMemSemaphore_ MPSShMemSemaphore;
struct MPSShMemSemaphore_ {
    volatile int* lock;
    volatile int* count;
};

static inline
void
mps_shmem_sem_alloc(MPSShMemSemaphore* self, int lock, int count)
{
    self->lock = (volatile int*)SEMAPHORE_BASE + lock;
    self->count = (volatile int*)SHARED_BASE + count;
}

static inline
void
mps_shmem_sem_init(MPSShMemSemaphore* self, int v)
{
    mps_shmem_mutex_unlock((MPSShMemMutex*)self);
    *self->count = v;
}

static inline
void
mps_shmem_sem_wait(MPSShMemSemaphore* self)
{
    for(;;) {
	mps_shmem_mutex_lock((MPSShMemMutex*)self);
	int value = *self->count;
	if (value) {
	    *self->count = value - 1;
	    mps_shmem_mutex_unlock((MPSShMemMutex*)self);
	    break;
	}
	mps_shmem_mutex_unlock((MPSShMemMutex*)self);
	mps_shmem_mutex_yield((MPSShMemMutex*)self);
    }
}

static inline
void
mps_shmem_sem_post(MPSShMemSemaphore* self)
{
    mps_shmem_mutex_lock((MPSShMemMutex*)self);
    *self->count = *self->count + 1;
    mps_shmem_mutex_unlock((MPSShMemMutex*)self);
}

#endif
