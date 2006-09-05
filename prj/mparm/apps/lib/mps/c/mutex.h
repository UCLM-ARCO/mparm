#ifndef MPS_MUTEX_H
#define MPS_MUTEX_H

#include "mpsim_config.h"

typedef struct MPSShMemMutex_ MPSShMemMutex;
struct MPSShMemMutex_ {
    volatile int* sem;
};

static inline
void
mps_shmem_mutex_alloc(MPSShMemMutex* self, int id)
{
    self->sem = (volatile int*)SEMAPHORE_BASE + id;
}

static inline
int
mps_shmem_mutex_trylock(MPSShMemMutex* self)
{
    return !*self->sem;
}

static inline
void
mps_shmem_mutex_lock(MPSShMemMutex* self)
{
    while(!mps_shmem_mutex_trylock(self))
	mps_shmem_mutex_yield(self);
}

static inline
void
mps_shmem_mutex_unlock(MPSShMemMutex* self)
{
    *self->sem = 0;
}

static inline
void
mps_shmem_mutex_yield(MPSShMemMutex* self)
{
}

#endif
