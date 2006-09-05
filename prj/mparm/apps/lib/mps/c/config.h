#ifndef MPS_CONFIG_H
#define MPS_CONFIG_H

#ifndef MPS_RINGBUF_DATA_SIZE
# define MPS_RINGBUF_DATA_SIZE 16
#endif

#ifndef MPS_RINGBUF_N
# define MPS_RINGBUF_N 16
#endif

#ifndef MPS_RINGBUF_MUTEX
# define MPS_RINGBUF_MUTEX
# define MPSRingBufMutex MPSShMemMutex
# define mps_ringbuf_mutex_alloc(self,id) mps_shmem_mutex_alloc(self,id)
# define mps_ringbuf_mutex_trylock(self)  mps_shmem_mutex_trylock(self)
# define mps_ringbuf_mutex_lock(self)     mps_shmem_mutex_lock(self)
# define mps_ringbuf_mutex_unlock(self)   mps_shmem_mutex_unlock(self)
# define mps_ringbuf_mutex_yield(self)    mps_shmem_mutex_yield(self)
#endif

#endif
