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
#define BUILTIN_DEFAULT_CACHE_WRITE_POLICY WT
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

#endif
