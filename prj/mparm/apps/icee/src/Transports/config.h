#ifndef CONFIG_H
#define CONFIG_H

/*
 * Copyright 2003 DEIS - Universita' di Bologna
 * 
 * name         config.h
 * author       DEIS - Universita' di Bologna
 *              Davide Bertozzi - dbertozzi@deis.unibo.it
 *              Mirko Loghi - mloghi@deis.unibo.it
 *              Federico Angiolini - fangiolini@deis.unibo.it
 *              Francesco Poletti - fpoletti@deis.unibo.it
 * portions by  Massimo Scardamaglia - mascard@vizzavi.it
 * info         Basic platform configuration
 *
 */

/* Command line parameter defaults */
#define BUILTIN_DEFAULT_CURRENT_ISS          SWARM
#define BUILTIN_DEFAULT_CURRENT_INTERC       AMBAAHB
#define BUILTIN_DEFAULT_N_CORES              4
#define BUILTIN_DEFAULT_SHARED_CACHE         false
#define BUILTIN_DEFAULT_NSIMCYCLES           -1
#define BUILTIN_DEFAULT_ACCTRACE             false
#define BUILTIN_DEFAULT_VCD                  false
#define BUILTIN_DEFAULT_AUTOSTARTMEASURING   false
#define BUILTIN_DEFAULT_SPCHECK              false
#define BUILTIN_DEFAULT_SHOWPROMPT           false
#define BUILTIN_DEFAULT_FREQSCALING          false
#define BUILTIN_DEFAULT_FREQSCALINGDEVICE    false
#define BUILTIN_DEFAULT_CORESLAVE            false
#define BUILTIN_DEFAULT_INT_WS               1
#define BUILTIN_DEFAULT_MEM_IN_WS            1
#define BUILTIN_DEFAULT_MEM_BB_WS            1
#define BUILTIN_DEFAULT_STATS                true
#define BUILTIN_DEFAULT_POWERSTATS           false
#define BUILTIN_DEFAULT_DMA                  false
#define BUILTIN_DEFAULT_SMARTMEM             false
#define BUILTIN_DEFAULT_SCRATCH              false
#define BUILTIN_DEFAULT_SCRATCH_WS           0
#define BUILTIN_DEFAULT_ISCRATCH             false
#define BUILTIN_DEFAULT_ISCRATCHSIZE         (1024 * 4)
#define BUILTIN_DEFAULT_N_PRIVATE            BUILTIN_DEFAULT_N_CORES
#define BUILTIN_DEFAULT_N_LX_PRIVATE         0
#define BUILTIN_DEFAULT_N_SHARED             1
#define BUILTIN_DEFAULT_N_SEMAPHORE          1
#define BUILTIN_DEFAULT_N_INTERRUPT          1
#define BUILTIN_DEFAULT_N_STORAGE            0
#define BUILTIN_DEFAULT_N_SMARTMEM           0
#define BUILTIN_DEFAULT_N_FFT                0
#define BUILTIN_DEFAULT_N_BUSES              1
#define BUILTIN_DEFAULT_N_BRIDGES            0
#define BUILTIN_DEFAULT_N_IP_TG              0
#define BUILTIN_DEFAULT_N_FREQ_DEVICE        4
#define BUILTIN_DEFAULT_TG_TRACE_COLLECTION  false
/* Unified cache: 8 kB 4-way set associative cache */
#define BUILTIN_DEFAULT_UCACHESIZE           (1024 * 8)
#define BUILTIN_DEFAULT_UCACHETYPE           SETASSOC
#define BUILTIN_DEFAULT_UCACHEWAYS           4
#define BUILTIN_DEFAULT_UCACHE_WS            0
/* Split cache: 4 kB 4-way set associative D-cache and 8 kB direct mapped I-cache */
#define BUILTIN_DEFAULT_DCACHESIZE           (1024 * 4)
#define BUILTIN_DEFAULT_ICACHESIZE           (1024 * 8)
#define BUILTIN_DEFAULT_DCACHETYPE           SETASSOC
#define BUILTIN_DEFAULT_ICACHETYPE           DIRECT
#define BUILTIN_DEFAULT_DCACHEWAYS           4
#define BUILTIN_DEFAULT_ICACHEWAYS           0
#define BUILTIN_DEFAULT_DCACHE_WS            0
#define BUILTIN_DEFAULT_ICACHE_WS            0
#define BUILTIN_DEFAULT_SCRATCH_SIZE         (1024 * 4)
#define BUILTIN_DEFAULT_QUEUE_SIZE           (1024 * 16)
#define BUILTIN_DEFAULT_USING_OCP            false
#define BUILTIN_DEFAULT_STATSFILENAME        "stats.txt"
#define BUILTIN_DEFAULT_CFGFILENAME          ""
#define BUILTIN_DEFAULT_SNOOPING             0
#define BUILTIN_DEFAULT_SNOOP_POLICY         SNOOP_INVALIDATE
#define BUILTIN_DEFAULT_DRAM                 false

/* martin letis 02-09-2004 */
/* define WB_CACHE to have modifications for Write Back Cache */
#ifdef WB_CACHE
# define BUILTIN_DEFAULT_CACHE_WRITE_POLICY WT
#endif

#define CLOCKPERIOD 5                    /* SystemC clock signal period, in time units */
#define CACHE_LINE 4                     /* default ARM cache line length (words) */
#define FREQSCALING_BITS 8               /* bits coding the frequency dividers in the system (8 bits -> 256 steps) */

/*Enables traffic generators on the st lx platform */
/*#define N_TGEN //FIXME make it selectable at runtime */

#ifdef WITH_POWER_NODE
/*Enables LEAKAGE computation on STBUS target  */
/*#define EN_LEAKAGE */
#endif

/* Platform memory mappings */
/* Some sanity checks will be performed at runtime by the Addresser module */
/* Private memories */
#define PRIVATE_BASE       0x00000000
#define PRIVATE_SPACING    0x01000000   /* 16 MB */
#define PRIVATE_SIZE       0x00c00000   /* 12 MB */
/* Instruction cache memories */
/* Please notice: they must reside in "low" memory because they must be */
/* accessible by jumps with compact offset */
#define ISCRATCH_BASE      0x00c00000   /* 12 MB */
/* Shared memories */
#define SHARED_BASE        0x19000000
#define SHARED_SPACING     0x00100000   /* 1 MB */
#define SHARED_SIZE        0x00100000   /* 1 MB */
/* Semaphore memories */
#define SEMAPHORE_BASE     0x20000000
#define SEMAPHORE_SPACING  0x00004000   /* 16 kB */
#define SEMAPHORE_SIZE     0x00004000   /* 16 kB */
/* Interrupt slaves */
#define INTERRUPT_BASE     0x21000000
#define INTERRUPT_SPACING  0x00000150   /* 336 B */
#define INTERRUPT_SIZE     0x00000150   /* 336 B */
/* Core-associated slaves and scratchpad memories */
#define CORESLAVE_BASE     0x22000000
#define CORESLAVE_SPACING  0x00020000   /* 128 kB */
#define CORESLAVE_SIZE     0x00015000   /*  84 kB */
/* FFT devices */
#define FFT_BASE           0x23000000
#define FFT_SPACING        0x00000010   /*  16 B */
#define FFT_SIZE           0x00000010   /*  16 B */
/* Storage slaves */
#define STORAGE_BASE       0x24000000
#define STORAGE_SPACING    0x02000000   /* 32 MB */
#define STORAGE_SIZE       0x02000000   /* 32 MB */
/* Frequency scaling devices */
#define FREQ_BASE          0x50000000
#define FREQ_SPACING       0x00000200   /* 512 B */
#define FREQ_SIZE          0x00000200   /* 512 B */
/* DMA devices */
#define INTERNAL_DMA_BASE  0x60000000
#define INTERNAL_MAX_OBJ            9  /* Maximum number of objects managed by the dma */
#define INTERNAL_DIM_BURST          4  /* Length of a burst transfer made by the dma */
#define INTERNAL_DMA_SIZE  (7*4*INTERNAL_MAX_OBJ+4)
/* "Smart" (DMA-paired) memories */
#define SMARTMEM_MAX_CONF           1
#define SMARTMEM_BASE      0x70000000
#define SMARTMEM_SPACING   0x02000000   /* 32 MB  */
#define SMARTMEM_MAX_OBJ            4   /* Maximum number of objects managed by the dma */
#define SMARTMEM_DIM_BURST          8   /* Length of a burst transfer made by the dma */
/* DRAM-associated DMA */
#define DRAMDMA_BASE       0x75000000
#define DRAMDMA_SPACING    0x00100000   /* 1 MB  */
#define DRAMDMA_MAX_OBJ             4   /* Maximum number of objects managed by the dma */
#define DRAM_BASE          0x76000000
#define DRAM_SIZE          0x00800000   /* 8MB (8MB, 16MB, 32MB, 64MB allowed) */
/* LMI SDRAM */
#define LMI_BASE          0x77000000
#define LMI_SPACING       0x00800000   /* 8 MB  */
/*#define DRAM_BASE         0x77000000   // same as lmi base */
#define DRAM2_SIZE        0x00800000   /*8M It can be 8MB 16MB 32MB 64MB  */
/* Simulation support */
#define SIMSUPPORT_BASE    0x7f000000
#define SIMSUPPORT_SIZE    0x00100000   /* 1 MB */
/* LX private memories */
#define PRIVATE_LX_BASE    0x08000000                               
#define PRIVATE_LX_SPACING 0x01000000   /* 16 MB */
#define PRIVATE_LX_SIZE    0x01000000   /* 16 MB */
/* Internal devices */
#define INTERNAL_BASE      0x90000000

/* Start statistics collection */
#define START_METRIC_ADDRESS     0x00000000
/* Stop statistics collection */
#define STOP_METRIC_ADDRESS      0x00000004
/* Mark the end of the boot stage */
#define ENDBOOT_ADDRESS          0x00000008
/* Shutdown this processor */
#define SHUTDOWN_ADDRESS         0x0000000c
/* Dump system statistics */
#define DUMP_ADDRESS             0x00000010
/* Dump system statistics (light version) */
#define DUMP_LIGHT_ADDRESS       0x00000014
/* Clear system statistics */
#define CLEAR_ADDRESS            0x00000018

/* Get the ID of this CPU */
#define GET_CPU_ID_ADDRESS       0x00000020
/* Get the total amopunt of CPUs in this system */
#define GET_CPU_CNT_ADDRESS      0x00000024

/* ?? */
#define SET_REQ_IO_ADDRESS       0x00000030

/* Get the current simulation time (32 LSBs) */
#define GET_TIME_ADDRESS_LO      0x00000040
/* Get the current simulation time (32 MSBs) */
#define GET_TIME_ADDRESS_HI      0x00000044
/* Get the current simulation cycle (32 LSBs) */
#define GET_CYCLE_ADDRESS_LO     0x00000048
/* Get the current simulation cycle (32 MSBs) */
#define GET_CYCLE_ADDRESS_HI     0x0000004c
/* Freeze the current simulation time for retrieval */
#define STOP_TIME_ADDRESS        0x00000050
/* Unfreeze the simulation time counter */
#define RELEASE_TIME_ADDRESS     0x00000054
/* Freeze the current simulation cycle for retrieval */
#define STOP_CYCLE_ADDRESS       0x00000058
/* Unfreeze the simulation cycle counter */
#define RELEASE_CYCLE_ADDRESS    0x0000005c

/* Print a debug message to console: set output string */
#define DEBUG_MSG_STRING_ADDRESS 0x00000060
/* Print a debug message to console: set output value */
#define DEBUG_MSG_VALUE_ADDRESS  0x00000064
/* Print a debug message to console: set output mode (newline, etc.) and print */
#define DEBUG_MSG_MODE_ADDRESS   0x00000068
/* Print a debug message to console: ID of the involved processor */
#define DEBUG_MSG_ID_ADDRESS     0x0000006c

/* Profile print functions */
#define DUMP_TIME_START          0x00000070
#define DUMP_TIME_STOP           0x00000074

/* Location where to find the command line argc */
#define GET_ARGC_ADDRESS         0x00010000
/* Location where to find a pointer to the command line argv */
#define GET_ARGV_ADDRESS         0x00010004
/* Location where to find a pointer to the environment */
#define GET_ENVP_ADDRESS         0x00010008
/* Location where to find the command line argv (64 kB area) */
#define ARGV_ADDRESS             0x00020000
/* Location where to find the environment (64 kB area) */
#define ENVP_ADDRESS             0x00030000

/* Set the path and name of the file to be accessed (512 B area) */
#define FILE_NAME_ADDRESS        0x00040000
/* Set the mode for file access */
#define FILE_MODE_ADDRESS        0x00040200
/* Shift the file window forward */
#define FILE_SHIFT_ADDRESS       0x00040204
/* Open a file on disk */
#define FILE_OPEN_ADDRESS        0x00040208
/* Close a file */
#define FILE_CLOSE_ADDRESS       0x0004020c
/* Amount of useful data in the file window */
#define FILE_WINDOW_DATA_ADDRESS 0x00040210
/* Buffer where to exchange file data (64 kB area) */
#define FILE_ADDRESS             0x00050000
/* Don't put anything before 0x00060000 */


/* Print mode flags
 * ----------------------------------------------
 *    bit     mnemonic         output
 * ----------------------------------------------
 * 0x00000001 PR_CPU_ID   "Processor PROCID - "
 * 0x00000002 PR_STRING   "STRING "
 * 0x00000004 PR_HEX      "0xHEXVAL "
 * 0x00000008 PR_DEC      "DECVAL "
 * 0x00000010 PR_CHAR     "CHARVAL "
 * 0x00000020 PR_TSTAMP   "@ TIMESTAMP "
 * 0x00000040 PR_NEWL     "\n"
 * 
 * e.g.: 0x00000067 PR_CPU_ID | PR_STRING | PR_HEX | PR_TSTAMP | PR_NEWL  "Processor PROCID - STRING 0xHEXVAL @ TIMESTAMP \n"
 * e.g.: 0x00000049 PR_CPU_ID | PR_DEC | PR_NEWL                          "Processor PROCID - DECVAL \n"
 */ 
#define PR_CPU_ID 0x00000001
#define PR_STRING 0x00000002
#define PR_HEX    0x00000004
#define PR_DEC    0x00000008
#define PR_CHAR   0x00000010
#define PR_TSTAMP 0x00000020
#define PR_NEWL   0x00000040

#define FILE_READ    0x00000001
#define FILE_WRITE   0x00000002

/* Size of the file window (in words: 64 kB -> 16 kwords) */
#define FILE_WINDOW_SIZE 0x00004000

#endif
