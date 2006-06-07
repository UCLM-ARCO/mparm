///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         globals.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Global configuration variables
//
///////////////////////////////////////////////////////////////////////////////

#include "globals.h"

// Global configuration variables. They will be set by the parser module
bool HAVE_AMBA_AHB;              // Flags to see what interconnects are available for instantiation
bool HAVE_AMBA_SIG;
bool HAVE_AMBA_AHB_SYN;
bool HAVE_AMBA_AXI_SYN;
bool HAVE_AMBA_OCCN;
bool HAVE_STBUS;
bool HAVE_XPIPES;
bool HAVE_SWARM;                 // Flags to see what ISSs are available for instantiation
bool HAVE_SIMITARM;
bool HAVE_POWERPC;
bool HAVE_LX;
bool HAVE_TG;
ISS CURRENT_ISS;                 // ISS to be instantiated
INTERC CURRENT_INTERC;           // Interconnection to be instantiated
bool SHARED_CACHE;               // unified vs split cache
bool ACCTRACE;                   // traces of memory accesses
bool STATS;                      // advanced statistics
bool POWERSTATS;                 // power statistics
bool VCD;                        // VCD waveform output
bool SMARTMEM;                   // enable SMART memories
bool DMA;                        // DMA controllers enabled on the platform (requires SCRATCH too)
bool SCRATCH;                    // scratchpad memories enabled on the platform
bool ISCRATCH;                   // instruction scratchpad memories enabled on the platform
unsigned long int ISCRATCHSIZE;  // size of instruction scratchpad memories
bool AUTOSTARTMEASURING;         // starts collecting statistics immediately at boot
bool SPCHECK;                    // extrapolates performance of a partitioned SPM
bool SHOWPROMPT;                 // shows prompt to connect to /dev/pts devices
bool FREQSCALING;                // support for frequency scaling
bool FREQSCALINGDEVICE;          // device to set frequency dividers at runtime
bool CORESLAVE;                  // support for core-associated slaves
long int NSIMCYCLES;             // forces simulation to stop after x clock cycles
unsigned short int N_CORES;      // processors in the system
unsigned short int N_MASTERS;    // masters in the system (= 2 * N_CORES if DMA present)
unsigned short int N_PRIVATE;    // private ARM memories in the system
unsigned short int N_LX_PRIVATE; // private LX memories in the system
unsigned short int N_SHARED;     // shared memories in the system
unsigned short int N_SEMAPHORE;  // semaphore memories in the system
unsigned short int N_INTERRUPT;  // interrupt devices in the system
unsigned short int N_STORAGE;    // storage devices in the system
unsigned short int N_SMARTMEM;   // number of smart memories in the SoC
unsigned short int N_CORESLAVE;  // core-associated slaves in the system
unsigned short int N_FREQ;       // frequency scaling slaves
unsigned short int N_FFT;        // FFT computation devices
unsigned short int N_SLAVES;     // slaves in the system
unsigned short int N_BUSES;      // interconnects in the system
unsigned short int N_BRIDGES;    // bridges between interconnects in the system
unsigned short int N_IP_TG;      // STM IPTG traffic generetors 
unsigned short int N_FREQ_DEVICE;// FREQUENCY Scaled MASTER DEVICES 
MASTER_CONF *MASTER_CONFIG;      // binding and config of every system master
SLAVE_CONF *SLAVE_CONFIG;        // binding and config of every system slave
BRIDGE_CONF *BRIDGE_CONFIG;      // binding and config of every system bridge
unsigned long int DCACHESIZE;    // data cache size (only used with split caches)
unsigned long int ICACHESIZE;    // instruction cache size (only used with split caches)
unsigned long int UCACHESIZE;    // unified cache size (only used with shared caches)
CACHETYPE DCACHETYPE;            // data cache type (only used with split caches)
CACHETYPE ICACHETYPE;            // instruction cache type (only used with split caches)
CACHETYPE UCACHETYPE;            // unified cache type (only used with shared caches)
unsigned short int DCACHEWAYS;   // data cache ways (if set associative)  (only used with split caches)
unsigned short int ICACHEWAYS;   // instruction cache ways (if set associative)  (only used with split caches)
unsigned short int UCACHEWAYS;   // unified cache ways (if set associative) (only used with shared caches)
unsigned long int SCRATCH_SIZE;  // size of scratchpad memories
unsigned long int QUEUE_SIZE;    // size of the queues of core-associated slaves
unsigned short int INT_WS;       // wait states of the interrupt slave
unsigned short int MEM_IN_WS;    // initial wait states of memory slaves
unsigned short int MEM_BB_WS;    // back-to-back wait states of memory slaves
unsigned short int DCACHE_WS;    // wait states of the data cache
unsigned short int ICACHE_WS;    // wait states of the instruction cache
unsigned short int UCACHE_WS;    // wait states of the unified cache
unsigned short int SCRATCH_WS;   // wait states of scratchpad local memory
unsigned short int NUMBER_OF_EXT;       // number of external interrupts to the cores 
bool TG_TRACE_COLLECTION;        // collect execution traces to be used as TG inputs
bool USING_OCP;                  // use OCP for handshaking between modules
std::string STATSFILENAME;       // name of the file on which to collect statistics
std::string CFGFILENAME;         // name of the file from which to get platform configuration
unsigned short int *M_DIVIDER;   // clock divider of every platform master (for frequency scaling)
unsigned short int *I_DIVIDER;   // clock divider of every platform interconnect (for frequency scaling)
unsigned short int *PLL_DELAY;   // PLL latency before being able to scale to a new device frequency
int SNOOPING;                    // non-0 if we are using snoop and cache coherency
SNOOP_POLICY_TYPE SNOOP_POLICY;

CACHE_WRITE_POLICY_TYPE CACHE_WRITE_POLICY; // WT, WB_RR, WB_SMART_RR
unsigned short int DCACHE_DIRTY_BITS; // number of bits used by dcache for dirty data (INVALID, DIRTY, EXCLUSIVE, etc.)

unsigned short int SMARTMEM_DMA_SIZE;   // size of the SMARTMEM_DMA address space 
bool DRAM;                              // enables DRAM device
unsigned int DRAMDMA_SIZE;              // size of the DRAMDMA address space
