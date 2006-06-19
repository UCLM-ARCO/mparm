#ifndef MPSIM_H
#define MPSIM_H

#include "simctrl.h"

/* FIXME: These should be extracted from simulator binary */

#define PRIVATE_BASE       0x00000000
#define PRIVATE_SPACING    0x01000000   /* 16 MB */
#define PRIVATE_SIZE       0x00c00000   /* 12 MB */
#define ISCRATCH_BASE      0x00c00000   /* 12 MB */
#define SHARED_BASE        0x19000000
#define SHARED_SPACING     0x00100000   /* 1 MB */
#define SHARED_SIZE        0x00100000   /* 1 MB */
#define SEMAPHORE_BASE     0x20000000
#define SEMAPHORE_SPACING  0x00004000   /* 16 kB */
#define SEMAPHORE_SIZE     0x00004000   /* 16 kB */
#define INTERRUPT_BASE     0x21000000
#define INTERRUPT_SPACING  0x00000150   /* 336 B */
#define INTERRUPT_SIZE     0x00000150   /* 336 B */
#define CORESLAVE_BASE     0x22000000
#define CORESLAVE_SPACING  0x00020000   /* 128 kB */
#define CORESLAVE_SIZE     0x00015000   /*  84 kB */
#define FFT_BASE           0x23000000
#define FFT_SPACING        0x00000010   /*  16 B */
#define FFT_SIZE           0x00000010   /*  16 B */
#define STORAGE_BASE       0x24000000
#define STORAGE_SPACING    0x02000000   /* 32 MB */
#define STORAGE_SIZE       0x02000000   /* 32 MB */
#define FREQ_BASE          0x50000000
#define FREQ_SPACING       0x00000200   /* 512 B */
#define FREQ_SIZE          0x00000200   /* 512 B */
#define INTERNAL_DMA_BASE  0x60000000
#define INTERNAL_MAX_OBJ   9  /* Maximum number of objects managed by dma */
#define INTERNAL_DIM_BURST 4  /* Length of a burst transfer made the dma */
#define INTERNAL_DMA_SIZE  (7*4*INTERNAL_MAX_OBJ+4)
#define SMARTMEM_MAX_CONF  1
#define SMARTMEM_BASE      0x70000000
#define SMARTMEM_SPACING   0x02000000   /* 32 MB  */
#define SMARTMEM_MAX_OBJ   4   /* Maximum number of objects managed by dma */
#define SMARTMEM_DIM_BURST 8   /* Length of a burst transfer made by dma */
#define DRAMDMA_BASE       0x75000000
#define DRAMDMA_SPACING    0x00100000   /* 1 MB  */
#define DRAMDMA_MAX_OBJ    4   /* Maximum number of objects managed by dma */
#define DRAM_BASE          0x76000000
#define DRAM_SIZE          0x00800000 /* 8MB (8MB, 16MB, 32MB, 64MB allowed) */
#define LMI_BASE           0x77000000
#define LMI_SPACING        0x00800000   /* 8 MB  */
/*#define DRAM_BASE         0x77000000   // same as lmi base */
#define DRAM2_SIZE         0x00800000   /*8M It can be 8MB 16MB 32MB 64MB  */
#define SIMSUPPORT_BASE    0x7f000000
#define SIMSUPPORT_SIZE    0x00100000   /* 1 MB */
#define PRIVATE_LX_BASE    0x08000000                               
#define PRIVATE_LX_SPACING 0x01000000   /* 16 MB */
#define PRIVATE_LX_SIZE    0x01000000   /* 16 MB */
#define INTERNAL_BASE      0x90000000

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
