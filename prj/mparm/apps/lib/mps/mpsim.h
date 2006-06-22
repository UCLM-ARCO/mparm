#ifndef MPSIM_H
#define MPSIM_H

#include "mpsim_config.h"

/* --------------------------------------------------------------------
   Shared Memory map:
      0x0000: flag to notify shared allocator initialization
      0x0010: pointer to the next memory block to be allocated
      0x0200: flag to notify system initialization
      0x0300: barriers (a barrier requires 8 bytes)
      0x1000: start of the dynamically allocatable shared memory
      0x80000: start of the user memory
 
  Hardware Lock map:
      first -> last - 0x14: user-available (semaphores[])
      last - 0x10 -> last: used by WAIT, SIGNAL, TEST_AND_SET (lock[])
      The lock[] array points to
        ((int*)(SEMAPHORE_BASE + SEMAPHORE_SIZE)) - HWLOCK_SPLIT
      Being HWLOCK_SPLIT == 0xa, lock[] has six negative locations and
      ten positive. Locks with negative IDs (-6 to -1) are reserved for
      the functions of the support library; locks with positive IDs (0 
      to 9) are again user-available

  Hardware Locks used by the support library map:
      -6  :    (reserved for future use)
      -5  :    (reserved for future use)
      -4  :    (reserved for future use)
      -3  :  synchronizes the shared memory allocator
      -2  :  synchronizes accesses to barriers
      -1  :    (reserved for future use)
-------------------------------------------------------------------- */

#define SHM_IN_FLAG_OFFSET    0x00000000
#define SHM_VARNEXT_OFFSET    0x00000010
#define INIT_FLAG_OFFSET      0x00000200
#define BARRIER_OFFSET        0x00000300
#define ALLOCABLE_SHM_OFFSET  0x00001000
#define USER_SHM_OFFSET       0x00080000
#define HWLOCK_SPLIT          0xa
#define BARRIER_SIZE          (2 * sizeof(int))

#define _Cr(x) ((volatile char*)(x))
#define _Ir(x) ((volatile int*)(x))
#define _Ur(x) ((volatile unsigned*)(x))
#define _Pr(x) ((volatile void* volatile *)(x))

#define MPSIM_SEM       _Ir(SEMAPHORE_BASE)
#define MPSIM_LOCK      _Ir(SEMAPHORE_BASE + SEMAPHORE_SIZE - HWLOCK_SPLIT)
#define MPSIM_INT       _Ur(INTERNAL_BASE)
#define MPSIM_FREQ      _Ur(FREQ_BASE)
#define MPSIM_SHARED    _Cr(SHARED_BASE)
#define MPSIM_INIT_FLAG _Ur(SHARED_BASE + INIT_FLAG_OFFSET)
#define MPSIM_BARRIER(id) _Ur(SHARED_BASE + BARRIER_OFFSET + id * BARRIER_SIZE)

static inline int  TEST_AND_SET(int id) { return MPSIM_LOCK[id]; }
static inline void WAIT(int id)         { while (MPSIM_LOCK[id]); }
static inline void SIGNAL(int id)       { MPSIM_LOCK[id] = 0; }

/* normalize_address - Aligns address to the next 0x10 boundary */
#define normalize_address(A)  ( ( ((unsigned)A) + 0xe) & (~0xf) )

#ifdef DEBUG
#  ifndef NOCORE
#    define DIE(a) abort();            // CORE DUMP!!!
#  else
#    define DIE(a) exit(a);
#  endif
#  define ASSERT(cond) if (!(cond)) { \
     pr("Assert failed [" #cond "] on file " __FILE__ \
        " - line " __LINE__ "", 0x0, PR_CPUID, PR_STRING, PR_NEWL); \
     DIE(0xdeadbeef); \
   }
#  define SHOW_DEBUG(x)     pr(x, PR_CPUID, PR_STRING, PR_NEWL)
#  define SHOW_DEBUG_INT(x) pr(x, PR_CPUID, PR_INT, PR_NEWL)
#else
#  define ASSERT(cond)
#  define SHOW_DEBUG(x)
#  define SHOW_DEBUG_INT(x)
#endif

void scale_this_core_frequency(unsigned short divider);
void scale_device_frequency(unsigned short int divider, int id);
unsigned short get_this_core_frequency();
unsigned short int get_device_frequency(int id);
void send_interrupt(int id);
void WAIT_FOR_INITIALIZATION();
void INITIALIZATION_DONE();
void BARINIT(int id);
void BARRIER(int id, int n_proc);

#endif
