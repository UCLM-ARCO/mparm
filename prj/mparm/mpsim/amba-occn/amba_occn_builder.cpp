///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_occn_builder.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Manages AMBA AHB (OCCN) platform instantiation
//
///////////////////////////////////////////////////////////////////////////////

#include "amba_occn_builder.h"

void buildAMBAAHBOCCNPlatform(int new_argc, char *new_argv[], char *new_envp[])
{
  
  int i, j;
  char buffer[20];

  readymast = new sc_signal<bool> [N_MASTERS];
  pinoutmast = new sc_signal<PINOUT> [N_MASTERS];
  requestmast = new sc_signal<bool> [N_MASTERS];
  extint = new sc_signal<bool> [NUMBER_OF_EXT*N_CORES];

  #if 0 
  if (DMA)
  {
    pinoutwrappertodma = new sc_signal<PINOUT> [N_CORES];
    readywrappertodma = new sc_signal<bool> [N_CORES];
    requestwrappertodma = new sc_signal<bool> [N_CORES];
    datadmacontroltotransfer = new sc_signal<DMA_CONT_REG1> [N_CORES];
    requestcontroltotransfer = new sc_signal<bool> [N_CORES];
    finishedtransfer = new sc_signal<bool> [N_CORES];
    spinoutscratchdma = new sc_signal<PINOUT> [N_CORES];
    sreadyscratchdma = new sc_signal<bool> [N_CORES];
    srequestscratchdma = new sc_signal<bool> [N_CORES];
  }
  #endif
  
  // Scratchpad memories
  Mem_class *datascratch[N_CORES];
  for (i=0; i<N_CORES; i++)
  {
    if (SCRATCH && SPCHECK)
      datascratch[i] = new Scratch_part_mem(i, addresser->ReturnScratchSize());
    else
      if (SCRATCH)
      	datascratch[i] = new Scratch_mem("Scratchpad",i, addresser->ReturnScratchSize(),addresser->ReturnScratchPhysicalAddress(i));
    	else
      	 datascratch[i] = NULL;
  }

  Mem_class *queue[N_CORES];
  
   for (i=0; i<N_CORESLAVE; i++)
    {
     if(CORESLAVE)
      queue[i] = new Scratch_queue_mem("Scratch-semaphore",i, addresser->ReturnScratchSize(),addresser->ReturnQueuePhysicalAddress(i));
     else
       queue[i] = NULL;
    }
  
  // ARM processors
  armsystem *soc [N_CORES];
  for (i=0; i<N_CORES; i++)
  {
    sprintf(buffer, "Arm_System%d", i);
    soc[i] = new armsystem(buffer, (unsigned int)i, datascratch[i], queue[i]);
    soc[i]->clock(ClockGen_1);
    soc[i]->pinout_ft_master[0](pinoutmast[i]);
    soc[i]->ready_from_master[0](readymast[i]);
    soc[i]->request_to_master[0](requestmast[i]);
    for (j=0; j<NUMBER_OF_EXT; j++)
      soc[i]->extint[j](extint[i*NUMBER_OF_EXT + j]);
  }
  
  // AHB BUS. Must be instantiated before masters and slaves (which require my_ahb)
  AhbBus<Pdu<AhbMasterCtrl,N_int32>,Pdu<AhbSlaveCtrl,N_int32> > *my_ahb;
  my_ahb = new AhbBus<Pdu<AhbMasterCtrl,N_int32>,Pdu<AhbSlaveCtrl,N_int32> > ("AHBBUS");
  my_ahb->clk(ClockGen_1);
  
  // AMBA bus masters
  amba_occn_master *mst[N_MASTERS];
  for (i=0; i<N_MASTERS; i++)
  {
    sprintf(buffer, "mst%d", i);
    mst[i] = new amba_occn_master(buffer);
    mst[i]->clock(ClockGen_1);
    mst[i]->pinout(pinoutmast[i]);
    mst[i]->ready_to_wrapper(readymast[i]);
    mst[i]->request_from_wrapper(requestmast[i]);
    
    // AHB binding
    mst[i]->port(*my_ahb);
    
    my_ahb->set_master_priority(&(mst[i]->port),2);
    
  }
  //default master
  //AST-OCCN : Default master must be connected as the last master in the architecture 
   AhbDefMaster *masterdef;
  masterdef = new AhbDefMaster("DefMaster");
  masterdef->m_if(*my_ahb);
  my_ahb->set_master_priority(&(masterdef->m_if),1);
 
  #if 0 
  // DMA support
  if (DMA)
  {
    Dma_control_scratch *dmacont[N_CORES];
    Dma_transfer_scratch *transf[N_CORES];

    for (i=0; i<N_CORES; i++)
    {
      //instance a DMA controller
      sprintf(buffer,"dma%d",i);
      dmacont[i] = new
      Dma_control_scratch(buffer,INTERNAL_MAX_OBJ,1,addresser->ReturnDMAPhysicalAddress());
      dmacont[i]->clock(ClockGen_1);

      //instance a DMA transfer module
      sprintf(buffer,"tra%d", i);
      transf[i] = new Dma_transfer_scratch(buffer,INTERNAL_DIM_BURST,INTERNAL_MAX_OBJ,1,datascratch[i]);
      transf[i]->clock(ClockGen_1);

      //connect this wrapper to the DMA controller
      soc[i]->pinout_ft_master[1](pinoutwrappertodma[i]);
      soc[i]->ready_from_master[1](readywrappertodma[i]);
      soc[i]->request_to_master[1](requestwrappertodma[i]);
      dmacont[i]->dmapin(pinoutwrappertodma[i]);
      dmacont[i]->readydma(readywrappertodma[i]);
      dmacont[i]->requestdma(requestwrappertodma[i]);

      //connect the DMA controller to the DMA transfer module
      dmacont[i]->datadma(datadmacontroltotransfer[i]);
      dmacont[i]->finished(finishedtransfer[i]);
      dmacont[i]->requestdmatransfer(requestcontroltotransfer[i]);
      transf[i]->datadma(datadmacontroltotransfer[i]);
      transf[i]->finished(finishedtransfer[i]);
      transf[i]->requestdmatransfer(requestcontroltotransfer[i]);

      //connect the DMA transfer module to the DMA bus master
      transf[i]->pinout(pinoutmast[N_CORES+i]);
      transf[i]->ready_from_master(readymast[N_CORES+i]);
      transf[i]->request_to_master(requestmast[N_CORES+i]);
    }
  }
  #endif

  // AMBA memory slaves. Their instantiation must be done
  // before that of scratchpad memories, because if SPCHECK
  // is active, scratchpad memories must fetch initialization
  // data from external memories
  amba_occn_slavemem *s0[N_PRIVATE + N_SHARED + N_SEMAPHORE];
  for (i=0; i<(N_PRIVATE + N_SHARED + N_SEMAPHORE); i++)
  {
    unsigned int start, finish;
    sprintf(buffer, "s%d", i);
    s0[i]= new amba_occn_slavemem(buffer,i);
    s0[i]->clock(ClockGen_1);
    
    s0[i]->port(*my_ahb);
    start = addresser->ReturnSlavePhysicalAddress(i);
    finish = start + addresser->ReturnSlaveSize(i);
    s0[i]->port->set_slave_address_range(start, finish);
  }

  // AMBA interrupt slaves
  amba_occn_slaveint *s1[N_INTERRUPT];
  for (i=0; i<N_INTERRUPT; i++)
  {
      unsigned int start, finish;
    sprintf(buffer, "s%d", N_PRIVATE + N_SHARED + N_SEMAPHORE + i);
    s1[i]= new amba_occn_slaveint(buffer, N_PRIVATE + N_SHARED + N_SEMAPHORE + i);
    s1[i]->clock(ClockGen_1);
    s1[i]->port(*my_ahb);
    for (j=0; j<N_CORES; j++)
      s1[i]->extint[j](extint[(j*NUMBER_OF_EXT)+i]);
    start = addresser->ReturnSlavePhysicalAddress(N_PRIVATE + N_SHARED + N_SEMAPHORE + i);
    if (addresser->IsInterrupt(N_PRIVATE + N_SHARED + N_SEMAPHORE + i))
      finish = start + addresser->ReturnInterruptSize();
    else
        {
          printf("Fatal Error: Error in address boundary setting!\n");
          exit(1);
        }
    s1[i]->port->set_slave_address_range(start, finish);    
  }

#ifdef PRINTDEBUG
  cout << "Platform instantiation completed!" << endl;
#endif
}
