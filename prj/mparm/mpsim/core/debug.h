///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         debug.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Basic debug routines
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <systemc.h>

#ifdef WITH_TIME_COUNTER
  #define TRACE_CLK_CNT(level) TRACE(level, "time: %10.1f\n", sc_simulation_time())
#else
  #define TRACE_CLK_CNT(level) 
#endif

#ifndef NOCORE
  #define DIE(a) abort(); /* CORE DUMP!!! */
#else
  #define DIE(a) exit(a);
#endif

#define ASSERT(cond) if (!(cond))                                                      \
                     {                                                                 \
                       fprintf(stderr,                                                 \
                       "Assert failed [%s] on file %s at line %d at time %10.1f ns\n", \
                        #cond, __FILE__, __LINE__, sc_simulation_time());              \
                        DIE(0xBADF00D);                                                \
                     }

#ifdef PRINTDEBUG
  #ifndef TRACELEVEL
    #define TRACELEVEL 12
  #endif
#else
  #define TRACELEVEL 0
#endif


#ifdef OUTDEBUG
  void TRACE(int level, char *format, ...);
  void TRACEX(int flags, int level, char *format, ...);
#else
  #define TRACE(level, format...)
  #define TRACEX(flags, level, format...)
#endif

#define TRACEX_FLAGS_NONE           0
#define TRACEX_FLAGS_ALL            0xFFFFFFFF

#define STBUS_OP_TRACEX             0x00000001
#define INTERRUPT_TRACEX            0x00000002
#define SNOOPDEV_TRACEX             0x00000004
#define CACHE_TRACEX                0x00000008
#define TARGET_TRACEX               0x00000010
#define MEM_TRACEX                  0x00000020
#define SCRATCH_TRACEX              0x00000040
#define SEMAPHORE_TRACEX            0x00000080
#define SMARTMEM_TRACEX             0x00000100
#define CORESLAVE_TRACEX            0x00000200
#define IDMA_CONT_TRACEX            0x00000400
#define IDMA_TRAN_TRACEX            0x00000800
#define MASTER_TRACEX               0x00001000
#define XPIPES_NI_TRACEX            0x00002000
#define ISS_ACC_TRACEX              0x00004000
#define SHARED_TRACEX               0x00008000
#define DCFIFO_TRACEX               0x00010000
#define BRIDGE_TRACEX               0x00020000
#define ST_INITIATOR_LX             0x00040000
#define ST_INITIATOR_LX1            0x00080000
#define FREQDEVICE_TRACEX           0x00100000
#define ICM_TRACEX                  0x00200000
#define DRAMDMACONTROL_TRACEX       0x00400000
#define DRAMDMATRANSFER_TRACEX      0x00800000
#define DRAMDMASTATEMACHINE_TRACEX  0x01000000
#define SDRAM_TRACEX                0x02000000
#define DRAMDMATARGET_TRACEX        0x04000000

#endif // __DEBUG_H__

