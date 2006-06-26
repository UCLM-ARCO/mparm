///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         config.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Basic platform configuration
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __CONFIG_H__
#define __CONFIG_H__

#ifdef DO_INTERNAL_SYMBOL_TABLE

# define   _V(x)   #x
# define   _M(x)   { #x, _V(x) },
# define   _E(x,y) { #x, #y },
# define _U32(x,y)   _E(x,y)
# define _I32(x,y)   _E(x,y)
# define  _EN(t,x,y) _E(x,y)
# define   _B(x,y)   _E(x,y)
# define _STR(x,y)   _E(x,y)

#else // DO_INTERNAL_SYMBOL_TABLE

#include "core/globals.h"
#include <stdint.h>

# define   _M(x)
# define _U32(x,y) const uint32_t x = y;
# define _I32(x,y) const int32_t x = y;
# define  _EN(t,x,y) const t x = y;
# define   _B(x,y) const bool x = y;
# define _STR(x,y) const char x[] = y;
  
#endif // DO_INTERNAL_SYMBOL_TABLE

// Command line parameter defaults
 _EN(ISS,    BUILTIN_DEFAULT_CURRENT_ISS,    SWARM)
 _EN(INTERC, BUILTIN_DEFAULT_CURRENT_INTERC, AMBAAHB)
_U32(BUILTIN_DEFAULT_N_CORES,            4)
  _B(BUILTIN_DEFAULT_SHARED_CACHE,       false)
_I32(BUILTIN_DEFAULT_NSIMCYCLES,         -1)
  _B(BUILTIN_DEFAULT_ACCTRACE,           false)
  _B(BUILTIN_DEFAULT_VCD,                false)
  _B(BUILTIN_DEFAULT_AUTOSTARTMEASURING, false)
  _B(BUILTIN_DEFAULT_SPCHECK,            false)
  _B(BUILTIN_DEFAULT_SHOWPROMPT,         false)
  _B(BUILTIN_DEFAULT_FREQSCALING,        false)
  _B(BUILTIN_DEFAULT_FREQSCALINGDEVICE,  false)
  _B(BUILTIN_DEFAULT_CORESLAVE,          false)
_U32(BUILTIN_DEFAULT_INT_WS,             1)
_U32(BUILTIN_DEFAULT_MEM_IN_WS,          1)
_U32(BUILTIN_DEFAULT_MEM_BB_WS,          1)
  _B(BUILTIN_DEFAULT_STATS,              true)
  _B(BUILTIN_DEFAULT_POWERSTATS,         false)
  _B(BUILTIN_DEFAULT_DMA,                false)
  _B(BUILTIN_DEFAULT_SMARTMEM,           false)
  _B(BUILTIN_DEFAULT_SCRATCH,            false)
_U32(BUILTIN_DEFAULT_SCRATCH_WS,         0)
  _B(BUILTIN_DEFAULT_ISCRATCH,           false)
_U32(BUILTIN_DEFAULT_ISCRATCHSIZE,       (1024 * 4))
_U32(BUILTIN_DEFAULT_N_PRIVATE,          BUILTIN_DEFAULT_N_CORES)
_U32(BUILTIN_DEFAULT_N_LX_PRIVATE,       0)
_U32(BUILTIN_DEFAULT_N_SHARED,           1)
_U32(BUILTIN_DEFAULT_N_SEMAPHORE,        1)
_U32(BUILTIN_DEFAULT_N_INTERRUPT,        1)
_U32(BUILTIN_DEFAULT_N_STORAGE,          0)
_U32(BUILTIN_DEFAULT_N_SMARTMEM,         0)
_U32(BUILTIN_DEFAULT_N_FFT,              0)
_U32(BUILTIN_DEFAULT_N_BUSES,            1)
_U32(BUILTIN_DEFAULT_N_BRIDGES,          0)
_U32(BUILTIN_DEFAULT_N_IP_TG,            0)
  _B(BUILTIN_DEFAULT_TG_TRACE_COLLECTION, false)
// Unified cache: 8 kB 4-way set associative cache
_U32(BUILTIN_DEFAULT_UCACHESIZE,         (1024 * 8))
 _EN(CACHETYPE, BUILTIN_DEFAULT_UCACHETYPE, SETASSOC)
_U32(BUILTIN_DEFAULT_UCACHEWAYS,         4)
_U32(BUILTIN_DEFAULT_UCACHE_WS,          0)
// Split cache: 4 kB 4-way set associative D-cache and 8 kB direct mapped I-cache
_U32(BUILTIN_DEFAULT_DCACHESIZE,         (1024 * 4))
_U32(BUILTIN_DEFAULT_ICACHESIZE,         (1024 * 8))
 _EN(CACHETYPE, BUILTIN_DEFAULT_DCACHETYPE, SETASSOC)
 _EN(CACHETYPE, BUILTIN_DEFAULT_ICACHETYPE, DIRECT)
_U32(BUILTIN_DEFAULT_DCACHEWAYS,         4)
_U32(BUILTIN_DEFAULT_ICACHEWAYS,         0)
_U32(BUILTIN_DEFAULT_DCACHE_WS,          0)
_U32(BUILTIN_DEFAULT_ICACHE_WS,          0)
_U32(BUILTIN_DEFAULT_SCRATCH_SIZE,       (1024 * 1024 * 2))
_U32(BUILTIN_DEFAULT_QUEUE_SIZE,         (1024 * 16))
  _B(BUILTIN_DEFAULT_USING_OCP,          false)
_STR(BUILTIN_DEFAULT_STATSFILENAME,      "stats.txt")
_STR(BUILTIN_DEFAULT_CFGFILENAME,        "")
_U32(BUILTIN_DEFAULT_SNOOPING,           0)
 _EN(SNOOP_POLICY_TYPE, BUILTIN_DEFAULT_SNOOP_POLICY, SNOOP_INVALIDATE)
  _B(BUILTIN_DEFAULT_DRAM,               false)

// martin letis 02-09-2004
// define WB_CACHE to have modifications for Write Back Cache
#ifdef WB_CACHE
_U32(BUILTIN_DEFAULT_CACHE_WRITE_POLICY, WT)
#endif

_U32(CLOCKPERIOD,      5) // SystemC clock signal period, in time units
_U32(CACHE_LINE,       4) // default ARM cache line length (words)
_U32(FREQSCALING_BITS, 8) // bits coding the frequency dividers in the
			  // system (8 bits -> 256 steps)

//Enables traffic generators on the st lx platform
//#define N_TGEN //FIXME make it selectable at runtime

#ifdef WITH_POWER_NODE
//Enables LEAKAGE computation on STBUS target 
//#define EN_LEAKAGE
#endif

// Platform memory mappings
// Some sanity checks will be performed at runtime by the Addresser module
// Private memories
_U32(PRIVATE_BASE,    0x00000000)
_U32(PRIVATE_SPACING, 0x01000000)   // 16 MB
_U32(PRIVATE_SIZE,    0x00c00000)   // 12 MB
// Instruction cache memories
// Please notice: they must reside in "low" memory because they must be
// accessible by jumps with compact offset
_U32(ISCRATCH_BASE,   0x00c00000)   // 12 MB
// Shared memories
_U32(SHARED_BASE,     0x19000000)
_U32(SHARED_SPACING,  0x00100000)   // 1 MB
_U32(SHARED_SIZE,     0x00100000)   // 1 MB
// Semaphore memories
_U32(SEMAPHORE_BASE,    0x20000000)
_U32(SEMAPHORE_SPACING, 0x00004000)   // 16 kB
_U32(SEMAPHORE_SIZE,    0x00004000)   // 16 kB
// Interrupt slaves
_U32(INTERRUPT_BASE,    0x21000000)
_U32(INTERRUPT_SPACING, 0x00000150)   // 336 B
_U32(INTERRUPT_SIZE,    0x00000150)   // 336 B
// Core-associated slaves and scratchpad memories
_U32(CORESLAVE_BASE,    0x22000000)
_U32(CORESLAVE_SPACING, 0x00100000)   // 1 MB
_U32(CORESLAVE_SIZE,    0x00100000)   // 1 MB
// FFT devices
_U32(FFT_BASE,    0x23000000)
_U32(FFT_SPACING, 0x00000010)   //  16 B
_U32(FFT_SIZE,    0x00000010)   //  16 B
// Storage slaves
_U32(STORAGE_BASE,    0x24000000)
_U32(STORAGE_SPACING, 0x02000000)   // 32 MB
_U32(STORAGE_SIZE,    0x02000000)   // 32 MB
// Frequency scaling devices
_U32(FREQ_BASE,    0x50000000)
_U32(FREQ_SPACING, 0x00000200)   // 512 B
_U32(FREQ_SIZE,    0x00000200)   // 512 B
// DMA devices
_U32(INTERNAL_DMA_BASE,  0x60000000)
_U32(INTERNAL_MAX_OBJ,   9)  // Maximum number of objects managed by the dma
_U32(INTERNAL_DIM_BURST, 4)  // Length of a burst transfer made by the dma
_U32(INTERNAL_DMA_SIZE,  (7*4*INTERNAL_MAX_OBJ+4))
// "Smart" (DMA-paired) memories
_U32(SMARTMEM_MAX_CONF,  1)
_U32(SMARTMEM_BASE,      0x70000000)
_U32(SMARTMEM_SPACING,   0x02000000)   // 32 MB
_U32(SMARTMEM_MAX_OBJ,   4)   // Maximum number of objects managed by the dma
_U32(SMARTMEM_DIM_BURST, 8)   // Length of a burst transfer made by the dma
// DRAM-associated DMA
_U32(DRAMDMA_BASE,    0x75000000)
_U32(DRAMDMA_SPACING, 0x00100000)   // 1 MB
_U32(DRAMDMA_MAX_OBJ, 4)   // Maximum number of objects managed by the dma
_U32(DRAM_BASE,       0x76000000)
_U32(DRAM_SIZE,       0x00100000)   // 8MB (8MB, 16MB, 32MB, 64MB allowed)
// Simulation support
_U32(SIMSUPPORT_BASE, 0x7f000000)
_U32(SIMSUPPORT_SIZE, 0x00100000)   // 1 MB
// LX private memories
_U32(PRIVATE_LX_BASE,    0x08000000)
_U32(PRIVATE_LX_SPACING, 0x01000000)   // 16 MB
_U32(PRIVATE_LX_SIZE,    0x01000000)   // 16 MB
// Internal devices
_U32(INTERNAL_BASE,      0x90000000)

// Config parameters overrided from make
_M(MAKEFILE_DEFAULT_CURRENT_ISS)
_M(MAKEFILE_DEFAULT_CURRENT_INTERC)
_M(MAKEFILE_DEFAULT_N_CORES)
_M(MAKEFILE_DEFAULT_SHARED_CACHE)
_M(MAKEFILE_DEFAULT_NSIMCYCLES)
_M(MAKEFILE_DEFAULT_ACCTRACE)
_M(MAKEFILE_DEFAULT_VCD)
_M(MAKEFILE_DEFAULT_AUTOSTARTMEASURING)
_M(MAKEFILE_DEFAULT_SPCHECK)
_M(MAKEFILE_DEFAULT_SHOWPROMPT)
_M(MAKEFILE_DEFAULT_FREQSCALING)
_M(MAKEFILE_DEFAULT_FREQSCALINGDEVICE)
_M(MAKEFILE_DEFAULT_CORESLAVE)
_M(MAKEFILE_DEFAULT_INT_WS)
_M(MAKEFILE_DEFAULT_MEM_IN_WS)
_M(MAKEFILE_DEFAULT_MEM_BB_WS)
_M(MAKEFILE_DEFAULT_STATS)
_M(MAKEFILE_DEFAULT_POWERSTATS)
_M(MAKEFILE_DEFAULT_DMA)
_M(MAKEFILE_DEFAULT_SMARTMEM)
_M(MAKEFILE_DEFAULT_SCRATCH)
_M(MAKEFILE_DEFAULT_ISCRATCH)
_M(MAKEFILE_DEFAULT_ISCRATCHSIZE)
_M(MAKEFILE_DEFAULT_N_PRIVATE)
_M(MAKEFILE_DEFAULT_N_LX_PRIVATE)
_M(MAKEFILE_DEFAULT_N_SHARED)
_M(MAKEFILE_DEFAULT_N_SEMAPHORE)
_M(MAKEFILE_DEFAULT_N_INTERRUPT)
_M(MAKEFILE_DEFAULT_N_STORAGE)
_M(MAKEFILE_DEFAULT_N_SMARTMEM)
_M(MAKEFILE_DEFAULT_N_FFT)
_M(MAKEFILE_DEFAULT_N_BUSES)
_M(MAKEFILE_DEFAULT_N_BRIDGES)
_M(MAKEFILE_DEFAULT_N_IP_TG)
_M(MAKEFILE_DEFAULT_N_FREQ_DEVICE)
_M(MAKEFILE_DEFAULT_DCACHESIZE)
_M(MAKEFILE_DEFAULT_ICACHESIZE)
_M(MAKEFILE_DEFAULT_UCACHESIZE)
_M(MAKEFILE_DEFAULT_DCACHETYPE)
_M(MAKEFILE_DEFAULT_ICACHETYPE)
_M(MAKEFILE_DEFAULT_UCACHETYPE)
_M(MAKEFILE_DEFAULT_DCACHEWAYS)
_M(MAKEFILE_DEFAULT_ICACHEWAYS)
_M(MAKEFILE_DEFAULT_UCACHEWAYS)
_M(MAKEFILE_DEFAULT_DCACHE_WS)
_M(MAKEFILE_DEFAULT_ICACHE_WS)
_M(MAKEFILE_DEFAULT_UCACHE_WS)
_M(MAKEFILE_DEFAULT_SCRATCH_WS)
_M(MAKEFILE_DEFAULT_SCRATCHSIZE)
_M(MAKEFILE_DEFAULT_QUEUESIZE)
_M(MAKEFILE_DEFAULT_TG_TRACE_COLLECTION)
_M(MAKEFILE_DEFAULT_USING_OCP)
_M(MAKEFILE_DEFAULT_STATSFILENAME)
_M(MAKEFILE_DEFAULT_CFGFILENAME)
_M(MAKEFILE_DEFAULT_SNOOPING)
_M(MAKEFILE_DEFAULT_SNOOP_POLICY)
_M(MAKEFILE_DEFAULT_CACHE_WRITE_POLICY)
_M(MAKEFILE_DEFAULT_DRAM)

#endif // __CONFIG_H__
