///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         address.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Performs address management and translation
//
///////////////////////////////////////////////////////////////////////////////

#include "address.h"
#include "smartmem_signal.h"

Addresser *addresser;

///////////////////////////////////////////////////////////////////////////////
// Constructor - Initializes addresses basing on default values.
Addresser::Addresser()
{
  uint8_t loop;
  uint16_t i;
  uint32_t base;

  SLV_TABLE = (slv_config *)malloc(N_SLAVES * sizeof(slv_config));
  SCRATCHadd = (slv_config *)malloc(N_CORES * sizeof(slv_config));
  ISCRATCHadd = (slv_config *)malloc(N_CORES * sizeof(slv_config));
  QUEUEadd = (slv_config *)malloc(N_CORES * sizeof(slv_config));
  // marchal create addressing table for smart memories
  // we take the number of slaves since this simplifies 
  // addressing
  SMARTMEM_DMAadd =  (slv_config *)malloc(N_SLAVES * sizeof(slv_config));

  pMemoryDebug = (char **)malloc(N_SLAVES * sizeof(char *));
  scratchMemoryDebug = (char **)malloc(N_CORES * sizeof(char *));
  
  pMem_classDebug = (Mem_class **)malloc(N_SLAVES * sizeof(Mem_class));
  
  // Boundary checks
  if (PRIVATE_SIZE > PRIVATE_SPACING)
  {
    printf("Fatal error: Wrong sizes in config.h #define's for private memories!\n");
    exit(1);
  } 
  if (PRIVATE_LX_SIZE > PRIVATE_LX_SPACING)
  {
    printf("Fatal error: Wrong sizes in config.h #define's for privatelx memories!\n");
    exit(1);
  }
  if (SHARED_SIZE > SHARED_SPACING)
  {
    printf("Fatal error: Wrong sizes in config.h #define's for shared memories!\n");
    exit(1);
  }
  if (SEMAPHORE_SIZE > SEMAPHORE_SPACING)
  {
    printf("Fatal error: Wrong sizes in config.h #define's for semaphore memories!\n");
    exit(1);
  }
  if (INTERRUPT_SIZE > INTERRUPT_SPACING)
  {
    printf("Fatal error: Wrong sizes in config.h #define's for interrupt slaves!\n");
    exit(1);
  }
  if ((PRIVATE_BASE + N_PRIVATE*PRIVATE_SPACING > PRIVATE_LX_BASE) && (N_LX_PRIVATE > 0))
  {
    printf("Fatal error: Collision in address space of private and privatelx (check config.h #define's)!\n");
    exit(1);
  }
  if (PRIVATE_LX_BASE + N_LX_PRIVATE*PRIVATE_LX_SPACING > SHARED_BASE)
  {
    printf("Fatal error: Collision in address space of privatelx and shared memories (check config.h #define's)!\n");
    exit(1);
  } 
  if (SHARED_BASE + N_SHARED*SHARED_SPACING > SEMAPHORE_BASE)
  {
    printf("Fatal error: Collision in address space of shared and semaphore memories (check config.h #define's)!\n");
    exit(1);
  }
  if (SEMAPHORE_BASE + N_SEMAPHORE*SEMAPHORE_SPACING > INTERRUPT_BASE)
  {
    printf("Fatal error: Collision in address space of semaphore memories and interrupt slaves (check config.h #define's)!\n");
    exit(1);
  }
  if (INTERRUPT_BASE + N_INTERRUPT*INTERRUPT_SPACING > CORESLAVE_BASE)
  {
    printf("Fatal error: Collision in address space of interrupt slaves and core-associated slaves (check config.h #define's)!\n");
    exit(1);
  }
  if (CORESLAVE_BASE + N_CORESLAVE*CORESLAVE_SPACING > STORAGE_BASE)
  {
    printf("Fatal error: Collision in address space of core-associated slaves and storage slaves (check config.h #define's)!\n");
    exit(1);
  }
  if (STORAGE_BASE + N_STORAGE*STORAGE_SPACING > FREQ_BASE)
  {
    printf("Fatal error: Collision in address space of storage slaves and frequency scaling slaves (check config.h #define's)!\n");
    exit(1);
  }
  if (FREQ_BASE + N_CORES*FREQ_SPACING > INTERNAL_DMA_BASE)
  {
    printf("Fatal error: Collision in address space of frequency scaling slaves and DMA devices (check config.h #define's)!\n");
    exit(1);
  }
  // FIXME check collisions
  if (INTERNAL_DMA_BASE + INTERNAL_DMA_SIZE > 0x70000000)
  {
    printf("Fatal error: Error in address space of DMA devices (check config.h #define's)!\n");
    exit(1);
  }
  if (CORESLAVE_BASE + N_CORES*CORESLAVE_SIZE > FFT_BASE)
  {
    printf("Fatal error: Collision in address space of scratchpad memories and FFT devices (check config.h #define's)!\n");
    exit(1);
  }
  if (FFT_BASE + N_FFT*FFT_SIZE > STORAGE_BASE)
  {
    printf("Fatal error: Collision in address space of scratchpad memories and storage devices (check config.h #define's)!\n");
    exit(1);
  }
  if (CORESLAVE)
  {
    if (SCRATCH_SIZE + QUEUE_SIZE > CORESLAVE_SIZE)
    {
      printf("Fatal error: Collision in address space of scratchpad memories: scratchpad size plus queue\n");
      printf("size exceeds the coreslave size (check config.h #define's and command line parameters)!\n");
      exit(1);
    }
  }
  if (N_SMARTMEM)
  {
    if ((unsigned int)(SMARTMEM_BASE + N_SMARTMEM * SMARTMEM_SPACING) > DRAMDMA_BASE)
      {
        printf("Fatal error: Collision in address space of smart memories and DRAM-associated DMA (check config.h #define's)!\n");
        exit(1);
      }
  }
  // FIXME check are even too tight (if no DRAM, more SMARTMEM are allowed, etc.)
  if (DRAM)
  {
    if (DRAMDMA_BASE + DRAMDMA_SIZE > DRAM_BASE)
    {
      printf("Fatal error: Collision in address space of DRAM-associated DMA and DRAM (check config.h #define's)!\n");
      exit(1);
    }
  
    if (DRAM_BASE + DRAM_SIZE > INTERNAL_BASE)
    {
      printf("Fatal error: Collision in address space of DRAM and internal devices (check config.h #define's)!\n");
      exit(1);
    }
  }
  
  // Setting up address structures
  for (i=0; i < N_PRIVATE; i++)          // Private ARM memories
  {
    SLV_TABLE[i+PrivateStartID()].firstaddr = PRIVATE_BASE + (PRIVATE_SPACING*i);
    SLV_TABLE[i+PrivateStartID()].lastaddr = SLV_TABLE[i+PrivateStartID()].firstaddr + PRIVATE_SIZE - 1;
  }
  for (i=0; i < N_LX_PRIVATE; i++)       // Private LX memories
  {
    SLV_TABLE[i+PrivateLXStartID()].firstaddr = PRIVATE_LX_BASE + (PRIVATE_LX_SPACING*i);
    SLV_TABLE[i+PrivateLXStartID()].lastaddr = SLV_TABLE[i+PrivateLXStartID()].firstaddr + PRIVATE_LX_SIZE - 1;
  }
  for (i=0; i < N_SHARED; i++)           // Shared memories
  {
    if (SLAVE_CONFIG[i+SharedStartID()].range_set == true)
    {
      SLV_TABLE[i+SharedStartID()].firstaddr = SLAVE_CONFIG[i+SharedStartID()].start_address;
      SLV_TABLE[i+SharedStartID()].lastaddr = SLAVE_CONFIG[i+SharedStartID()].end_address;
    }
    else
    {
      SLV_TABLE[i+SharedStartID()].firstaddr = SHARED_BASE + (SHARED_SPACING*i);
      SLV_TABLE[i+SharedStartID()].lastaddr = SLV_TABLE[i+SharedStartID()].firstaddr + SHARED_SIZE - 1;
    }
  }
  for (i=0; i < N_SEMAPHORE; i++)        // Semaphore memories
  {
    if (SLAVE_CONFIG[i+SemaphoreStartID()].range_set == true)
    {
      SLV_TABLE[i+SemaphoreStartID()].firstaddr = SLAVE_CONFIG[i+SemaphoreStartID()].start_address;
      SLV_TABLE[i+SemaphoreStartID()].lastaddr = SLAVE_CONFIG[i+SemaphoreStartID()].end_address;
    }
    else
    {
      SLV_TABLE[i+SemaphoreStartID()].firstaddr = SEMAPHORE_BASE + (SEMAPHORE_SPACING*i);
      SLV_TABLE[i+SemaphoreStartID()].lastaddr = SLV_TABLE[i+SemaphoreStartID()].firstaddr + SEMAPHORE_SIZE - 1;
    }
  }
  for (i=0; i < N_INTERRUPT; i++)        // Interrupt slaves
  {
    SLV_TABLE[i+InterruptStartID()].firstaddr = INTERRUPT_BASE + (INTERRUPT_SPACING*i);
    SLV_TABLE[i+InterruptStartID()].lastaddr = SLV_TABLE[i+InterruptStartID()].firstaddr + INTERRUPT_SIZE - 1;
  }
  for (i=0; i < N_CORESLAVE; i++)        // Core-associated slaves
  {
    SLV_TABLE[i+CoreSlaveStartID()].firstaddr = CORESLAVE_BASE + (CORESLAVE_SPACING*i);
    SLV_TABLE[i+CoreSlaveStartID()].lastaddr = SLV_TABLE[i+CoreSlaveStartID()].firstaddr + CORESLAVE_SIZE - 1;
  }
  for (i=0; i < N_STORAGE; i++)          // Storage slaves
  {
    SLV_TABLE[i+StorageStartID()].firstaddr = STORAGE_BASE + (STORAGE_SPACING*i);
    SLV_TABLE[i+StorageStartID()].lastaddr = SLV_TABLE[i+StorageStartID()].firstaddr + STORAGE_SIZE - 1;
  }  
  for (i=0; i < N_FREQ; i++)             // Frequency scaling slaves
  {
    SLV_TABLE[i+FreqStartID()].firstaddr = FREQ_BASE + (FREQ_SPACING*i);
    SLV_TABLE[i+FreqStartID()].lastaddr = SLV_TABLE[i+FreqStartID()].firstaddr + FREQ_SIZE - 1;
  }
  for (i=0; i < N_SMARTMEM; i++)         // Smart memory slaves
  {
    SLV_TABLE[i+Smartmem_slave_StartID()].firstaddr = SMARTMEM_BASE + (SMARTMEM_SPACING*i);
    SLV_TABLE[i+Smartmem_slave_StartID()].lastaddr = SLV_TABLE[i+Smartmem_slave_StartID()].firstaddr + SMART_MEM_SIZE[i] - 1;
  }
  for (i=0; i < N_FFT; i++)              // FFT device slaves
  {
    SLV_TABLE[i+FFT_slave_StartID()].firstaddr = FFT_BASE + (FFT_SPACING*i);
    SLV_TABLE[i+FFT_slave_StartID()].lastaddr = SLV_TABLE[i+FFT_slave_StartID()].firstaddr + FFT_SIZE - 1;
  }
  
  if (DRAM)
  {
    SLV_TABLE[DRAM_StartID()].firstaddr = DRAMDMA_BASE; 
    SLV_TABLE[DRAM_StartID()].lastaddr = DRAMDMA_BASE + DRAMDMA_SIZE - 1;
  }

  if (DMA)
  {
    DMAadd.firstaddr = INTERNAL_DMA_BASE;
    DMAadd.lastaddr  = DMAadd.firstaddr + INTERNAL_DMA_SIZE - 1;
  }

  //The scratchpad is mapped in the physical address of the CORESLAVE
  if (SCRATCH)
  {
    for(i=0; i < N_CORES; i++)
    { 
      SCRATCHadd[i].firstaddr = CORESLAVE_BASE + (CORESLAVE_SPACING*i);
      SCRATCHadd[i].lastaddr = SCRATCHadd[i].firstaddr + SCRATCH_SIZE - 1;
    }
  }
  
  //The instruction scratchpad. Currently they are all mapped at the same
  //address (in "low" memory, to be accessible by short-offset jumps)
  if (ISCRATCH)
  {
    for(i=0; i < N_CORES; i++)
    { 
      ISCRATCHadd[i].firstaddr = ISCRATCH_BASE;
      ISCRATCHadd[i].lastaddr = ISCRATCHadd[i].firstaddr + ISCRATCHSIZE - 1;
      //TODO implement collision checking
    }
  }
  
  //If core-associated devices are instantiated, then queues must also be initialized just after
  //the end of the scratchpad address space
  if (CORESLAVE)
  {
    for(i=0; i < N_CORESLAVE; i++)
    { 
      QUEUEadd[i].firstaddr = CORESLAVE_BASE + (CORESLAVE_SPACING*i) + SCRATCH_SIZE;
      QUEUEadd[i].lastaddr = QUEUEadd[i].firstaddr + QUEUE_SIZE - 1;
    }
  }
  
  // If working with partitioned scratchpad memories, load their profile and manage them differently
  if (SPCHECK)
  {
    nRanges = (unsigned short int *)malloc(N_CORES * sizeof(unsigned short int));
    start = (unsigned int **)malloc(N_CORES * sizeof(unsigned int *));
    finish = (unsigned int **)malloc(N_CORES * sizeof(unsigned int *));
    decode = (unsigned int **)malloc(N_CORES * sizeof(unsigned int *));
    tocheck = (bool *)malloc(N_CORES * sizeof(bool));
    fcheckscratch = (FILE **)malloc(N_CORES * sizeof(FILE *));

    for (i=0; i < N_CORES; i++)          // Scratchpad memories
    {
      // Tries to open, for every processor, the file containing the description of the
      // optimal partitioning for the associated scratchpad memory. If a file is not found,
      // a simple warning is asserted and execution continues without related checks
      sprintf(checkname, "trace%d.tra.sort.count.clean.output", i);
      fcheckscratch[i]=fopen(checkname, "r");
      tocheck[i] = false;
      if (!fcheckscratch[i])
        fprintf(stderr, "\nWarning: input file trace%d.tra.sort.count.clean.output with SPM partitioning info not found!\n", i);
      else
      {
        tocheck[i] = true;

        // Initializes two arrays (per processor) with the starting and finishing addresses of scartchpad ranges
        fscanf(fcheckscratch[i], "%hu", &nRanges[i]);

        start[i] = (unsigned int *)malloc(nRanges[i] * sizeof(unsigned int));
        finish[i] = (unsigned int *)malloc(nRanges[i] * sizeof(unsigned int));
        decode[i] = (unsigned int *)malloc(nRanges[i] * sizeof(unsigned int));

        for (loop = 0, base = 0; loop < nRanges[i]; loop++)
        {
          fscanf(fcheckscratch[i], "%x %x", start[i] + loop, finish[i] + loop);
          decode[i][loop] = base;
          base += finish[i][loop] + 1 - start[i][loop];
#ifdef PRINTDEBUG
          printf("Addresser: set up an SPM decode table. Range 0x%08x-0x%08x maps to base 0x%08x\n", start[i][loop],
            finish[i][loop], decode[i][loop]);
#endif
        }
      }
    }
  }
}
