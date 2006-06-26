///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         globals.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Global configuration variables
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __GLOBALS_H__
#define __GLOBALS_H__
#include <string>

#ifndef WIN32
#define FALSE  0
#define TRUE   !FALSE
#endif

/* int is 32 bit on all platforms */
#ifndef WIN32
#include <inttypes.h>
#else
typedef signed char               int8_t;
typedef unsigned char             uint8_t;
typedef short                     int16_t;
typedef int                       int32_t;
typedef unsigned short            uint16_t;
typedef unsigned int              uint32_t;
typedef INT64                     int64_t;
typedef UINT64                    uint64_t;
#endif
typedef int                       bool_t;
typedef int                       BOOL;

#ifdef LXBUILD
typedef long long                 SInt64;
typedef unsigned long long        UInt64;
typedef int                       SInt32;
typedef long unsigned int         UInt32;
typedef short                     SInt16;
typedef unsigned short            UInt16;
typedef signed char               SInt8;
typedef unsigned char             UInt8;
#else
typedef uint64_t                  UInt64;
typedef int64_t                   SInt64;
typedef uint32_t                  UInt32;
typedef int32_t                   SInt32;
typedef uint16_t                  UInt16;
typedef int16_t                   SInt16;
typedef uint8_t                   UInt8;
typedef int8_t                    SInt8;
#endif

typedef UInt64                    dword_t;
typedef SInt64                    sdword_t;
typedef UInt32                    word_t;
typedef SInt32                    sword_t;
typedef UInt16                    halfword_t;
typedef SInt16                    shalfword_t;
typedef UInt8                     byte_t;
typedef SInt8                     sbyte_t;

typedef UInt32                    arm_inst_t;
typedef UInt32                    arm_addr_t;

typedef arm_addr_t                target_addr_t;

typedef UInt32                    addr_t;
typedef UInt32                    ppc_inst_t;



// ISS
typedef enum {SWARM, POWERPC, SIMITARM, LX, TG} ISS;
// Interconnect
typedef enum {STBUS, AMBAAHB, AMBASIG, AMBAAHBSYN, AMBAAXISYN, AMBAOCCN, XPIPES} INTERC;
// Cache instantiation
typedef enum {MISSING, DIRECT, SETASSOC, FASSOC} CACHETYPE;

typedef enum SNOOP_POLICY_TYPE {SNOOP_INVALIDATE, SNOOP_UPDATE};
typedef enum CACHE_WRITE_POLICY_TYPE {WT, WB};

typedef struct MASTER_CONF
{
  unsigned short int binding;
};

typedef struct SLAVE_CONF
{
  unsigned short int binding;
  bool range_set;
  uint32_t start_address;
  uint32_t end_address;
};

typedef struct BRIDGE_CONF
{
  unsigned short int master_binding;
  unsigned short int slave_binding;
  unsigned short int n_ranges;
  uint32_t *start_address;
  uint32_t *end_address;
};

extern bool HAVE_AMBA_AHB;
extern bool HAVE_AMBA_SIG;
extern bool HAVE_AMBA_AHB_SYN;
extern bool HAVE_AMBA_AXI_SYN;
extern bool HAVE_AMBA_OCCN;
extern bool HAVE_STBUS;
extern bool HAVE_XPIPES;
extern bool HAVE_SWARM;
extern bool HAVE_SIMITARM;
extern bool HAVE_POWERPC;
extern bool HAVE_LX;
extern bool HAVE_TG;
extern ISS CURRENT_ISS;
extern INTERC CURRENT_INTERC;
extern bool SHARED_CACHE;
extern bool ACCTRACE;
extern bool STATS;
extern bool POWERSTATS;
extern bool VCD;
extern bool DMA;
extern bool SCRATCH;
extern bool ISCRATCH;
extern unsigned long int ISCRATCHSIZE;
extern bool SMARTMEM;
extern bool AUTOSTARTMEASURING;
extern bool SPCHECK;
extern bool SHOWPROMPT;
extern bool FREQSCALING;
extern bool FREQSCALINGDEVICE;
extern bool CORESLAVE;
extern long int NSIMCYCLES;
extern unsigned short int N_CORES;
extern unsigned short int N_MASTERS;
extern unsigned short int N_PRIVATE;
extern unsigned short int N_LX_PRIVATE;
extern unsigned short int N_SHARED;
extern unsigned short int N_SEMAPHORE;
extern unsigned short int N_INTERRUPT;
extern unsigned short int N_STORAGE;
extern unsigned short int N_SMARTMEM;
extern unsigned short int N_CORESLAVE;
extern unsigned short int N_FREQ;
extern unsigned short int N_FFT;
extern unsigned short int N_SLAVES;
extern unsigned short int N_BUSES;
extern unsigned short int N_BRIDGES;
extern unsigned short int N_IP_TG;
extern unsigned short int N_FREQ_DEVICE;
extern MASTER_CONF *MASTER_CONFIG;
extern SLAVE_CONF *SLAVE_CONFIG;
extern BRIDGE_CONF *BRIDGE_CONFIG;
extern unsigned long int DCACHESIZE;
extern unsigned long int ICACHESIZE;
extern unsigned long int UCACHESIZE;
extern CACHETYPE DCACHETYPE;
extern CACHETYPE ICACHETYPE;
extern CACHETYPE UCACHETYPE;
extern unsigned short int DCACHEWAYS;
extern unsigned short int ICACHEWAYS;
extern unsigned short int UCACHEWAYS;
extern unsigned long int SCRATCH_SIZE;
extern unsigned long int QUEUE_SIZE;
extern unsigned short int INT_WS;
extern unsigned short int MEM_IN_WS;
extern unsigned short int MEM_BB_WS;
extern unsigned short int DCACHE_WS; 
extern unsigned short int ICACHE_WS;
extern unsigned short int UCACHE_WS;
extern unsigned short int SMARTMEM_DMA_SIZE; // FIXME no default value provided, needs initialization in parser
extern bool DRAM;
extern unsigned int DRAMDMA_SIZE;            // FIXME no default value provided, needs initialization in parser
extern unsigned short int SCRATCH_WS;
extern unsigned short int NUMBER_OF_EXT;
extern bool TG_TRACE_COLLECTION;
extern bool USING_OCP;
extern std::string STATSFILENAME;
extern std::string CFGFILENAME;
extern unsigned short int *M_DIVIDER;
extern unsigned short int *I_DIVIDER;
extern unsigned short int *PLL_DELAY;
extern int SNOOPING;
extern SNOOP_POLICY_TYPE SNOOP_POLICY;
extern CACHE_WRITE_POLICY_TYPE CACHE_WRITE_POLICY;
extern unsigned short int DCACHE_DIRTY_BITS;

#include "config.h"

#endif // __GLOBALS_H__
