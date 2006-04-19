///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_sig_builder.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Manages AMBA AHB (signal) platform instantiation
//
///////////////////////////////////////////////////////////////////////////////

#include <systemc.h>
#include "globals.h"
#include "amba_sig_signal.h"
#include "arbiter.h"
#include "multi1.h"
#include "multi2.h"
#include "multi3.h"
#include "decoder.h"
#include "amba_sig_ext_target.h"
#include "smartmem.h"
#include "smartmem_signal.h"
#include "slaving.h"
#include "delayer_hsel.h"
#include "delayer_hwdata.h"
#include "mem_class.h"
#include "scratch_mem.h"
#include "amba_sig_dma_scratch.h"
#include "stats.h"
#include "slave2.h"
#include "slave3.h"
#include "clock_tree.h"
#include "reset.h"
#include "freq_register.h"
#include "dual_clock_pinout_adapter.h"

#ifdef WITHOUT_OCP
  #include "master.h"
  #ifdef SWARMBUILD
    #include "wrapper.h"
  #endif
  #ifdef SIMITARMBUILD
    #include "simitarm_wrapper.h"
  #endif
  #ifdef POWERPCBUILD
    #include "PowerPC.h"
    #include "decode.h"
    #include "ppc_syscall.h"    
  #endif
#else
  #include "swarm_ocp_master_wrapper.h"
  #include "ocp_to_amba_sig_slave.h"
#endif

//#define TRACEPPC
//PowerPC **ppc;
void exit_proc(int code) //FIXME
{	
	cerr << "Program exit with code " << code << endl;
	statobject->stopMeasuring(0);
	statobject->quit(0);
}



void buildAMBAAHBSigPlatform(int new_argc, char *new_argv[], char *new_envp[])
{
  int i,j,k;
  char buffer[20];
  mast = new sc_signal<request> [N_MASTERS];
  address = new sc_signal<sc_uint<32> > [N_MASTERS];
  hwdata = new sc_signal<sc_uint<32> > [N_MASTERS];
  hreq = new sc_signal<bool> [N_MASTERS];
  hsel = new sc_signal<bool> [N_SLAVES];
  delayed_hsel = new sc_signal<bool> [N_SLAVES];
  hgrant = new sc_signal<bool> [N_MASTERS];
  pinoutmast = new sc_signal<PINOUT> [N_MASTERS];
  requestmast = new sc_signal<bool> [N_MASTERS];
  readymast = new sc_signal<bool> [N_MASTERS];
  pinoutmast_interconnect = new sc_signal<PINOUT> [N_MASTERS];
  requestmast_interconnect = new sc_signal<bool> [N_MASTERS];
  readymast_interconnect = new sc_signal<bool> [N_MASTERS];
  //N_INTERRUPT*N_CORES ... N_SMARTMEM*N_CORES
  extint = new sc_signal<bool> [NUMBER_OF_EXT*N_CORES];
  ready = new sc_signal<bool> [N_SLAVES];
  readdata = new sc_signal<sc_uint<32> > [N_SLAVES];
  
  // Clock signals and dividers
  init_clock_amba = new sc_signal< bool > [N_MASTERS];
  interconnect_clock_amba = new sc_signal< bool > [N_BUSES];
  init_div_amba = new sc_signal< sc_uint<FREQSCALING_BITS> > [N_MASTERS];
  interconnect_div_amba = new sc_signal< sc_uint<FREQSCALING_BITS> > [N_BUSES];
      
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
  
  // Reset device
  resetter *reset_unit;
  reset_unit = new resetter("reset_device", (unsigned long int)1);
  reset_unit->clock(ClockGen_1);
  reset_unit->reset_out_true_high(ResetGen_1);
  reset_unit->reset_out_true_low(ResetGen_low_1);
  
  // Frequency divider
  if (FREQSCALING || FREQSCALINGDEVICE)
  {
    clock_tree *ctree;
    ctree = new clock_tree("clock_tree_gen", N_FREQ_DEVICE + N_BUSES);
    ctree->Clock_in(ClockGen_1);
    ctree->Reset(ResetGen_1);
    for (i = 0; i < N_FREQ_DEVICE; i ++)
    {
      ctree->Div[i](init_div_amba[i]);
      ctree->Clock_out[i](init_clock_amba[i]);
    }
    for (i = 0; i < N_BUSES; i ++)
    {
      ctree->Div[N_FREQ_DEVICE + i](interconnect_div_amba[i]);
      ctree->Clock_out[N_FREQ_DEVICE + i](interconnect_clock_amba[i]);
    }
    
    // Programmable register for the frequency divider
    feeder *p_register;
    p_register = new feeder("p_register", N_FREQ_DEVICE + N_BUSES);
    p_register->clock(ClockGen_1);
 
    for (i = 0; i < N_FREQ_DEVICE; i ++)
      p_register->feed[i](init_div_amba[i]);
    for (i = 0; i < N_BUSES; i ++)
      p_register->feed[N_FREQ_DEVICE + i](interconnect_div_amba[i]);
     
    // Frequency controller interface
    dual_clock_pinout_adapter *fifo[N_FREQ_DEVICE]; 
    for (i = 0; i < N_FREQ_DEVICE; i ++)
    {
      if (M_DIVIDER[i] != I_DIVIDER[MASTER_CONFIG[i].binding] || FREQSCALINGDEVICE)
      {
        sprintf(buffer, "dual_clock_fifo_%d", i);
        fifo[i] = new dual_clock_pinout_adapter(buffer);
      
        fifo[i]->clk_s(init_clock_amba[i]);
        fifo[i]->rst_s(ResetGen_1);
        fifo[i]->request_from_wrapper(requestmast[i]);
        fifo[i]->ready_to_wrapper(readymast[i]);
        fifo[i]->pinout_s(pinoutmast[i]);
        
        fifo[i]->clk_m(interconnect_clock_amba[MASTER_CONFIG[i].binding]);
        fifo[i]->rst_m(ResetGen_1);
        fifo[i]->request_to_master(requestmast_interconnect[i]);
        fifo[i]->ready_from_master(readymast_interconnect[i]);
        fifo[i]->pinout_m(pinoutmast_interconnect[i]);
      }
    }
  }
  
  // AMBA memory slaves. Their instantiation must be done
  // before that of scratchpad memories, because if SPCHECK
  // is active, scratchpad memories must fetch initialization
  // data from external memories
  uint16_t privatestartid=addresser->PrivateStartID();
  AMBA_SIG_EXT_MEMORY_TARGET *target_mem[N_PRIVATE];
  for (i=0; i<N_PRIVATE; i++)
  {
    sprintf(buffer, "s%d", i);
    target_mem[i] = new AMBA_SIG_EXT_MEMORY_TARGET(buffer,privatestartid + i,
     addresser->ReturnSlavePhysicalAddress(i),
     addresser->ReturnPrivateSize(),
     MEM_IN_WS);
    if (FREQSCALING || FREQSCALINGDEVICE)
      target_mem[i]->clock(interconnect_clock_amba[SLAVE_CONFIG[privatestartid + i].binding]);
    else
      target_mem[i]->clock(ClockGen_1);
    target_mem[i]->haddr(haddr);
    target_mem[i]->delayed_hwdataout(delayed_hwdataout);
    target_mem[i]->ready0(ready[i]);
    target_mem[i]->hready(hready);
    target_mem[i]->ctrl_signals(ctrl_signals);
    target_mem[i]->hsel0(hsel[i]);
    target_mem[i]->readdata0(readdata[i]);
  }
  
  uint16_t sharedstartid=addresser->SharedStartID();
  AMBA_SIG_EXT_SHARED_TARGET *share_slave[N_SHARED];
  for (i=0; i< N_SHARED ; i++)
  {
    sprintf(buffer, "s%d",sharedstartid+i);
    share_slave[i] = new AMBA_SIG_EXT_SHARED_TARGET(buffer,sharedstartid + i,
     addresser->ReturnSlavePhysicalAddress(sharedstartid+i),
     addresser->ReturnSharedSize(sharedstartid+i),
     MEM_IN_WS);
    if (FREQSCALING || FREQSCALINGDEVICE)
      share_slave[i]->clock(interconnect_clock_amba[SLAVE_CONFIG[sharedstartid + i].binding]);
    else
      share_slave[i]->clock(ClockGen_1);
    share_slave[i]->haddr(haddr);
    share_slave[i]->delayed_hwdataout(delayed_hwdataout);
    share_slave[i]->ready0(ready[sharedstartid+i]);
    share_slave[i]->hready(hready);
    share_slave[i]->ctrl_signals(ctrl_signals);
    share_slave[i]->hsel0(hsel[sharedstartid+i]);
    share_slave[i]->readdata0(readdata[sharedstartid+i]);
  }
  
  uint16_t semaphorestartid=addresser->SemaphoreStartID();
  AMBA_SIG_EXT_SEMAPHORE_TARGET *sem_slave[N_SEMAPHORE];
  for (i=0; i< N_SEMAPHORE ; i++)
  {
    sprintf(buffer, "s%d",semaphorestartid+i);
    sem_slave[i] = new AMBA_SIG_EXT_SEMAPHORE_TARGET(buffer, semaphorestartid+i,
     addresser->ReturnSlavePhysicalAddress(semaphorestartid+i),
     addresser->ReturnSemaphoreSize(semaphorestartid+i),
     MEM_IN_WS);
    if (FREQSCALING || FREQSCALINGDEVICE)
      sem_slave[i]->clock(interconnect_clock_amba[SLAVE_CONFIG[semaphorestartid + i].binding]);
    else
      sem_slave[i]->clock(ClockGen_1);
    sem_slave[i]->haddr(haddr);
    sem_slave[i]->delayed_hwdataout(delayed_hwdataout);
    sem_slave[i]->ready0(ready[semaphorestartid+i]);
    sem_slave[i]->hready(hready);
    sem_slave[i]->ctrl_signals(ctrl_signals);
    sem_slave[i]->hsel0(hsel[semaphorestartid+i]);
    sem_slave[i]->readdata0(readdata[semaphorestartid+i]);
  }
  
  uint16_t interruptstartid=addresser->InterruptStartID();
  // AMBA interrupt slaves
  AMBA_SIG_EXT_INTERRUPT_TARGET *s1[N_INTERRUPT];
  for (i=0; i<N_INTERRUPT; i++)
  {
    sprintf(buffer, "s%d", interruptstartid+i);
    s1[i]= new AMBA_SIG_EXT_INTERRUPT_TARGET(buffer,interruptstartid+i,
     addresser->ReturnSlavePhysicalAddress(i+interruptstartid),
     addresser->ReturnInterruptSize(),
     INT_WS);
    if (FREQSCALING || FREQSCALINGDEVICE)
      s1[i]->clock(interconnect_clock_amba[SLAVE_CONFIG[interruptstartid + i].binding]);
    else
      s1[i]->clock(ClockGen_1);
    s1[i]->haddr(haddr);
    s1[i]->delayed_hwdataout(delayed_hwdataout);
    s1[i]->ready0(ready[interruptstartid + i]);
    s1[i]->hready(hready);
    s1[i]->ctrl_signals(ctrl_signals);
    s1[i]->hsel0(hsel[interruptstartid + i]);
    s1[i]->readdata0(readdata[interruptstartid + i]);
    for (j=0; j<N_CORES; j++)
      s1[i]->extint[j](extint[(j*NUMBER_OF_EXT)+i]);
  }

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
     if(CORESLAVE)
       {
        sprintf(buffer, "Scratch-semaphore%d",i);
        queue[i] = new Scratch_queue_mem(buffer,i,addresser->ReturnScratchSize(),addresser->ReturnQueuePhysicalAddress(i));
        queue[i]->sendint(extint[(i*NUMBER_OF_EXT)+N_INTERRUPT]);
       }     
     else
       queue[i] = NULL;
   
   //AMBA core-associated slaves. These slaves receive a scratchpad pointer as a parameter!
   if(CORESLAVE) 
   {       
    uint16_t core_slavestartid=addresser->CoreSlaveStartID();
 
    slave2 *s2[N_CORESLAVE];
    for (i=0; i<N_CORESLAVE; i++)
    {
      sprintf(buffer, "s%d", core_slavestartid + i);
      s2[i] = new slave2(buffer,core_slavestartid+i,i,
       addresser->ReturnSlavePhysicalAddress(core_slavestartid+i),
       addresser->ReturnCoreSlaveSize(),
       1,datascratch[i],queue[i]); 
      // the latency of the slave should be SCRATCH_WS but now the slave is not working with
      //latency 0
      if (FREQSCALING || FREQSCALINGDEVICE)
        s2[i]->clock(interconnect_clock_amba[SLAVE_CONFIG[core_slavestartid + i].binding]);
      else
        s2[i]->clock(ClockGen_1);
      s2[i]->haddr(haddr);
      s2[i]->delayed_hwdataout(delayed_hwdataout);
      s2[i]->ready0(ready[core_slavestartid + i]);
      s2[i]->hready(hready);
      s2[i]->ctrl_signals(ctrl_signals);
      s2[i]->hsel0(hsel[core_slavestartid + i]);
      s2[i]->readdata0(readdata[core_slavestartid + i]);
    }
   }
   
#ifdef WITHOUT_OCP
  // System processors
#ifdef SIMITARMBUILD
    Wrapper *soc[N_CORES];
#elif defined POWERPCBUILD
    PowerPC *soc[N_CORES];
#else
    armsystem *soc[N_CORES];
#endif

  for (i=0; i<N_CORES; i++)
  {
#ifdef SIMITARMBUILD
    {
      	sprintf(buffer, "StrongArm%d", i);
      	uint32_t DCacheAssoc, ICacheAssoc;
	uint32_t ITLBBlocks = 32;
	uint32_t ITLBAssoc = 32;
	uint32_t DTLBBlocks = 32;
	uint32_t DTLBAssoc = 32;
	uint32_t WBEntries = 8;
      	if (ICACHETYPE == DIRECT)
      		ICacheAssoc = 1;
	else if (ICACHETYPE == SETASSOC)
			ICacheAssoc = ICACHEWAYS;
		else
			ICacheAssoc = ICACHESIZE/ICacheLineSize;
	if (DCACHETYPE == DIRECT)
      		DCacheAssoc = 1;
	else if (DCACHETYPE == SETASSOC)
			DCacheAssoc = DCACHEWAYS;
		else
			DCacheAssoc = DCACHESIZE/DCacheLineSize;
      	soc[i] = new Wrapper(buffer,(unsigned int)i,(uint8_t *)addresser->pMemoryDebug[i],ICACHESIZE/ICacheLineSize,ICacheAssoc,
      			ITLBBlocks,ITLBAssoc,DCACHESIZE/DCacheLineSize,DCacheAssoc,DTLBBlocks,DTLBAssoc,WBEntries);
      	soc[i]->LoadProgram(new_argv[0], new_argc, new_argv, new_envp);
	}
#elif defined POWERPCBUILD
         {
           sprintf(buffer, "PowerPC%d", i);
           soc[i] = new PowerPC(buffer, (uint8_t *)addresser->pMemoryDebug[i]);
           ppc_init_decode();
           
	   syscall_cb_t syscall_cb;
           syscall_cb.write_gpr = PowerPC::write_gpr;
           syscall_cb.read_gpr = PowerPC::read_gpr;
           syscall_cb.reset_cr0so = PowerPC::reset_cr0so;
           syscall_cb.set_cr0so = PowerPC::set_cr0so;
           syscall_cb.mem_read_byte = PowerPC::mem_read_byte;
           syscall_cb.mem_write_byte = PowerPC::mem_write_byte;
           syscall_cb.mem_read_half_word = PowerPC::mem_read_half_word;
           syscall_cb.mem_write_half_word = PowerPC::mem_write_half_word;
           syscall_cb.mem_read_word = PowerPC::mem_read_word;
           syscall_cb.mem_write_word = PowerPC::mem_write_word;
           syscall_cb.mem_read_dword = PowerPC::mem_read_dword;
           syscall_cb.mem_write_dword = PowerPC::mem_write_dword;
           syscall_cb.mem_set = PowerPC::mem_set;
           syscall_cb.mem_read = PowerPC::mem_read;
           syscall_cb.mem_write = PowerPC::mem_write;
           syscall_cb.instance = NULL;
           syscall_init(&syscall_cb, /* verbose */true, /*emulate */ false, soc[i], exit_proc);

           soc[i]->BusInterfaceInit();
           soc[i]->LoadProgram(new_argv[0], new_argc, new_argv, new_envp);
         }
#else
         {
           sprintf(buffer, "Arm_System%d", i);
           soc[i] = new armsystem(buffer, (unsigned int)i, datascratch[i], queue[i]);
         }
#endif
    if (FREQSCALING || FREQSCALINGDEVICE)
      soc[i]->clock(init_clock_amba[i]);
    else
      soc[i]->clock(ClockGen_1);
    soc[i]->pinout_ft_master[0](pinoutmast[i]);
    soc[i]->ready_from_master[0](readymast[i]);
    soc[i]->request_to_master[0](requestmast[i]);
    for (j=0; j<NUMBER_OF_EXT; j++)
      soc[i]->extint[j](extint[i*NUMBER_OF_EXT + j]); 
  }  
               
  // AMBA bus masters
  amba_sig_master *mst[N_MASTERS];
  for (i=0; i<N_MASTERS; i++)
  {
    sprintf(buffer, "mst%d", i);
    mst[i] = new amba_sig_master(buffer);
    if (FREQSCALING || FREQSCALINGDEVICE)
      mst[i]->clock(interconnect_clock_amba[MASTER_CONFIG[i].binding]);
    else
      mst[i]->clock(ClockGen_1);
    if ((FREQSCALING && M_DIVIDER[i] != I_DIVIDER[MASTER_CONFIG[i].binding]) || FREQSCALINGDEVICE)
    {
      mst[i]->pinout(pinoutmast_interconnect[i]);
      mst[i]->ready_to_wrapper(readymast_interconnect[i]);
      mst[i]->request_from_wrapper(requestmast_interconnect[i]);
    }
    else
    {
      mst[i]->pinout(pinoutmast[i]);
      mst[i]->ready_to_wrapper(readymast[i]);
      mst[i]->request_from_wrapper(requestmast[i]);
    }
    // connect the master to the bus
    mst[i]->mast(mast[i]);
    mst[i]->address(address[i]);
    mst[i]->hwdata(hwdata[i]);
    mst[i]->hrdata(hrdata);
    mst[i]->hgrant(hgrant[i]);
    mst[i]->hready(hready);
    mst[i]->hreq(hreq[i]);
  }
#else
  // OCP signals
  MCmd = new sc_signal< sc_uint<MCMDWD> > [N_CORES + N_SLAVES];
  MBurst = new sc_signal< sc_uint<MBURSTWD> > [N_CORES + N_SLAVES];
  MAddr = new sc_signal< sc_uint<MADDRWD> > [N_CORES + N_SLAVES];
  MAddrSpace = new sc_signal< sc_uint<MADDRSPACEWD> > [N_CORES + N_SLAVES];
  MData = new sc_signal< sc_uint<MDATAWD> > [N_CORES + N_SLAVES];
  MByteEn = new sc_signal< sc_uint<MBYTEENWD> > [N_CORES + N_SLAVES];
  MDataValid = new sc_signal< bool > [N_CORES + N_SLAVES];
  SCmdAccept = new sc_signal< bool > [N_CORES + N_SLAVES];
  SDataAccept = new sc_signal< bool > [N_CORES + N_SLAVES];
  SResp = new sc_signal< sc_uint<SRESPWD> > [N_CORES + N_SLAVES];
  SData = new sc_signal< sc_uint<MDATAWD> > [N_CORES + N_SLAVES];
  MRespAccept = new sc_signal< bool > [N_CORES + N_SLAVES];

  // ARM processors
  ocpswarm *soc[N_CORES];
  for (i=0; i<N_CORES; i++)
  {
    sprintf(buffer, "Arm_System%d", i);
    soc[i] = new ocpswarm(buffer, (unsigned int)i, datascratch[i], queue[i]);
    if (FREQSCALING || FREQSCALINGDEVICE)
      soc[i]->clock(init_clock_amba[i]);
    else
      soc[i]->clock(ClockGen_1);
    soc[i]->MCmd[0](MCmd[i]);
    soc[i]->MBurst[0](MBurst[i]);
    soc[i]->MAddr[0](MAddr[i]);
    soc[i]->MAddrSpace[0](MAddrSpace[i]);
    soc[i]->MData[0](MData[i]);
    soc[i]->MByteEn[0](MByteEn[i]);
    soc[i]->MDataValid[0](MDataValid[i]);
    soc[i]->SCmdAccept[0](SCmdAccept[i]);
    soc[i]->SDataAccept[0](SDataAccept[i]);
    soc[i]->SResp[0](SResp[i]);
    soc[i]->SData[0](SData[i]);
    soc[i]->MRespAccept[0](MRespAccept[i]);
    for (j=0; j<NUMBER_OF_EXT; j++)
      soc[i]->extint[j](extint[i*NUMBER_OF_EXT + j]);
  }
               
  // AMBA bus masters
  ocpambasigslave *mst[N_MASTERS];
  for (i=0; i<N_MASTERS; i++)
  {
    sprintf(buffer, "mst%d", i);
    mst[i] = new ocpambasigslave(buffer);
    if (FREQSCALING || FREQSCALINGDEVICE)
      mst[i]->clock(interconnect_clock_amba[MASTER_CONFIG[i].binding]);
    else
      mst[i]->clock(ClockGen_1);
    mst[i]->MCmd(MCmd[i]);
    mst[i]->MBurst(MBurst[i]);
    mst[i]->MAddr(MAddr[i]);
    mst[i]->MAddrSpace(MAddrSpace[i]);
    mst[i]->MData(MData[i]);
    mst[i]->MByteEn(MByteEn[i]);
    mst[i]->MDataValid(MDataValid[i]);
    mst[i]->SCmdAccept(SCmdAccept[i]);
    mst[i]->SDataAccept(SDataAccept[i]);
    mst[i]->SResp(SResp[i]);
    mst[i]->SData(SData[i]);
    mst[i]->MRespAccept(MRespAccept[i]);
    // connect the master to the bus
    mst[i]->mast(mast[i]);
    mst[i]->address(address[i]);
    mst[i]->hwdata(hwdata[i]);
    mst[i]->hrdata(hrdata);
    mst[i]->hgrant(hgrant[i]);
    mst[i]->hready(hready);
    mst[i]->hreq(hreq[i]);
  }
#endif

  // DMA support
  if (DMA)
  {
    Amba_sig_Dma_control_scratch *dmacont[N_CORES];
    Amba_sig_Dma_transfer_scratch *transf[N_CORES];

    for (i=0; i<N_CORES; i++)
    {
      //instance a DMA controller
      sprintf(buffer,"dma%d",i);
      dmacont[i] = new
      Amba_sig_Dma_control_scratch(buffer, i,INTERNAL_MAX_OBJ,1,addresser->ReturnDMAPhysicalAddress());
      if (FREQSCALING || FREQSCALINGDEVICE)
        dmacont[i]->clock(init_clock_amba[i]);
      else
        dmacont[i]->clock(ClockGen_1);
      
      //instance a DMA transfer module
      sprintf(buffer,"tra%d", i);
      transf[i] = new Amba_sig_Dma_transfer_scratch(buffer, i,INTERNAL_DIM_BURST,INTERNAL_MAX_OBJ,1,datascratch[i]);
      if (FREQSCALING || FREQSCALINGDEVICE)
        transf[i]->clock(init_clock_amba[i]);
      else
        transf[i]->clock(ClockGen_1);

      //connect this wrapper to the DMA controller
#ifdef WITHOUT_OCP
      soc[i]->pinout_ft_master[1](pinoutwrappertodma[i]);
      soc[i]->ready_from_master[1](readywrappertodma[i]);
      soc[i]->request_to_master[1](requestwrappertodma[i]);
#endif
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


  // Marchal
  // create the smart-memory memories
  Scratch_mem *smartscratch[N_SMARTMEM];
  slave3* s3[N_SMARTMEM];
  
  uint16_t slavestartid=addresser->Smartmem_slave_StartID();
  uint16_t masterstartid=addresser->Smartmem_master_StartID();
  for (i=0; i<N_SMARTMEM; i++)
    {
      if (SMARTMEM)
	{
	 smartscratch[i] = 
	  new  Scratch_mem("Smart_mem",i,SMART_MEM_SIZE[i],
	  	(addresser->ReturnSlavePhysicalAddress(slavestartid+i))+SMARTMEM_DMA_SIZE);
        }
        else
	 smartscratch[i] = NULL;
    }

 
  // create number of smart-memory slaves
  if(SMARTMEM)
  {
    Amba_sig_Dma_control_smartmem* smartdmacont[N_SMARTMEM];
    Amba_sig_Dma_transfer_smartmem* smarttransf[N_SMARTMEM];
    
    for (i=0; i<N_SMARTMEM; i++) 
      {
	
	// create the smart mem slave
	sprintf(buffer, "s%d",slavestartid+i );
	s3[i] =  new slave3(buffer,slavestartid+i,masterstartid+i,
         addresser->ReturnSlavePhysicalAddress(slavestartid+i),
         SMART_MEM_SIZE[i],SMART_MEM_IN_WS[i],smartscratch[i]);
        if (FREQSCALING || FREQSCALINGDEVICE)
          s3[i]->clock(interconnect_clock_amba[SLAVE_CONFIG[slavestartid + i].binding]);
        else
          s3[i]->clock(ClockGen_1);
	s3[i]->haddr(haddr);
	s3[i]->delayed_hwdataout(delayed_hwdataout);
	s3[i]->ready0(ready[slavestartid + i]);
	s3[i]->hready(hready);
	s3[i]->ctrl_signals(ctrl_signals);
	s3[i]->hsel0(hsel[slavestartid + i ]);
	s3[i]->readdata0(readdata[slavestartid + i]);

	// create the SMARTMEM DMA controller is important to get the correct id to the DMA for
	// relate it with the correct master
	sprintf(buffer,"dma%d",DMA*N_CORES+i);
	smartdmacont[i] =  new Amba_sig_Dma_control_smartmem(buffer,DMA*N_CORES+i,SMARTMEM_MAX_OBJ,N_CORES,
	                      	addresser->ReturnSlavePhysicalAddress(slavestartid+i));
        if (FREQSCALING || FREQSCALINGDEVICE)
          smartdmacont[i]->clock(interconnect_clock_amba[SLAVE_CONFIG[slavestartid + i].binding]);
        else
          smartdmacont[i]->clock(ClockGen_1);

	// create the SMARTMEM DMA transfer
	sprintf(buffer,"tra%d",DMA*N_CORES+i);
	smarttransf[i] = new Amba_sig_Dma_transfer_smartmem(buffer,DMA*N_CORES+i,SMARTMEM_DIM_BURST,SMARTMEM_MAX_OBJ,
				N_CORES,smartscratch[i]);
       if (FREQSCALING || FREQSCALINGDEVICE)
         smarttransf[i]->clock(interconnect_clock_amba[SLAVE_CONFIG[slavestartid + i].binding]);
       else
         smarttransf[i]->clock(ClockGen_1);

	//connect the SMARTMEM_SLAVE to the DMA controller
	s3[i]->pinoutdma(smartmem_pinoutslavetodma[i]);
	s3[i]->readydma(smartmem_readyslavetodma[i]);
	s3[i]->requestdma(smartmem_requestslavetodma[i]);
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
	    smarttransf[i]->extint[j*SMARTMEM_MAX_OBJ+k]
		(extint[(NUMBER_OF_EXT*j)+k+(SMARTMEM_MAX_OBJ*i)+N_INTERRUPT+CORESLAVE]);
	   }
      }
  }



  // AMBA arbiter
  arbiter *arb;
  arb = new arbiter("arb");
  if (FREQSCALING || FREQSCALINGDEVICE)
    arb->clock(interconnect_clock_amba[0]); // FIXME [0]
  else
    arb->clock(ClockGen_1);
  arb->hready(hready);
  arb->ctrl_signals(ctrl_signals);
  arb->hmaster(hmaster);
  for(i=0; i<N_MASTERS; i++)
  {
    arb->hreq[i](hreq[i]);
    arb->hgrant[i](hgrant[i]);
  }

  // AMBA signal multiplexers
  // multi1: control and address buses
  multi1 *m1;
  m1 = new multi1("m1");
  if (FREQSCALING || FREQSCALINGDEVICE)
    m1->clock(interconnect_clock_amba[0]); // FIXME [0]
  else
    m1->clock(ClockGen_1);
  m1->hmaster(hmaster);
  m1->haddr(haddr);
  m1->ctrl_signals(ctrl_signals);
  for(i=0; i<N_MASTERS; i++)
  {
    m1->address[i](address[i]);
    m1->mast[i](mast[i]);
  }
  // multi2: data write bus
  multi2 *m2;
  m2 = new multi2("m2");
  if (FREQSCALING || FREQSCALINGDEVICE)
    m2->clock(interconnect_clock_amba[0]); // FIXME [0]
  else
    m2->clock(ClockGen_1);
  for(i=0; i<N_MASTERS; i++)
    m2->hwdata[i](hwdata[i]);
  m2->hwdataout(hwdataout);
  m2->hmaster(hmaster);
  // multi3: data read bus
  multi3 *m3;
  m3 = new multi3("m3");
  for(i=0; i<N_SLAVES; i++)
  {
    m3->readdata[i](readdata[i]);
    m3->delayed_hsel[i](delayed_hsel[i]);
  }
  m3->hrdata(hrdata);

  // AMBA address decoder
  decoder *dec;
  dec = new decoder("dec");
  if (FREQSCALING || FREQSCALINGDEVICE)
    dec->clock(interconnect_clock_amba[0]); // FIXME [0]
  else
    dec->clock(ClockGen_1);
  dec->haddr(haddr);
  for(i=0; i<N_SLAVES; i++)
    dec->hsel[i](hsel[i]);

  // AMBA signal delayer (delays hsel signals)
  delayer_hsel *d1;
  d1 = new delayer_hsel("d1");
  if (FREQSCALING || FREQSCALINGDEVICE)
    d1->clock(interconnect_clock_amba[0]); // FIXME [0]
  else
    d1->clock(ClockGen_1);
  d1->hready(hready);
  for(i=0; i<N_SLAVES; i++)
  {
    d1->hsel[i](hsel[i]);
    d1->delayed_hsel[i](delayed_hsel[i]);
  }
               
  // AMBA signal delayer (delays hwdata bus)
  delayer_hwdata *d2;
  d2 = new delayer_hwdata("d2");
  if (FREQSCALING || FREQSCALINGDEVICE)
    d2->clock(interconnect_clock_amba[0]); // FIXME [0]
  else
    d2->clock(ClockGen_1);
  d2->hready(hready);
  d2->hwdataout(hwdataout);
  d2->delayed_hwdataout(delayed_hwdataout);

  // AMBA logical OR module (ORs ready signals)
  slaving *tr;
  tr = new slaving("tr");
  if (FREQSCALING || FREQSCALINGDEVICE)
    tr->clock(interconnect_clock_amba[0]); // FIXME [0]
  else
    tr->clock(ClockGen_1);
  for(i=0; i<N_SLAVES; i++)
  {
    tr->ready[i](ready[i]);
    tr->hsel[i](hsel[i]);
  }
  tr->hready(hready);
}
