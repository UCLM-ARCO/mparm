/***************************************************

 Author : Carlo Spitale
 CMG Design FMVG STMicroelectronics
 
 Modified by: DEIS - Università di Bologna

****************************************************/

#include <math.h>
#include <systemc.h>
#include "globals.h"
#include "st_signal.h"
#include "address.h"
#include "stats.h"
#include "smartmem_signal.h"
#include "mem_class.h"
#include "scratch_mem.h"
#include "st_builder.h"
#include "wrapper.h"
#include "stbus_protocol_defs.h"
#include "stbus_opcodes.h"
#include "SC_RESET_GEN.h"
#include "SC_INITIATOR_T3.h"
#include "SC_INITIATOR_T2.h"
#include "SC_TARGET_T3.h"
#include "SC_TARGET_T2.h"
#include "SC_TARGET_T1.h"
#include "SMART_TARGET_T3.h"
#include "st_dma_scratch.h"
#include "ST_PROG_FREQ_REG.h"
#include "ST_DMA_SMARTMEM.h"
#include "CORE_TARGET_T3.h"
#include "clock_tree.h"
#include "reset.h"
#include "freq_register.h"
#include "dual_clock_pinout_adapter.h"

#ifdef IP_TG
#include "tlm_module.h"
#include "iptg_stbus_bca_initiator.h"
#endif

#include "EXT_MEMORY_TARGET.h"

//this enables sharedbus configuration
#define stSHAREDst
//this enables fullcrossbar configuration
//#define stFULLst
//Picciano this enables partial crossbar configuration:
//If you want to define new configuration, just copy correspondent file on Node_1-8x844partial.h or Node_1-32x32partial.h
//#define stPARTIALst

#include "shared_8x8ICN.h"
#include "shared_32x32ICN.h"
#include "snoopdev.h"

#include "full_32x32ICN.h"
#include "full_8x8ICN.h"

#include "partial_8x8ICN.h" 
#include "partial_32x32ICN.h"


#ifdef SWARMBUILD
#include "st_initiator.h"
#endif

#ifdef LXBUILD
#include "st_initiator_lx.h"
#endif

#ifndef INI_DEBUG
#ifdef WITH_INI_DEBUG
  #define INI_DEBUG true
#else
  #define INI_DEBUG false
#endif
#endif

#ifdef SWARMBUILD
STBus_initiator< uint, uint, uchar, 32, 16, 0, true, INI_DEBUG >  *init0=NULL;
STBus_initiator< uint, uint, uchar, 32, 16, 1, true, INI_DEBUG >  *init1=NULL;
STBus_initiator< uint, uint, uchar, 32, 16, 2, true, INI_DEBUG >  *init2=NULL;
STBus_initiator< uint, uint, uchar, 32, 16, 3, true, INI_DEBUG >  *init3=NULL;
STBus_initiator< uint, uint, uchar, 32, 16, 4, true, INI_DEBUG >  *init4=NULL;
STBus_initiator< uint, uint, uchar, 32, 16, 5, true, INI_DEBUG >  *init5=NULL;
STBus_initiator< uint, uint, uchar, 32, 16, 6, true, INI_DEBUG >  *init6=NULL;
STBus_initiator< uint, uint, uchar, 32, 16, 7, true, INI_DEBUG >  *init7=NULL;
STBus_initiator< uint, uint, uchar, 32, 16, 8, true, INI_DEBUG >  *init8=NULL;
STBus_initiator< uint, uint, uchar, 32, 16, 9, true, INI_DEBUG >  *init9=NULL;
STBus_initiator< uint, uint, uchar, 32, 16, 10, true, INI_DEBUG > *init10=NULL;
STBus_initiator< uint, uint, uchar, 32, 16, 11, true, INI_DEBUG > *init11=NULL;
STBus_initiator< uint, uint, uchar, 32, 16, 12, true, INI_DEBUG > *init12=NULL;
STBus_initiator< uint, uint, uchar, 32, 16, 13, true, INI_DEBUG > *init13=NULL;
STBus_initiator< uint, uint, uchar, 32, 16, 14, true, INI_DEBUG > *init14=NULL;
STBus_initiator< uint, uint, uchar, 32, 16, 15, true, INI_DEBUG > *init15=NULL;
STBus_initiator< uint, uint, uchar, 32, 16, 16, true, INI_DEBUG > *init16=NULL;
STBus_initiator< uint, uint, uchar, 32, 16, 17, true, INI_DEBUG > *init17=NULL;
STBus_initiator< uint, uint, uchar, 32, 16, 18, true, INI_DEBUG > *init18=NULL;
STBus_initiator< uint, uint, uchar, 32, 16, 19, true, INI_DEBUG > *init19=NULL;
#endif


#ifdef LXBUILD
#include "ast_iss_wrapper.h"

#ifdef DRAMBUILD
    #include "dma2control.h"
    #include "dma2transfer.h"
    #include "sdram.h"
    #include "statemachine3.h"
    #include "DRAMDMA_TARGET.h"
#endif

//Poggiali
ASTISSWrapper **lx;

//LX
STBus_initiator_lx< uint, uint, uchar, 32, 16, 0, true, INI_DEBUG >  *init0_lx=NULL;
STBus_initiator_lx< uint, uint, uchar, 32, 16, 1, true, INI_DEBUG >  *init1_lx=NULL;
STBus_initiator_lx< uint, uint, uchar, 32, 16, 2, true, INI_DEBUG >  *init2_lx=NULL;
STBus_initiator_lx< uint, uint, uchar, 32, 16, 3, true, INI_DEBUG >  *init3_lx=NULL;
STBus_initiator_lx< uint, uint, uchar, 32, 16, 4, true, INI_DEBUG >  *init4_lx=NULL;
STBus_initiator_lx< uint, uint, uchar, 32, 16, 5, true, INI_DEBUG >  *init5_lx=NULL;
STBus_initiator_lx< uint, uint, uchar, 32, 16, 6, true, INI_DEBUG >  *init6_lx=NULL;
STBus_initiator_lx< uint, uint, uchar, 32, 16, 7, true, INI_DEBUG >  *init7_lx=NULL;
STBus_initiator_lx< uint, uint, uchar, 32, 16, 8, true, INI_DEBUG >  *init8_lx=NULL;
STBus_initiator_lx< uint, uint, uchar, 32, 16, 9, true, INI_DEBUG >  *init9_lx=NULL;
STBus_initiator_lx< uint, uint, uchar, 32, 16, 10, true, INI_DEBUG > *init10_lx=NULL;
STBus_initiator_lx< uint, uint, uchar, 32, 16, 11, true, INI_DEBUG > *init11_lx=NULL;
STBus_initiator_lx< uint, uint, uchar, 32, 16, 12, true, INI_DEBUG > *init12_lx=NULL;
STBus_initiator_lx< uint, uint, uchar, 32, 16, 13, true, INI_DEBUG > *init13_lx=NULL;
STBus_initiator_lx< uint, uint, uchar, 32, 16, 14, true, INI_DEBUG > *init14_lx=NULL;
STBus_initiator_lx< uint, uint, uchar, 32, 16, 15, true, INI_DEBUG > *init15_lx=NULL;
STBus_initiator_lx< uint, uint, uchar, 32, 16, 16, true, INI_DEBUG > *init16_lx=NULL;
STBus_initiator_lx< uint, uint, uchar, 32, 16, 17, true, INI_DEBUG > *init17_lx=NULL;
STBus_initiator_lx< uint, uint, uchar, 32, 16, 18, true, INI_DEBUG > *init18_lx=NULL;
STBus_initiator_lx< uint, uint, uchar, 32, 16, 19, true, INI_DEBUG > *init19_lx=NULL;
#endif

void buildSTBusPlatform(int new_argc, char *new_argv[], char *new_envp[])
{
	int i,j,k;
        char buffer[40];
        
        // Clock signals and dividers
        init_clock = new sc_signal< bool > [N_MASTERS];
        init_div = new sc_signal< sc_uint<FREQSCALING_BITS> > [N_CORES];
        interconnect_clock = new sc_signal< bool > [N_BUSES];
        interconnect_div = new sc_signal< sc_uint<FREQSCALING_BITS> > [N_BUSES];
        // System wires
        readymast = new sc_signal<bool> [32];
        pinoutmast = new sc_signal<PINOUT> [32];
        requestmast = new sc_signal<bool> [32];
        readymast_interconnect = new sc_signal<bool> [32];
        pinoutmast_interconnect = new sc_signal<PINOUT> [32];
        requestmast_interconnect = new sc_signal<bool> [32];
        initiator_to_node = new stbus_T2T3_link< uint, uint, uchar> [32];
        node_to_target = new stbus_T2T3_link< uint, uint, uchar> [32]; 

        extint = new sc_signal<bool> [NUMBER_OF_EXT*N_CORES];
        
#ifdef WITH_POWER_NODE
#include "power_stbus_sysc_lib.h"
         /* Initialize power engine */
         pow_load_power_model_library();
#endif

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

  // marchal: create signals for the smart memories
  if (SMARTMEM)
  {
    smartmem_pinoutslavetodma = new sc_signal<PINOUT> [N_SMARTMEM];
    smartmem_readyslavetodma = new sc_signal<bool> [N_SMARTMEM];
    smartmem_requestslavetodma = new sc_signal<bool> [N_SMARTMEM];
    smartmem_datadmacontroltotransfer = 
      new sc_signal<DMA_CONT_REG1> [N_SMARTMEM];
    smartmem_requestcontroltotransfer = new sc_signal<bool> [N_SMARTMEM];
    smartmem_finishedtransfer = new sc_signal<bool> [N_SMARTMEM];
    smartmem_spinoutscratchdma = new sc_signal<PINOUT> [N_SMARTMEM];
    smartmem_sreadyscratchdma = new sc_signal<bool> [N_SMARTMEM];
    smartmem_srequestscratchdma = new sc_signal<bool> [N_SMARTMEM];
  }

	/*** reset generator definitions ***/
	SC_RESET_GEN< 10 > * ResetGen_1_gen = new SC_RESET_GEN< 10 > ( "ResetGen_1" );
	ResetGen_1_gen->rst( ResetGen_1 );
	ResetGen_1_gen->clk( ClockGen_1 );

	/*** Top Level IPs instantiation ***/
  
  // Frequency divider
  if (FREQSCALING || FREQSCALINGDEVICE)
  {
    clock_tree *ctree;
    ctree = new clock_tree("clock_tree_gen", N_FREQ_DEVICE + N_BUSES);
    ctree->Clock_in(ClockGen_1);
    ctree->Reset(ResetGen_1);
    for (i = 0; i < N_FREQ_DEVICE; i ++)
    {
      ctree->Div[i](init_div[i]);
      ctree->Clock_out[i](init_clock[i]);
    }
    for (i = 0; i < N_BUSES; i ++)
    {
      ctree->Div[N_FREQ_DEVICE + i](interconnect_div[i]);
      ctree->Clock_out[N_FREQ_DEVICE + i](interconnect_clock[i]);
    }
    
    // Device taking care of frequency divider generation
    ST_PROG_FREQ_REG< uint, uint, uchar, 32, 2, 1, 0 > *prog_register;
    feeder *p_register;
    if (FREQSCALINGDEVICE)
    {
      // A bus-attached slave device
      prog_register = new ST_PROG_FREQ_REG< uint, uint, uchar, 32, 2, 1, 0 >
        ("prog_register", addresser->FreqStartID(),
        FREQ_BASE, FREQ_SIZE, INT_WS, 0, N_FREQ_DEVICE + N_BUSES);
      prog_register->clk(interconnect_clock[0]);
      prog_register->rst(ResetGen_1);
      prog_register->In(node_to_target[addresser->FreqStartID()]);
      
      for (i = 0; i < N_FREQ_DEVICE; i ++)
        prog_register->p_feed[i](init_div[i]);
      for (i = 0; i < N_BUSES; i ++)
        prog_register->p_feed[N_CORES + i](interconnect_div[i]);
    }
    else
    {
      // A statically-programmed device
      p_register = new feeder("p_register", N_FREQ_DEVICE + N_BUSES);
      p_register->clock(ClockGen_1);
      
      for (i = 0; i < N_FREQ_DEVICE; i ++)
        p_register->feed[i](init_div[i]);
      for (i = 0; i < N_BUSES; i ++)
        p_register->feed[N_FREQ_DEVICE + i](interconnect_div[i]);
    }

    // Frequency controller interface
    dual_clock_pinout_adapter *fifo[N_FREQ_DEVICE]; 
    for (i = 0; i < N_FREQ_DEVICE; i ++)
    {
      if (M_DIVIDER[i] != I_DIVIDER[MASTER_CONFIG[i].binding] || FREQSCALINGDEVICE)
      {
        sprintf(buffer, "dual_clock_fifo_%d", i);
        fifo[i] = new dual_clock_pinout_adapter(buffer);
      
        fifo[i]->clk_s(init_clock[i]);
        fifo[i]->rst_s(ResetGen_1);
        fifo[i]->request_from_wrapper(requestmast[i]);
        fifo[i]->ready_to_wrapper(readymast[i]);
        fifo[i]->pinout_s(pinoutmast[i]);
        
        fifo[i]->clk_m(interconnect_clock[MASTER_CONFIG[i].binding]);
        fifo[i]->rst_m(ResetGen_1);
        fifo[i]->request_to_master(requestmast_interconnect[i]);
        fifo[i]->ready_from_master(readymast_interconnect[i]);
        fifo[i]->pinout_m(pinoutmast_interconnect[i]);
      }
    }
  }

/* Target Memories */
#ifdef SWARMBUILD
// Their instantiation must be done before that of scratchpad memories, because if SPCHECK is active,
// scratchpad memories must fetch initialization data from external memories
 EXT_MEMORY_TARGET < uint, uint, uchar, 32, 2, 1, 0 > 
  *TargetMem_inst[N_PRIVATE];

if(CURRENT_ISS == SWARM)
{
  for(i=0;i<N_CORES;i++)
  {
   sprintf(buffer, "s%d", i);
   TargetMem_inst[i]=new EXT_MEMORY_TARGET< uint, uint, uchar, 32, 2, 1, 0 >
    (buffer,i,
     addresser->ReturnSlavePhysicalAddress(i),
     addresser->ReturnPrivateSize(),
     MEM_IN_WS,MEM_BB_WS);
   if (FREQSCALING || FREQSCALINGDEVICE)
     TargetMem_inst[i]-> clk( interconnect_clock[0] ); // FIXME [0]
   else
     TargetMem_inst[i]-> clk( ClockGen_1 );
   TargetMem_inst[i]->rst( ResetGen_1 );
   TargetMem_inst[i]->In( node_to_target[i] );
  }
}  
#endif
  
#ifdef LXBUILD
 uint16_t lxmemstartid=addresser->PrivateLXStartID();

 EXT_MEMORY_LX_TARGET < uint, uint, uchar, 32, 2, 1, 0 > 
 *TargetMem_lx_inst[N_LX_PRIVATE];

if(CURRENT_ISS == LX)
{
  for(i=0;i<N_CORES;i++)
  {
   sprintf(buffer, "lx_s%d", i);
   TargetMem_lx_inst[i]=new EXT_MEMORY_LX_TARGET< uint, uint, uchar, 32, 2, 1, 0 >
    (buffer,lxmemstartid+i,
     addresser->ReturnSlavePhysicalAddress(lxmemstartid+i),
     addresser->ReturnPrivateLXSize(),
     MEM_IN_WS,MEM_BB_WS);
   if (FREQSCALING || FREQSCALINGDEVICE)
     TargetMem_lx_inst[lxmemstartid+i]-> clk( interconnect_clock[0] ); // FIXME [0]
   else
     TargetMem_lx_inst[lxmemstartid+i]-> clk( ClockGen_1 );
   TargetMem_lx_inst[lxmemstartid+i]->rst( ResetGen_1 );
   TargetMem_lx_inst[lxmemstartid+i]->In( node_to_target[lxmemstartid+i] );
  }
}  
#endif

  uint16_t sharedstartid=addresser->SharedStartID();
  EXT_SHARED_TARGET < uint, uint, uchar, 32, 2, 1, 0 > 
  *TargetShared_inst[N_SHARED];
      
  for(i=0;i<N_SHARED;i++)
  {      
   sprintf(buffer, "SharedMem_%d",sharedstartid+i);
   TargetShared_inst[i]=new EXT_SHARED_TARGET< uint, uint, uchar, 32, 2, 1, 0 >
    (buffer,sharedstartid+i,
     addresser->ReturnSlavePhysicalAddress(i+sharedstartid),
     addresser->ReturnSharedSize(i+sharedstartid),
     MEM_IN_WS,MEM_BB_WS);
   if (FREQSCALING || FREQSCALINGDEVICE)
     TargetShared_inst[i]->clk( interconnect_clock[0] ); // FIXME [0]
   else
     TargetShared_inst[i]->clk( ClockGen_1 );
   TargetShared_inst[i]->rst( ResetGen_1 );
   TargetShared_inst[i]->In( node_to_target[i+sharedstartid] );
  }
  
  uint16_t semaphorestartid=addresser->SemaphoreStartID();
  EXT_SEMAPHORE_TARGET < uint, uint, uchar, 32, 2, 1, 0 > 
  *TargetSemaphore_inst[N_SEMAPHORE];
  
  for(i=0;i<N_SEMAPHORE;i++)
  {
   sprintf(buffer, "SemaphoreMem_%d",semaphorestartid+i);
   TargetSemaphore_inst[i]=new EXT_SEMAPHORE_TARGET< uint, uint, uchar, 32, 2, 1, 0 >
    (buffer,semaphorestartid+i,
     addresser->ReturnSlavePhysicalAddress(i+semaphorestartid),
     addresser->ReturnSemaphoreSize(semaphorestartid+i),
     1,1);
   if (FREQSCALING || FREQSCALINGDEVICE)
     TargetSemaphore_inst[i]->clk( interconnect_clock[0] ); // FIXME [0]
   else
     TargetSemaphore_inst[i]-> clk( ClockGen_1 );
   TargetSemaphore_inst[i]->rst( ResetGen_1 );
   TargetSemaphore_inst[i]->In( node_to_target[i+semaphorestartid] );
  }
  
  uint16_t interruptstartid=addresser->InterruptStartID();
  EXT_INTERRUPT_TARGET< uint, uint, uchar, 32, 2, 1, 0 > 
  *TargetMem_interrupt_inst[N_INTERRUPT];
  
  for(i=0;i<N_INTERRUPT;i++)
  { 
   sprintf(buffer, "Intdev_%d",interruptstartid+i);
   TargetMem_interrupt_inst[i]=new EXT_INTERRUPT_TARGET< uint, uint, uchar, 32, 2, 1, 0 >
   (buffer,interruptstartid+i,
    addresser->ReturnSlavePhysicalAddress(i+interruptstartid),
    addresser->ReturnInterruptSize(),
    INT_WS,INT_WS);
   if (FREQSCALING || FREQSCALINGDEVICE)
     TargetMem_interrupt_inst[i]->clk( interconnect_clock[0] ); // FIXME [0]
   else
     TargetMem_interrupt_inst[i]->clk( ClockGen_1 );
   TargetMem_interrupt_inst[i]->rst( ResetGen_1 );
   TargetMem_interrupt_inst[i]->In( node_to_target[interruptstartid+i] );
   
   for (j=0; j<N_CORES; j++)
   {
    TargetMem_interrupt_inst[i]->extint[j](extint[j*NUMBER_OF_EXT+i]);
   }
  }
  // Marchal
  // create the smart-memory memories
  Scratch_mem *smartscratch[N_SMARTMEM];
  
  uint16_t slavestartid=addresser->Smartmem_slave_StartID();
  uint16_t masterstartid=addresser->Smartmem_master_StartID();
  
  for (i=0; i<N_SMARTMEM; i++)
    {
      if (SMARTMEM)
      {
       smartscratch[i] = 
	  new  Scratch_mem("Smart_mem",i,SMART_MEM_SIZE[i]-SMARTMEM_DMA_SIZE,
	  	(addresser->ReturnSlavePhysicalAddress(slavestartid+i))+SMARTMEM_DMA_SIZE);
      }	
      else
	smartscratch[i] = NULL;
      
    }
    
   if(SMARTMEM) 
   {
    EXT_SMART_TARGET< uint, uint, uchar, 32, 2, 1, 0 >* s3[N_SMARTMEM];
    St_Dma_control_smartmem* smartdmacont[N_SMARTMEM];
    St_Dma_transfer_smartmem* smarttransf[N_SMARTMEM];
    
    for (i=0; i<N_SMARTMEM; i++)
    {
     sprintf(buffer, "smart_target%d", i);
     s3[i]=new EXT_SMART_TARGET< uint, uint, uchar, 32, 2, 1, 0 >
     (buffer,slavestartid+i,masterstartid+i,
      addresser->ReturnSlavePhysicalAddress(slavestartid+i),
      SMART_MEM_SIZE[i],SMART_MEM_IN_WS[i],SMART_MEM_BB_WS[i],
      smartscratch[i]);
      if (FREQSCALING || FREQSCALINGDEVICE)
        s3[i]->clk( interconnect_clock[0] );
      else
        s3[i]->clk( ClockGen_1 );
     s3[i]->rst( ResetGen_1 );
     s3[i]->In( node_to_target[slavestartid+i] );
     //connect the SMARTMEM_SLAVE to the DMA controller
     s3[i]->pinoutdma(smartmem_pinoutslavetodma[i]);
     s3[i]->readydma(smartmem_readyslavetodma[i]);
     s3[i]->requestdma(smartmem_requestslavetodma[i]);     
     
     // create the SMARTMEM DMA controller is important to get the correct id to the DMA for
     // relate it with the correct master
     	sprintf(buffer,"dma%d",DMA*N_CORES+i);
	smartdmacont[i] =  new St_Dma_control_smartmem(buffer,DMA*N_CORES+i,SMARTMEM_MAX_OBJ,N_CORES,
	                      	addresser->ReturnSlavePhysicalAddress(slavestartid+i));
	if (FREQSCALING || FREQSCALINGDEVICE)
	  smartdmacont[i]->clock( interconnect_clock[0] ); // FIXME [0]
	else
	  smartdmacont[i]->clock(ClockGen_1);

	// create the SMARTMEM DMA transfer
	sprintf(buffer,"tra%d",DMA*N_CORES+i);
	smarttransf[i] = new St_Dma_transfer_smartmem(buffer,DMA*N_CORES+i,slavestartid+i,SMARTMEM_DIM_BURST,SMARTMEM_MAX_OBJ,
		     N_CORES,addresser->ReturnSlavePhysicalAddress(slavestartid+i),smartscratch[i]);
	if (FREQSCALING || FREQSCALINGDEVICE)
	  smarttransf[i]->clock( interconnect_clock[0] ); // FIXME [0]
	else
	  smarttransf[i]->clock(ClockGen_1);

	//connect the SMARTMEM_SLAVE to the DMA controller
	smartdmacont[i]->dmapin(smartmem_pinoutslavetodma[i]);
	smartdmacont[i]->readydma(smartmem_readyslavetodma[i]);
	smartdmacont[i]->requestdma(smartmem_requestslavetodma[i]);

	//connect the SMARTMEM DMA controller to the SMARTMEM DMA transfer module
	smartdmacont[i]->datadma(smartmem_datadmacontroltotransfer[i]);
	smartdmacont[i]->finished(smartmem_finishedtransfer[i]);
	smartdmacont[i]->requestdmatransfer(smartmem_requestcontroltotransfer[i]);
	smarttransf[i]->datadma(smartmem_datadmacontroltotransfer[i]);
	smarttransf[i]->finished(smartmem_finishedtransfer[i]);
	smarttransf[i]->requestdmatransfer(smartmem_requestcontroltotransfer[i]);

	//connect the DMA transfer module to the DMA bus master
	// second N_CORES is actually N_DMAs
	smarttransf[i]->pinout(pinoutmast[N_CORES+DMA*N_CORES+i]);
	smarttransf[i]->ready_from_master(readymast[N_CORES+DMA*N_CORES+i]);
	smarttransf[i]->request_to_master(requestmast[N_CORES+DMA*N_CORES+i]);
	
	for (j=0; j<N_CORES; j++)
	  for (k=0; k<SMARTMEM_MAX_OBJ; k++)
	   {
	    #if 0
	    printf("smart:%d,core:%d,obj:%d,smart->ext:%d,signal:%d\n",
	    i,j,k,j*SMARTMEM_MAX_OBJ+k,
	    (NUMBER_OF_EXT*j)+k+(SMARTMEM_MAX_OBJ*i)+N_INTERRUPT);
	    #endif
	    smarttransf[i]->extint[j*SMARTMEM_MAX_OBJ+k]
		(extint[(NUMBER_OF_EXT*j)+k+(SMARTMEM_MAX_OBJ*i)+N_INTERRUPT+CORESLAVE]);
	   }
    }//end of for N_SMARTMEM
   }//end of if(SMARTMEM)
       
	// Scratchpad memories
        Mem_class *datascratch[N_CORES];
	for (i=0; i<N_CORES; i++)
  	{
    	 if (SCRATCH && SPCHECK)
      	 datascratch[i] = new Scratch_part_mem(i,addresser->ReturnScratchSize());
     	 else
      	 if (SCRATCH)
      	    datascratch[i] = new Scratch_mem("Scratchpad",i,addresser->ReturnScratchSize(),addresser->ReturnScratchPhysicalAddress(i));
    	      else
      	      datascratch[i] = NULL;
  	}

  	Scratch_queue_mem *queue[N_CORES];
  
   	for (i=0; i<N_CORESLAVE; i++)
    	{
     	if(CORESLAVE)       
	 {
          sprintf(buffer, "Scratch-semaphore%d",i);
          queue[i] = new Scratch_queue_mem(buffer,i,addresser->ReturnScratchSize(),
	                                   addresser->ReturnQueuePhysicalAddress(i));
          queue[i]->sendint(extint[(i*NUMBER_OF_EXT)+N_INTERRUPT]);
         }
     	else
       	queue[i] = NULL;
    	}

   if(CORESLAVE)
   {
    CORESLAVE_T3< uint, uint, uchar, 32, 2, 1, 0 > *coreslave[N_CORESLAVE];
 
    uint16_t core_slavestartid=addresser->CoreSlaveStartID(); 
 
    for (i=0; i<N_CORESLAVE; i++)
    {
     sprintf(buffer, "TargetCore_%d", i);
     coreslave[i] = new CORESLAVE_T3< uint, uint, uchar, 32, 2, 1, 0 > 
     (buffer,core_slavestartid+i,i,
      addresser->ReturnSlavePhysicalAddress(core_slavestartid+i),
      addresser->ReturnCoreSlaveSize(),
      1,1,datascratch[i],queue[i]);
    if (FREQSCALING || FREQSCALINGDEVICE)
      coreslave[i]->clk( interconnect_clock[0] ); // FIXME [0]
    else
      coreslave[i]->clk( ClockGen_1 );
    coreslave[i]->rst( ResetGen_1 );
    coreslave[i]->In( node_to_target[core_slavestartid+i] );
   }
  }
  
  
	       
//==============================================================================
// SWARM PROCESSORS 	   
#ifdef SWARMBUILD
        // ARM processors
        armsystem *soc[N_CORES];

if(CURRENT_ISS == SWARM)
{
        for (i=0; i<N_CORES; i++)
        {
          sprintf(buffer, "Arm_System%d", i);
          soc[i] = new armsystem(buffer, (unsigned int)i, datascratch[i], queue[i]);
          if (FREQSCALING || FREQSCALINGDEVICE)
            soc[i]->clock(init_clock[i]);
          else
            soc[i]->clock(ClockGen_1);
          soc[i]->pinout_ft_master[0](pinoutmast[i]);
          soc[i]->ready_from_master[0](readymast[i]);
          soc[i]->request_to_master[0](requestmast[i]);
          for (j=0; j<NUMBER_OF_EXT; j++)
             soc[i]->extint[j](extint[i*NUMBER_OF_EXT + j]);
        }

 #include "initiator_t3_inst.cpp"
	  
	  // DMA support
  	  if (DMA)
  	  {
    	   St_Dma_control_scratch *dmacont[N_CORES];
    	   St_Dma_transfer_scratch *transf[N_CORES];

    	   for (i=0; i<N_CORES; i++)
    	   {
      	    //instance a DMA controller
            sprintf(buffer,"dma%d",i);
            dmacont[i] = new St_Dma_control_scratch(buffer,i,INTERNAL_MAX_OBJ,1,addresser->ReturnDMAPhysicalAddress());
            if (FREQSCALING || FREQSCALINGDEVICE)
              dmacont[i]->clock(init_clock[i]);
            else
              dmacont[i]->clock(ClockGen_1);
      
            //instance a DMA transfer module
            sprintf(buffer,"tra%d", i);
            transf[i] = new St_Dma_transfer_scratch(buffer,i,INTERNAL_DIM_BURST,INTERNAL_MAX_OBJ,1,datascratch[i]);
            if (FREQSCALING || FREQSCALINGDEVICE)
              transf[i]->clock(init_clock[i]);
            else
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
            if (FREQSCALING || FREQSCALINGDEVICE)
            {
              transf[i]->pinout(pinoutmast_interconnect[N_CORES+i]);
              transf[i]->ready_from_master(readymast_interconnect[N_CORES+i]);
              transf[i]->request_to_master(requestmast_interconnect[N_CORES+i]);
            }
            else
            {
              transf[i]->pinout(pinoutmast[N_CORES+i]);
              transf[i]->ready_from_master(readymast[N_CORES+i]);
              transf[i]->request_to_master(requestmast[N_CORES+i]);
            }
           } 
          }	
}	     
#endif
//end of SWARMBUILD

//==============================================================================
// LX PROCESSORS  
#ifdef LXBUILD
if(CURRENT_ISS == LX)
{
 #include "initiator_t3_inst_lx.cpp"

 //ASTISSWrapper **lx;
  lx=new ASTISSWrapper*[N_MASTERS];

  for(i=0;i<N_CORES;i++)
  {   
   sprintf(buffer, "Lx_%d",i);

//Aggiunto un nuovo buffer per contenere il nome del target
   char targetFilename[40];
   sprintf(targetFilename, "my_appli%d.elf",i);
   
   char* execPath="$SWARMDIR/bin/mpsim.x";
   char* statPath=".";   

//Creo un nuovo vector con un solo elemento (il nome del target) in modo che l'ISS vada a leggere dal file targets.cfg
   std::vector<std::string> fakeArguments;
   fakeArguments.push_back(targetFilename);
   //myVector.push_back(std::string(buffer2));
   
   lx[i] = new ASTISSWrapper(	buffer,  //INVARIATO
   				execPath,		//Dovrebbe essere il nome di mparm
				ASTISSWrapper::LITTLE,
				targetFilename,
				fakeArguments,
				new_envp, //INVARIATO
				0,  //INVARIATO
				0,
				0,
				false,//abilita le stampe di debug del wrapper
				false,
				false, //dovrebbe fare i dump a terminazione conclusa (ma bisogna invocare il distruggittore)
				statPath);
   
   if (FREQSCALING || FREQSCALINGDEVICE)
        lx[i]->clk(init_clock[i]);
          else
        lx[i]->clk(ClockGen_1);
   
   for(j=0;j<N_INTERRUPT;j++) lx[i]->irq_port(extint[i*N_INTERRUPT+j], 32+j);

  }
  
  //connessione wrapper 
switch(N_CORES){
 case 12: lx[11]->io_port(*init11_lx);
 case 11: lx[10]->io_port(*init10_lx);
 case 10: lx[9]->io_port(*init9_lx);
 case 9:  lx[8]->io_port(*init8_lx);
 case 8:  lx[7]->io_port(*init7_lx);
 case 7:  lx[6]->io_port(*init6_lx);
 case 6:  lx[5]->io_port(*init5_lx);
 case 5:  lx[4]->io_port(*init4_lx);
 case 4:  lx[3]->io_port(*init3_lx);
 case 3:  lx[2]->io_port(*init2_lx);
 case 2:  lx[1]->io_port(*init1_lx);
 case 1:  lx[0]->io_port(*init0_lx);
 break;
 default:
  printf("st_builder.cpp Warning you can't instance more then 10 LX CORES");
  exit(0);
}

}
//END OF LX PROCESSORS  
//==============================================================================
#endif

#ifdef IP_TG
iptg_stbus_bca_initiator <uint, uint, 4, unsigned char > * traff_inst[N_IP_TG];
     for (i=0; i<N_IP_TG; i++)
      {
       sprintf(buffer, "IP_TG%d", i);
       traff_inst[i] = new iptg_stbus_bca_initiator <uint, uint, 4, unsigned char> (buffer, N_MASTERS+i, buffer, STBUS_T3);
       if (FREQSCALING || FREQSCALINGDEVICE)
        traff_inst[i]->clk(interconnect_clock[0]);
       else
        traff_inst[i]->clk(ClockGen_1);
        traff_inst[i]->rst( ResetGen_1 );
        traff_inst[i]->Out(initiator_to_node[N_MASTERS+i]);
      }
#endif

/* The snoop device */
#ifndef NOSNOOP
#ifndef INITIAL_DEBUGGING
        snoopdev *snoop[i];
#endif
        if (SNOOPING && CURRENT_ISS == SWARM)
        {
#ifndef stSHAREDst
          fprintf(stderr, "Actually the snooping otion is valid only for a "
                          "shared bus topology\n");
          exit(1);
#endif

          for (i=0; i<N_CORES; i++)
          {
            sprintf(buffer, "Snoopdev%d", i);
            snoop[i] = new snoopdev(buffer);
            snoop[i]->the_ARM = soc[i]->pArm;
            if (FREQSCALING || FREQSCALINGDEVICE)
              snoop[i]->clock( interconnect_clock[0] ); // FIXME [0]
            else
              snoop[i]->clock(ClockGen_1);
            snoop[i]->SnoopPort_d[0](SnoopSignal_d[0]);
            snoop[i]->SnoopPort_r[0](SnoopSignal_r[0]);
               /* link the wrapper and the snoop device */
            //soc[i]->SnoopDevice = snoop[i];
          }
        }
#endif
#ifdef LXBUILD
#ifdef DRAMBUILD 
   if(DRAM && CURRENT_ISS == LX)
   {
   uint16_t dramdmastartid=addresser->DRAM_StartID(); 
   //Picciano Poletti DRAMDMA FIXME DA definire meglio l'interfacciamento con il dmacontrol e quindi anche 
   //il discorso dei delay
   EXT_DRAMDMA_TARGET< uint, uint, uchar, 32, 2, 1, 0 >* dramdmatarget;
   sprintf(buffer, "DRAMDMA_target%d", i);
     dramdmatarget=new EXT_DRAMDMA_TARGET< uint, uint, uchar, 32, 2, 1, 0 >
     (buffer,dramdmastartid,addresser->ReturnSlavePhysicalAddress(dramdmastartid),
      DRAMDMA_SIZE,0,0);
         
   statemachine3* FSM;
   FSM = new statemachine3("LMIfsm");

   sdram* memory;
   memory = new sdram("DRAMmemory");
   
   //Picciano Poletti DRAMDMA FIXME DA definire meglio il secondo parametro 
   //dma2control control("DRAMDMAcontrol",DRAMDMA_MAX_OBJ,N_CORES);
   dma2control* control;
   control = new dma2control("DRAMDMAcontrol",DRAMDMA_BASE,DRAMDMA_MAX_OBJ,N_CORES,0);

   dma2transfer* transfer;
   transfer = new dma2transfer("DRAMDMAtransfer");

     S_FPOP_PROCESSOR = new sc_signal<bool>;
     S_FPUSH_PROCESSOR = new sc_signal<bool>;
     S_FCOUNT = new sc_signal<uint>;
     S_DATA_SDRAM = new sc_signal<uint>;
     S_DATA_DIMENSION = new sc_signal<uint>; //porta per dare la dimensione del burst

     for(int h=0;h<BUS_MAX_BURST_LENGHT;h++)
     {
	    S_DATA_PROCESSOR[h] = new sc_signal<uint>;
     }

     S_DMAPIN = new sc_signal<PINOUT>;
     S_READYDMA = new sc_signal<bool>;
     S_REQUESTDMA = new sc_signal<bool>;

     S_DATADMA = new sc_signal<DMA_CONT_REG1>;		
     S_REQUESTDMATRANSFER = new sc_signal<bool>;		
     S_FINISHED = new sc_signal<bool>;				
     S_FINISHED_TRANSFER_ID = new sc_signal<uint>; 
     S_FINISHED_TRANSFER_PROC = new sc_signal<uint>;
     S_TRANSFER_ACK = new sc_signal<bool>;

     // Segnali tra transfer e MASTER-ST

     // Segnali tra transfer e LMI
     S_READY = new sc_signal<bool>;
     S_REQUEST = new sc_signal<bool>;
     S_ADDRESSBUS = new sc_signal<uint>;
     S_SIZEBUS = new sc_signal<uint>;
     S_RD = new sc_signal<bool>;
     S_DATA_OUT_PROC = new sc_signal<bool>;

     // Segnali tra LMI e SDRAM
     S_COMMAND = new sc_signal<uint>;
     S_AB = new sc_signal<uint>;
     S_BA = new sc_signal<uint>;
     S_DQM_SIGNAL = new sc_signal<bool>;

//Wiring dell'interfaccia STslave
if (FREQSCALING || FREQSCALINGDEVICE)
 dramdmatarget->clk( interconnect_clock[0] );
else
 dramdmatarget->clk( ClockGen_1 );
dramdmatarget->rst( ResetGen_1 );
dramdmatarget->In( node_to_target[dramdmastartid] );
dramdmatarget->pinoutdma(*S_DMAPIN);
dramdmatarget->readydma(*S_READYDMA);
dramdmatarget->requestdma(*S_REQUESTDMA); 

// Wiring del DMA-Control

// lato STBUS
if (FREQSCALING || FREQSCALINGDEVICE)
 control->clock( interconnect_clock[0] );
else
 control->clock(ClockGen_1);
control->dmapin(*S_DMAPIN);
control->readydma(*S_READYDMA);
control->requestdma(*S_REQUESTDMA);

// Lato transfer
control->datadma(*S_DATADMA);
control->requestdmatransfer(*S_REQUESTDMATRANSFER);
control->finished(*S_FINISHED);
control->finished_transfer_id(*S_FINISHED_TRANSFER_ID);
control->finished_transfer_proc(*S_FINISHED_TRANSFER_PROC);
control->transfer_ack(*S_TRANSFER_ACK);

// Wiring del DMA-transfer
if (FREQSCALING || FREQSCALINGDEVICE)
 transfer->clock( interconnect_clock[0] );
else
 transfer->clock(ClockGen_1);

transfer->reset_transfer(ResetGen_1);

// lato control
transfer->datadma(*S_DATADMA);		
transfer->requestdmatransfer(*S_REQUESTDMATRANSFER);		
transfer->finished(*S_FINISHED);				
transfer->finished_transfer_id(*S_FINISHED_TRANSFER_ID); 
transfer->finished_transfer_proc(*S_FINISHED_TRANSFER_PROC);
transfer->transfer_ack(*S_TRANSFER_ACK);

// lato LMI
transfer->ready(*S_READY);
transfer->request(*S_REQUEST);
transfer->AddressBus(*S_ADDRESSBUS);
transfer->SizeBus(*S_SIZEBUS);
transfer->RD(*S_RD);

// lato STBUS
switch(N_CORES){
 case 13:  transfer->stbusport(*init13_lx);break;
 case 12:  transfer->stbusport(*init12_lx);break;
 case 11:  transfer->stbusport(*init11_lx);break;
 case 10:  transfer->stbusport(*init10_lx);break;
 case 9:  transfer->stbusport(*init9_lx);break;
 case 8:  transfer->stbusport(*init8_lx);break;
 case 7:  transfer->stbusport(*init7_lx);break;
 case 6:  transfer->stbusport(*init6_lx);break;
 case 5:  transfer->stbusport(*init5_lx);break;
 case 4:  transfer->stbusport(*init4_lx);break;
 case 3:  transfer->stbusport(*init3_lx);break;
 case 2:  transfer->stbusport(*init2_lx);break;
 case 1:  transfer->stbusport(*init1_lx);break;
 default:
  printf("st_builder.cpp Warning you can't instance more then 9 LX CORES and the RAMDMA");
  exit(1);
}


// lato FIFO

transfer->fcount(*S_FCOUNT);
transfer->fpop_processor(*S_FPOP_PROCESSOR);		
transfer->fpush_processor(*S_FPUSH_PROCESSOR);
//transfer->data_processor(*S_DATA_PROCESSOR); Picciano :modifica bus tra Transfer e memory controller

for(int h=0;h<BUS_MAX_BURST_LENGHT;h++)
{
	transfer->data_processor[h](*S_DATA_PROCESSOR[h]);
}
transfer->data_out_proc(*S_DATA_OUT_PROC);

transfer->data_dimension(*S_DATA_DIMENSION);
// Wiring della FSM del Memory controller

// Lato transfer
if (FREQSCALING || FREQSCALINGDEVICE)
 FSM->clock( interconnect_clock[0] );
else
 FSM->clock(ClockGen_1);
FSM->reset_FSM(ResetGen_1);
FSM->ready(*S_READY);
FSM->request(*S_REQUEST);
FSM->AddressBus(*S_ADDRESSBUS);
FSM->SizeBus(*S_SIZEBUS);
FSM->RD(*S_RD);
FSM->data_out_proc(*S_DATA_OUT_PROC);

// Lato SDRAM

FSM->command(*S_COMMAND);
FSM->AB(*S_AB);
FSM->BA(*S_BA);
FSM->fcount(*S_FCOUNT);
//FSM->fpop_SDRAM(*S_FPOP_SDRAM);
//FSM->fpush_SDRAM(*S_FPUSH_SDRAM);
FSM->DQM_signal(*S_DQM_SIGNAL);

// Lato DATI

//FSM->data_processor(*S_DATA_PROCESSOR); //modifica bus tra Transfer e memory controller
for(int h=0;h<BUS_MAX_BURST_LENGHT;h++)
{
	FSM->data_processor[h](*S_DATA_PROCESSOR[h]);
}

FSM->data_dimension(*S_DATA_DIMENSION); 

FSM->fpop_processor(*S_FPOP_PROCESSOR);
FSM->fpush_processor(*S_FPUSH_PROCESSOR);
FSM->data_SDRAM(*S_DATA_SDRAM);

// Wiring SDRAM
if (FREQSCALING || FREQSCALINGDEVICE)
 memory->clock( interconnect_clock[0] );
else
 memory->clock(ClockGen_1);
memory->command(*S_COMMAND);
memory->AB(*S_AB);
memory->BA(*S_BA);
memory->reset_SDRAM(ResetGen_1);
//memory->fpop_SDRAM(*S_FPOP_SDRAM);
//memory->fpush_SDRAM(*S_FPUSH_SDRAM);
memory->DQM_signal(*S_DQM_SIGNAL);
memory->DATABUS(*S_DATA_SDRAM);
}
#endif
#endif

if (N_SLAVES>8 || N_MASTERS>8) 
 {
  #ifdef stSHAREDst
   shared_32x32ICN * st32x32ICN_inst;
   st32x32ICN_inst = new shared_32x32ICN ( "shared_32x32ICN" );
  #else
    #ifdef stFULLst
    full_32x32ICN * st32x32ICN_inst;
    st32x32ICN_inst = new full_32x32ICN ( "full_32x32ICN" );
    #else
       #ifdef stPARTIALst
       partial_32x32ICN * st32x32ICN_inst;
       st32x32ICN_inst = new partial_32x32ICN ( "partial_32x32ICN" );
       #else
       printf("You should define if the connection is shared or full\n");
       #endif
    #endif  
  #endif
  
 /* STBus interconnect 32x32*/
        if (FREQSCALING || FREQSCALINGDEVICE)
          st32x32ICN_inst->ClockGen_1( interconnect_clock[0] );
        else
          st32x32ICN_inst->ClockGen_1( ClockGen_1 );
	st32x32ICN_inst->ResetGen_1( ResetGen_1 );
	st32x32ICN_inst->In_1( initiator_to_node[0]);
	st32x32ICN_inst->In_2( initiator_to_node[1]);
	st32x32ICN_inst->In_3( initiator_to_node[2]);
	st32x32ICN_inst->In_4( initiator_to_node[3]);
	st32x32ICN_inst->In_5( initiator_to_node[4]);
	st32x32ICN_inst->In_6( initiator_to_node[5]);
	st32x32ICN_inst->In_7( initiator_to_node[6]);
	st32x32ICN_inst->In_8( initiator_to_node[7]);
	st32x32ICN_inst->In_9( initiator_to_node[8]);
	st32x32ICN_inst->In_10( initiator_to_node[9]);
	st32x32ICN_inst->In_11( initiator_to_node[10]);
	st32x32ICN_inst->In_12( initiator_to_node[11]);
	st32x32ICN_inst->In_13( initiator_to_node[12]);
	st32x32ICN_inst->In_14( initiator_to_node[13]);
	st32x32ICN_inst->In_15( initiator_to_node[14]);
	st32x32ICN_inst->In_16( initiator_to_node[15]); 
	st32x32ICN_inst->In_17( initiator_to_node[16]); 
	st32x32ICN_inst->In_18( initiator_to_node[17]); 
	st32x32ICN_inst->In_19( initiator_to_node[18]); 
	st32x32ICN_inst->In_20( initiator_to_node[19]); 
	st32x32ICN_inst->In_21( initiator_to_node[20]); 
	st32x32ICN_inst->In_22( initiator_to_node[21]); 
	st32x32ICN_inst->In_23( initiator_to_node[22]); 
	st32x32ICN_inst->In_24( initiator_to_node[23]); 
	st32x32ICN_inst->In_25( initiator_to_node[24]); 
	st32x32ICN_inst->In_26( initiator_to_node[25]); 
	st32x32ICN_inst->In_27( initiator_to_node[26]); 
	st32x32ICN_inst->In_28( initiator_to_node[27]); 
	st32x32ICN_inst->In_29( initiator_to_node[28]);
	st32x32ICN_inst->In_30( initiator_to_node[29]);
	st32x32ICN_inst->In_31( initiator_to_node[30]);
	st32x32ICN_inst->In_32( initiator_to_node[31]);
	
	st32x32ICN_inst->Out_1( node_to_target[0] );
	st32x32ICN_inst->Out_2( node_to_target[1] );
	st32x32ICN_inst->Out_3( node_to_target[2] );
	st32x32ICN_inst->Out_4( node_to_target[3] );
	st32x32ICN_inst->Out_5( node_to_target[4] );
	st32x32ICN_inst->Out_6( node_to_target[5] );
	st32x32ICN_inst->Out_7( node_to_target[6] );
	st32x32ICN_inst->Out_8( node_to_target[7] );
	st32x32ICN_inst->Out_9( node_to_target[8] );
	st32x32ICN_inst->Out_10( node_to_target[9] );
	st32x32ICN_inst->Out_11( node_to_target[10] );
	st32x32ICN_inst->Out_12( node_to_target[11] );
	st32x32ICN_inst->Out_13( node_to_target[12] );
	st32x32ICN_inst->Out_14( node_to_target[13] );
	st32x32ICN_inst->Out_15( node_to_target[14] );
	st32x32ICN_inst->Out_16( node_to_target[15] );
	st32x32ICN_inst->Out_17( node_to_target[16] );
	st32x32ICN_inst->Out_18( node_to_target[17] );
	st32x32ICN_inst->Out_19( node_to_target[18] );
	st32x32ICN_inst->Out_20( node_to_target[19] );
	st32x32ICN_inst->Out_21( node_to_target[20] );
	st32x32ICN_inst->Out_22( node_to_target[21] );
	st32x32ICN_inst->Out_23( node_to_target[22] );
	st32x32ICN_inst->Out_24( node_to_target[23] );
	st32x32ICN_inst->Out_25( node_to_target[24] );
        st32x32ICN_inst->Out_26( node_to_target[25] );
	st32x32ICN_inst->Out_27( node_to_target[26] );
	st32x32ICN_inst->Out_28( node_to_target[27] );
	st32x32ICN_inst->Out_29( node_to_target[28] );
	st32x32ICN_inst->Out_30( node_to_target[29] );
	st32x32ICN_inst->Out_31( node_to_target[30] );
	st32x32ICN_inst->Out_32( node_to_target[31] );

#ifndef NOSNOOP
        if (SNOOPING)
          st32x32ICN_inst->Node_1->SnoopingActive = true;
        else
          st32x32ICN_inst->Node_1->SnoopingActive = false;
        st32x32ICN_inst->Node_1->SnoopPort_d[0](SnoopSignal_d[0]);
        st32x32ICN_inst->Node_1->SnoopPort_r[0](SnoopSignal_r[0]);
#endif
 }
else
 {
  #ifdef stSHAREDst
  /* STBus 8x8 shared interconnect */
	shared_8x8ICN * st8x8ICN_inst;
	st8x8ICN_inst = new shared_8x8ICN ( "shared_8x8ICN" );
  #else
    #ifdef stFULLst	
    /* STBus 8x8 full interconnect */
	full_8x8ICN * st8x8ICN_inst;
	st8x8ICN_inst = new full_8x8ICN ( "full_8x8ICN" );
    #else
        #ifdef stPARTIALst	
	partial_8x8ICN * st8x8ICN_inst;
	st8x8ICN_inst = new partial_8x8ICN ( "partial_8x8ICN" );
       #else
       printf("You should define if the connection is shared or full\n");
      #endif
    #endif 
  #endif
  
        if (FREQSCALING || FREQSCALINGDEVICE)
          st8x8ICN_inst->ClockGen_1( interconnect_clock[0] ); // FIXME [0]
        else
          st8x8ICN_inst->ClockGen_1( ClockGen_1 );
	st8x8ICN_inst->ResetGen_1( ResetGen_1 );
	st8x8ICN_inst->In_1( initiator_to_node[0] );
	st8x8ICN_inst->In_2( initiator_to_node[1] );
	st8x8ICN_inst->In_3( initiator_to_node[2] );
	st8x8ICN_inst->In_4( initiator_to_node[3] );
	st8x8ICN_inst->In_5( initiator_to_node[4] );
	st8x8ICN_inst->In_6( initiator_to_node[5] );
	st8x8ICN_inst->In_7( initiator_to_node[6] );
	st8x8ICN_inst->In_8( initiator_to_node[7] );
	st8x8ICN_inst->Out_1( node_to_target[0] );
	st8x8ICN_inst->Out_2( node_to_target[1] );
	st8x8ICN_inst->Out_3( node_to_target[2] );
	st8x8ICN_inst->Out_4( node_to_target[3] );
	st8x8ICN_inst->Out_5( node_to_target[4] );
	st8x8ICN_inst->Out_6( node_to_target[5] );
	st8x8ICN_inst->Out_7( node_to_target[6] );
	st8x8ICN_inst->Out_8( node_to_target[7] );

#ifndef NOSNOOP
        if (SNOOPING) 
          st8x8ICN_inst->Node_1->SnoopingActive = true;
        else
          st8x8ICN_inst->Node_1->SnoopingActive = false;
        st8x8ICN_inst->Node_1->SnoopPort_d[0](SnoopSignal_d[0]);
        st8x8ICN_inst->Node_1->SnoopPort_r[0](SnoopSignal_r[0]);
#endif
 }
 
#ifdef PRINTDEBUG
  cout << "Platform instantiation completed!" << endl;
#endif

}


//This function is invoked at the end of the systemc simulation
//It's needed to delete the LX wrapper in oder to allow the ISS to dump statistics
void destroySTBusPlatform(){      

#ifdef LXBUILD

if(CURRENT_ISS == LX)
{
   for(int i=0;i<N_CORES;i++) delete lx[i];
   
   switch(N_MASTERS){
   	case 16: delete init15_lx;
   	case 15: delete init14_lx;	
	case 14: delete init13_lx;
	case 13: delete init12_lx;
	case 12: delete init11_lx;
	case 11: delete init10_lx;
	case 10: delete init9_lx;
   	case 9: delete init8_lx;
   	case 8: delete init7_lx;	
	case 7: delete init6_lx;
	case 6: delete init5_lx;
	case 5: delete init4_lx;
	case 4: delete init3_lx;
	case 3: delete init2_lx;
	case 2: delete init1_lx;
	case 1: delete init0_lx;
	default: break;
   }
}   
#endif

}

