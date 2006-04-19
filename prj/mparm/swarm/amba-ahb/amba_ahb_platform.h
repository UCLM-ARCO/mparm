///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_ahb_platform.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Describes an AMBA AHB platform.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __AMBA_AHB_PLATFORM_H__
#define __AMBA_AHB_PLATFORM_H__

#include <systemc.h>
#include "globals.h"
#include "amba_ahb_interconnect.h"
#include "amba_ahb_common.h"
#include "amba_ahb_master.h"
#include "amba_ahb_ocp_master.h"
#include "amba_ahb_simple_slaves.h"
#include "amba_ahb_smart_mem_slave.h"
#include "amba_ahb_core_slave.h"
#include "amba_ahb_prog_freq_reg.h"
#include "amba_ahb_mlicm.h"
#include "amba_ahb_bridge.h"
#include "core_signal.h"
#include "smartmem_signal.h"
#include "mem_class.h"
#include "scratch_mem.h"
#include "amba_ahb_dma_scratch.h"
#include "reset.h"
#include "clock_tree.h"
#ifdef SWARMBUILD
  #include "wrapper.h"
  #include "swarm_ocp_master_wrapper.h"
#endif
#ifdef LXBUILD
  #include "amba_ahb_initiator_lx.h"
  #include "ast_iss_wrapper.h"
  using namespace sc_dt;

#endif
#ifdef SIMITARMBUILD
  #include "simitarm_wrapper.h"
#endif
#ifdef POWERPCBUILD
  #include "PowerPC.h"
  #include "decode.h"
  #include "ppc_syscall.h"
#endif
#ifdef TGBUILD
  #include "TGMaster.h"
#endif
#include "dual_clock_pinout_adapter.h"
#include "freq_register.h"

#if 0
#include "smartmem.h"
#include "stats.h"

//#define TRACEPPC
//PowerPC **ppc;
void exit_proc(int code) //FIXME
{	
	cerr << "Program exit with code " << code << endl;
	statobject->stopMeasuring(0);
	statobject->quit(0);
}


#endif

///////////////////////////////////////////////////////////////////////////////
// amba_ahb_platform - This class instantiates a whole AMBA AHB system.
//                     nm: platform name
//                     tf: VCD trace file (used if tracing is enabled)
//                     tracing: enables/disables VCD tracing
//                     new_argc, new_argv, new_envp: environment for binary
//                             to be loaded by SimIt-ARM/PowerPC ISSs
SC_MODULE(amba_ahb_platform)
{
  private:
    
#ifdef SIMITARMBUILD
    Wrapper **soc;
#elif defined POWERPCBUILD
    PowerPC **soc;
#elif defined SWARMBUILD
    ocpswarm **ocp_soc;
    armsystem **soc;
#endif
#ifdef TGBUILD
    TGMaster **TGen;
#endif
#ifdef LXBUILD
    ASTISSWrapper **lx;
   
  #ifdef WITH_INI_DEBUG
    #define INI_DEBUG true
  #else
    #define INI_DEBUG false
  #endif

    AHBBus_initiator_lx< uint, uint, uchar, 32, 16, 0, true, INI_DEBUG >  *init0_lx;
    AHBBus_initiator_lx< uint, uint, uchar, 32, 16, 1, true, INI_DEBUG >  *init1_lx;
    AHBBus_initiator_lx< uint, uint, uchar, 32, 16, 2, true, INI_DEBUG >  *init2_lx;
    AHBBus_initiator_lx< uint, uint, uchar, 32, 16, 3, true, INI_DEBUG >  *init3_lx;
    AHBBus_initiator_lx< uint, uint, uchar, 32, 16, 4, true, INI_DEBUG >  *init4_lx;
    AHBBus_initiator_lx< uint, uint, uchar, 32, 16, 5, true, INI_DEBUG >  *init5_lx;
    AHBBus_initiator_lx< uint, uint, uchar, 32, 16, 6, true, INI_DEBUG >  *init6_lx;
    AHBBus_initiator_lx< uint, uint, uchar, 32, 16, 7, true, INI_DEBUG >  *init7_lx;
    AHBBus_initiator_lx< uint, uint, uchar, 32, 16, 8, true, INI_DEBUG >  *init8_lx;
    AHBBus_initiator_lx< uint, uint, uchar, 32, 16, 9, true, INI_DEBUG >  *init9_lx;
    AHBBus_initiator_lx< uint, uint, uchar, 32, 16, 10, true, INI_DEBUG > *init10_lx;
    AHBBus_initiator_lx< uint, uint, uchar, 32, 16, 11, true, INI_DEBUG > *init11_lx;
    AHBBus_initiator_lx< uint, uint, uchar, 32, 16, 12, true, INI_DEBUG > *init12_lx;
    AHBBus_initiator_lx< uint, uint, uchar, 32, 16, 13, true, INI_DEBUG > *init13_lx;
    AHBBus_initiator_lx< uint, uint, uchar, 32, 16, 14, true, INI_DEBUG > *init14_lx;
    AHBBus_initiator_lx< uint, uint, uchar, 32, 16, 15, true, INI_DEBUG > *init15_lx;
    AHBBus_initiator_lx< uint, uint, uchar, 32, 16, 16, true, INI_DEBUG > *init16_lx;
    AHBBus_initiator_lx< uint, uint, uchar, 32, 16, 17, true, INI_DEBUG > *init17_lx;
    AHBBus_initiator_lx< uint, uint, uchar, 32, 16, 18, true, INI_DEBUG > *init18_lx;
    AHBBus_initiator_lx< uint, uint, uchar, 32, 16, 19, true, INI_DEBUG > *init19_lx;
#endif 

  public:
    void destroy_amba_ahb_platform()
    {
#ifdef LXBUILD
      //FIXME Here there's still a bug when dumbing the lx stats
      //Poletti
      #if 0
      if (CURRENT_ISS == LX)
      {
        for (int i = 0; i < N_CORES; i ++)
          delete lx[i];
      }   
      #endif
      
      switch (N_MASTERS)
      {
        case 20: delete init19_lx;
        case 19: delete init18_lx;
        case 18: delete init17_lx;
        case 17: delete init16_lx;
        case 16: delete init15_lx;
        case 15: delete init14_lx;
        case 14: delete init13_lx;
        case 13: delete init12_lx;
        case 12: delete init11_lx;
        case 11: delete init10_lx;
        case 10: delete init9_lx;
        case 9:  delete init8_lx;
        case 8:  delete init7_lx;
        case 7:  delete init6_lx;
        case 6:  delete init5_lx;
        case 5:  delete init4_lx;
        case 4:  delete init3_lx;
        case 3:  delete init2_lx;
        case 2:  delete init1_lx;
        case 1:  delete init0_lx;
        default: break;
      }
#endif
    }
    
    amba_ahb_platform(sc_module_name nm,
                      sc_trace_file *tf,
                      bool tracing,
                      int new_argc,
                      char *new_argv[],
                      char *new_envp[]) : sc_module(nm)
    {
    
      // ------------------ Signal and misc initialization ------------------
      char buffer[200]; //FIXME
      unsigned char i, j, k;

      // OCP signals
      sc_signal< sc_uint<MCMDWD> >           *MCmd;
      sc_signal< sc_uint<MATOMICLENGTHWD> >  *MAtomicLength;
      sc_signal< bool >                      *MDataLast;
      sc_signal< sc_uint<MADDRWD> >          *MAddr;
      sc_signal< sc_uint<MADDRSPACEWD> >     *MAddrSpace;
      sc_signal< sc_uint<MDATAWD> >          *MData;
      sc_signal< bool >                      *MDataValid;
      sc_signal< sc_uint<MBYTEENWD> >        *MByteEn;
      sc_signal< bool >                      *MRespAccept;
      sc_signal< sc_uint<MBURSTLENGTHWD> >   *MBurstLength;
      sc_signal< sc_uint<MBURSTSEQWD> >      *MBurstSeq;
      sc_signal< bool >                      *MBurstPrecise;
      sc_signal< bool >                      *MReqLast;
      sc_signal< sc_uint<MREQINFOWD> >       *MReqInfo;
      sc_signal< bool >                      *MBurstSingleReq;
      sc_signal< sc_uint<MTHREADIDWD> >      *MThreadID;
      sc_signal< sc_uint<MFLAGWD> >          *MFlag;
      sc_signal< sc_uint<MCONNIDWD> >        *MConnID;
      sc_signal< bool >                      *SCmdAccept;
      sc_signal< bool >                      *SDataAccept;
      sc_signal< sc_uint<SRESPWD> >          *SResp;
      sc_signal< sc_uint<SDATAWD> >          *SData;
      sc_signal< bool >                      *SRespLast;
      sc_signal< sc_uint<STHREADIDWD> >      *SThreadID;
      sc_signal< sc_uint<SFLAGWD> >          *SFlag;
      sc_signal< bool >                      *SInterrupt;

      MCmd            = new sc_signal< sc_uint<MCMDWD> >           [N_CORES + N_SLAVES];
      MAtomicLength   = new sc_signal< sc_uint<MATOMICLENGTHWD> >  [N_CORES + N_SLAVES];
      MDataLast       = new sc_signal< bool >                      [N_CORES + N_SLAVES];
      MAddr           = new sc_signal< sc_uint<MADDRWD> >          [N_CORES + N_SLAVES];
      MAddrSpace      = new sc_signal< sc_uint<MADDRSPACEWD> >     [N_CORES + N_SLAVES];
      MData           = new sc_signal< sc_uint<MDATAWD> >          [N_CORES + N_SLAVES];
      MByteEn         = new sc_signal< sc_uint<MBYTEENWD> >        [N_CORES + N_SLAVES];
      MDataValid      = new sc_signal< bool >                      [N_CORES + N_SLAVES];
      MRespAccept     = new sc_signal< bool >                      [N_CORES + N_SLAVES];
      MBurstLength    = new sc_signal< sc_uint<MBURSTLENGTHWD> >   [N_CORES + N_SLAVES];
      MBurstSeq       = new sc_signal< sc_uint<MBURSTSEQWD> >      [N_CORES + N_SLAVES];
      MBurstPrecise   = new sc_signal< bool >                      [N_CORES + N_SLAVES];
      MReqLast        = new sc_signal< bool >                      [N_CORES + N_SLAVES];
      MReqInfo        = new sc_signal< sc_uint<MREQINFOWD> >       [N_CORES + N_SLAVES];
      MBurstSingleReq = new sc_signal< bool >                      [N_CORES + N_SLAVES];
      MThreadID       = new sc_signal< sc_uint<MTHREADIDWD> >      [N_CORES + N_SLAVES];
      MFlag           = new sc_signal< sc_uint<MFLAGWD> >          [N_CORES + N_SLAVES];
      MConnID         = new sc_signal< sc_uint<MCONNIDWD> >        [N_CORES + N_SLAVES];
      SCmdAccept      = new sc_signal< bool >                      [N_CORES + N_SLAVES];
      SDataAccept     = new sc_signal< bool >                      [N_CORES + N_SLAVES];
      SResp           = new sc_signal< sc_uint<SRESPWD> >          [N_CORES + N_SLAVES];
      SData           = new sc_signal< sc_uint<MDATAWD> >          [N_CORES + N_SLAVES];
      SRespLast       = new sc_signal< bool >                      [N_CORES + N_SLAVES];
      SThreadID       = new sc_signal< sc_uint<STHREADIDWD> >      [N_CORES + N_SLAVES];
      SFlag           = new sc_signal< sc_uint<SFLAGWD> >          [N_CORES + N_SLAVES];
      SInterrupt      = new sc_signal< bool >                      [N_CORES + N_SLAVES];
    
      // Clock signals and dividers
      sc_signal< bool >                      *init_clock;
      sc_signal< bool >                      *interconnect_clock;  //FIXME (not *)
      sc_signal< sc_uint<FREQSCALING_BITS> > *init_div;
      sc_signal< sc_uint<FREQSCALING_BITS> > *interconnect_div;
      
      init_clock                 = new sc_signal< bool >                       [N_MASTERS];
      interconnect_clock         = new sc_signal< bool >                       [N_BUSES];
      init_div                   = new sc_signal< sc_uint<FREQSCALING_BITS> >  [N_MASTERS];
      interconnect_div           = new sc_signal< sc_uint<FREQSCALING_BITS> >  [N_BUSES];

      pinoutmast                 = new sc_signal<PINOUT>                       [N_MASTERS];
      requestmast                = new sc_signal<bool>                         [N_MASTERS];
      readymast                  = new sc_signal<bool>                         [N_MASTERS];
      pinoutmast_interconnect    = new sc_signal<PINOUT>                       [N_MASTERS];
      requestmast_interconnect   = new sc_signal<bool>                         [N_MASTERS];
      readymast_interconnect     = new sc_signal<bool>                         [N_MASTERS];
      
      extint                     = new sc_signal<bool>                         [NUMBER_OF_EXT * N_CORES];
      
      if (DMA)
      {
        pinoutwrappertodma       = new sc_signal<PINOUT>                       [N_CORES];
        readywrappertodma        = new sc_signal<bool>                         [N_CORES];
        requestwrappertodma      = new sc_signal<bool>                         [N_CORES];
        datadmacontroltotransfer = new sc_signal<DMA_CONT_REG1>                [N_CORES];
        requestcontroltotransfer = new sc_signal<bool>                         [N_CORES];
        finishedtransfer         = new sc_signal<bool>                         [N_CORES];
        spinoutscratchdma        = new sc_signal<PINOUT>                       [N_CORES];
        sreadyscratchdma         = new sc_signal<bool>                         [N_CORES];
        srequestscratchdma       = new sc_signal<bool>                         [N_CORES];
      }
      
      if (SMARTMEM)
      {
        smartmem_pinoutslavetodma         = new sc_signal<PINOUT>              [N_SMARTMEM];
        smartmem_readyslavetodma          = new sc_signal<bool>                [N_SMARTMEM];
        smartmem_requestslavetodma        = new sc_signal<bool>                [N_SMARTMEM];
        smartmem_datadmacontroltotransfer = new sc_signal<DMA_CONT_REG1>       [N_SMARTMEM];
        smartmem_requestcontroltotransfer = new sc_signal<bool>                [N_SMARTMEM];
        smartmem_finishedtransfer         = new sc_signal<bool>                [N_SMARTMEM];
        smartmem_spinoutscratchdma        = new sc_signal<PINOUT>              [N_SMARTMEM];
        smartmem_sreadyscratchdma         = new sc_signal<bool>                [N_SMARTMEM];
        smartmem_srequestscratchdma       = new sc_signal<bool>                [N_SMARTMEM];
      }
      
      
      // ------------------ Reset device ------------------
      resetter *reset_unit;
      reset_unit = new resetter("reset_device", (unsigned long int)1);
      reset_unit->clock(ClockGen_1);
      reset_unit->reset_out_true_high(ResetGen_1);
      reset_unit->reset_out_true_low(ResetGen_low_1);
      
      
      // ------------------ AMBA AHB interconnect ------------------
      amba_ahb_interconnect *amba_ahb_ic [N_BUSES];
      
      for (i = 0; i < N_BUSES; i++)
      {
        sprintf(buffer, "AMBA_AHB_IC_%d", i);
        amba_ahb_ic[i] = new amba_ahb_interconnect(buffer, i, tf, tracing);
      
        if (FREQSCALING || FREQSCALINGDEVICE)
          amba_ahb_ic[i]->clock(interconnect_clock[i]);
        else
          amba_ahb_ic[i]->clock(ClockGen_1);
        amba_ahb_ic[i]->reset(ResetGen_1);
      }
      
      
      // ----- Clock tree generator and frequency sync ----
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
        amba_ahb_prog_freq_reg *prog_register;
        feeder *p_register;
        if (FREQSCALINGDEVICE)
        {
          // A bus-attached slave device
          prog_register = new amba_ahb_prog_freq_reg("prog_register", addresser->FreqStartID(),
            FREQ_BASE, FREQ_SIZE, INT_WS, 0, N_FREQ_DEVICE + N_BUSES);
          prog_register->clock(ClockGen_1);
          prog_register->reset(ResetGen_1);
          amba_ahb_ic[SLAVE_CONFIG[addresser->FreqStartID()].binding]->slave_port(*prog_register);
     
          for (i = 0; i < N_FREQ_DEVICE; i ++)
            prog_register->p_feed[i](init_div[i]);
          for (i = 0; i < N_BUSES; i ++)
            prog_register->p_feed[N_FREQ_DEVICE + i](interconnect_div[i]);
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
      
      
      // ------------------ AMBA AHB memory slaves ------------------
      // Their instantiation must be done before that of
      // scratchpad memories, because if SPCHECK is active,
      // scratchpad memories must fetch initialization data
      // from external memories
      amba_ahb_slave *amba_ahb_s [N_PRIVATE + N_LX_PRIVATE + N_SHARED + N_SEMAPHORE];
      // FIXME no check whether we're attaching to a layer more slaves than it supports:
      // SystemC will rightly complain, but with a hard-to-understand error message. Add
      // a user-friendly debug here?
      
      for (i = 0; i < N_PRIVATE + N_LX_PRIVATE + N_SHARED + N_SEMAPHORE; i ++)
      {
        sprintf(buffer, "AMBA_AHB_slave_%d", i);
#ifdef LXBUILD
        if (addresser->IsLXPrivate(i) && CURRENT_ISS == LX)
          amba_ahb_s[i] = new amba_ahb_private_memory(buffer, i, addresser->ReturnSlavePhysicalAddress(i),
                                                      addresser->ReturnPrivateLXSize(), MEM_IN_WS, MEM_BB_WS);  
#endif
#ifdef SWARMBUILD
        if (addresser->IsPrivate(i) && CURRENT_ISS == SWARM)
          amba_ahb_s[i] = new amba_ahb_private_memory(buffer, i, addresser->ReturnSlavePhysicalAddress(i),
                                                      addresser->ReturnPrivateSize(), MEM_IN_WS, MEM_BB_WS);
#endif
        // FIXME some overlapping seems to be possible
        else if (addresser->IsShared(i))
          amba_ahb_s[i] = new amba_ahb_shared_memory(buffer, i, addresser->ReturnSlavePhysicalAddress(i),
                                                     addresser->ReturnSharedSize(i), MEM_IN_WS, MEM_BB_WS);
        else if (addresser->IsSemaphore(i))
          amba_ahb_s[i] = new amba_ahb_semaphore_device(buffer, i, addresser->ReturnSlavePhysicalAddress(i),
                                                        addresser->ReturnSemaphoreSize(i), MEM_IN_WS, MEM_BB_WS);
        
        if (FREQSCALING || FREQSCALINGDEVICE)
          amba_ahb_s[i]->clock(interconnect_clock[SLAVE_CONFIG[i].binding]);
        else
          amba_ahb_s[i]->clock(ClockGen_1);
        amba_ahb_s[i]->reset(ResetGen_1);
        amba_ahb_ic[SLAVE_CONFIG[i].binding]->slave_port(*amba_ahb_s[i]);
      }
      
      if (N_INTERRUPT)
      {
        amba_ahb_interrupt_device *amba_ahb_i [N_INTERRUPT];
      
        for (i = 0; i < N_INTERRUPT; i ++)
        {
          sprintf(buffer, "AMBA_AHB_slave_%d", addresser->InterruptStartID() + i);

          amba_ahb_i[i] = new amba_ahb_interrupt_device(buffer, addresser->InterruptStartID() + i,
                                                        addresser->ReturnSlavePhysicalAddress(addresser->InterruptStartID() + i),
                                                        addresser->ReturnInterruptSize(), INT_WS, 0);
          for (j = 0; j < N_CORES; j ++)
            amba_ahb_i[i]->extint[j](extint[(j*NUMBER_OF_EXT)+i]);
          
          if (FREQSCALING || FREQSCALINGDEVICE)
            amba_ahb_i[i]->clock(interconnect_clock[SLAVE_CONFIG[addresser->InterruptStartID() + i].binding]);
          else
            amba_ahb_i[i]->clock(ClockGen_1);
          amba_ahb_i[i]->reset(ResetGen_1);
          amba_ahb_ic[SLAVE_CONFIG[addresser->InterruptStartID() + i].binding]->slave_port(*amba_ahb_i[i]);
        }
      }
      
      
      // ------------------ Scratchpad memories ------------------
      Mem_class *datascratch [N_CORES];
      
      for (i = 0; i < N_CORES; i ++)
      {
        if (SCRATCH && SPCHECK)
          datascratch[i] = new Scratch_part_mem(i, addresser->ReturnScratchSize());
        else
          if (SCRATCH)
            datascratch[i] = new Scratch_mem("Scratchpad", i, addresser->ReturnScratchSize(), addresser->ReturnScratchPhysicalAddress(i));
          else
            datascratch[i] = NULL;
      }

      Scratch_queue_mem *queue [N_CORES];
  
      for (i = 0; i < N_CORESLAVE; i ++)
      {
        if (CORESLAVE)
        {
          sprintf(buffer, "Scratch_semaphore_%d",i);
          queue[i] = new Scratch_queue_mem(buffer, i, addresser->ReturnScratchSize(),
                                           addresser->ReturnQueuePhysicalAddress(i));
          queue[i]->sendint(extint[(i*NUMBER_OF_EXT) + N_INTERRUPT]);
        }
        else
          queue[i] = NULL;
      }
      
      
      // ------------------ AMBA AHB core-associated slaves ------------------
      // Their instantiation must be done after that of
      // scratchpad memories, because they need pointers
      // to scratchpad memories
      if (CORESLAVE)
      {
        amba_ahb_core_slave *amba_ahb_c [N_CORESLAVE];
        for (i = 0; i < N_CORESLAVE; i ++)
        {
          sprintf(buffer, "AMBA_AHB_Core_Slave_%d", addresser->CoreSlaveStartID() + i);
          amba_ahb_c[i] = new amba_ahb_core_slave (buffer, addresser->CoreSlaveStartID() + i, i,
                                                   addresser->ReturnSlavePhysicalAddress(addresser->CoreSlaveStartID() + i),
                                                   addresser->ReturnCoreSlaveSize(),
                                                   1, // FIXME should be SCRATCH_WS, set to 1
                                                      // for compatibility with other interconnects
                                                   0, datascratch[i], queue[i]);
                                                   
          if (FREQSCALING || FREQSCALINGDEVICE)
            amba_ahb_c[i]->clock(interconnect_clock[SLAVE_CONFIG[addresser->CoreSlaveStartID() + i].binding]);
          else
            amba_ahb_c[i]->clock(ClockGen_1);
          amba_ahb_c[i]->reset(ResetGen_1);
         amba_ahb_ic[SLAVE_CONFIG[addresser->CoreSlaveStartID() + i].binding]->slave_port(*amba_ahb_c[i]);
        }
      }
           
      // ------------------ AMBA AHB smart memories ------------------
      if (SMARTMEM)
      {
        Scratch_mem *smartscratch [N_SMARTMEM];
        amba_ahb_smart_mem_slave *amba_ahb_sm [N_SMARTMEM];
        Amba_sig_Dma_control_smartmem* smartdmacont[N_SMARTMEM]; //FIXME
        Amba_sig_Dma_transfer_smartmem* smarttransf[N_SMARTMEM]; //FIXME
  
        for (i = 0; i < N_SMARTMEM; i ++)
        {
          // create the actual smart memory
          smartscratch[i] = new Scratch_mem("Smart_mem", i, SMART_MEM_SIZE[i],
                                            (addresser->ReturnSlavePhysicalAddress(addresser->Smartmem_slave_StartID() + i)) + SMARTMEM_DMA_SIZE);

          // create and bind the slave
          sprintf(buffer, "AMBA_AHB_Smart_Mem_Slave_%d", addresser->Smartmem_slave_StartID() + i);
          amba_ahb_sm[i] = new amba_ahb_smart_mem_slave (buffer, addresser->Smartmem_slave_StartID() + i,
                                                         addresser->Smartmem_master_StartID() + i,
                                                         addresser->ReturnSlavePhysicalAddress(addresser->Smartmem_slave_StartID() + i),
                                                         SMART_MEM_SIZE[i], MEM_IN_WS, MEM_BB_WS, smartscratch[i]);
                                
          if (FREQSCALING || FREQSCALINGDEVICE)
            amba_ahb_sm[i]->clock(interconnect_clock[SLAVE_CONFIG[addresser->Smartmem_slave_StartID() + i].binding]);
          else      
            amba_ahb_sm[i]->clock(ClockGen_1);
          amba_ahb_sm[i]->reset(ResetGen_1);
          amba_ahb_ic[SLAVE_CONFIG[addresser->Smartmem_slave_StartID() + i].binding]->slave_port(*amba_ahb_sm[i]);
          
          // create the DMA controller
          sprintf(buffer, "AMBA_AHB_Smart_Mem_DMA_Controller_%d", DMA * N_CORES + i);
          smartdmacont[i] = new Amba_sig_Dma_control_smartmem(buffer, DMA*N_CORES + i, SMARTMEM_MAX_OBJ, N_CORES,
                                                              addresser->ReturnSlavePhysicalAddress(addresser->Smartmem_slave_StartID() + i));
          
          if (FREQSCALING || FREQSCALINGDEVICE)
            smartdmacont[i]->clock(interconnect_clock[SLAVE_CONFIG[addresser->Smartmem_slave_StartID() + i].binding]);
          else   
            smartdmacont[i]->clock(ClockGen_1);

          // create the SMARTMEM DMA transfer
          sprintf(buffer, "AMBA_AHB_Smart_Mem_DMA_Transfer_%d", DMA * N_CORES + i);
          smarttransf[i] = new Amba_sig_Dma_transfer_smartmem(buffer, DMA * N_CORES + i, SMARTMEM_DIM_BURST, SMARTMEM_MAX_OBJ,
                                                              N_CORES, smartscratch[i]);
          if (FREQSCALING || FREQSCALINGDEVICE)
            smarttransf[i]->clock(interconnect_clock[SLAVE_CONFIG[addresser->Smartmem_slave_StartID() + i].binding]);
          else   
            smarttransf[i]->clock(ClockGen_1);

          // bind the slave to the DMA controller
          amba_ahb_sm[i]->pinoutdma(smartmem_pinoutslavetodma[i]);
          amba_ahb_sm[i]->readydma(smartmem_readyslavetodma[i]);
          amba_ahb_sm[i]->requestdma(smartmem_requestslavetodma[i]);
          smartdmacont[i]->dmapin(smartmem_pinoutslavetodma[i]);
          smartdmacont[i]->readydma(smartmem_readyslavetodma[i]);
          smartdmacont[i]->requestdma(smartmem_requestslavetodma[i]);

          // bind the DMA controller to the DMA transfer module
          smartdmacont[i]->datadma(smartmem_datadmacontroltotransfer[i]);
          smartdmacont[i]->finished(smartmem_finishedtransfer[i]);
          smartdmacont[i]->requestdmatransfer(smartmem_requestcontroltotransfer[i]);
          smarttransf[i]->datadma(smartmem_datadmacontroltotransfer[i]);
          smarttransf[i]->finished(smartmem_finishedtransfer[i]);
          smarttransf[i]->requestdmatransfer(smartmem_requestcontroltotransfer[i]);

          // bind the DMA transfer module to the DMA bus master
          // It is important to get the correct DMA ID to bind it to the right master.
          // In these calls, the second "DMA * N_CORES" is the number of DMA masters
          // associated with cores (if they are instantiated (DMA == true), they are
          // as many as the cores (== N_CORES))
          smarttransf[i]->pinout(pinoutmast[N_CORES + DMA * N_CORES + i]);
          smarttransf[i]->ready_from_master(readymast[N_CORES + DMA * N_CORES + i]);
          smarttransf[i]->request_to_master(requestmast[N_CORES + DMA * N_CORES + i]);

          //TODO is this OK and do we properly trace all of the signals?
          for (j = 0; j < N_CORES; j ++)
            for (k = 0; k < SMARTMEM_MAX_OBJ; k ++)
              smarttransf[i]->extint[j * SMARTMEM_MAX_OBJ + k]
                (extint[(NUMBER_OF_EXT * j) + k + (SMARTMEM_MAX_OBJ * i) + N_INTERRUPT]);
        }
      }
      
      
      // ------------------ AMBA AHB bus masters ------------------
      amba_ahb_ocp_master *amba_ahb_ocp_m [N_MASTERS];
      amba_ahb_master *amba_ahb_m [N_MASTERS];
      
#ifdef LXBUILD
  #ifdef WITH_INI_DEBUG
    #define INI_DEBUG true
  #else
    #define INI_DEBUG false
  #endif
      init0_lx  = NULL;
      init1_lx  = NULL;
      init2_lx  = NULL;
      init3_lx  = NULL;
      init4_lx  = NULL;
      init5_lx  = NULL;
      init6_lx  = NULL;
      init7_lx  = NULL;
      init8_lx  = NULL;
      init9_lx  = NULL;
      init10_lx = NULL;
      init11_lx = NULL;
      init12_lx = NULL;
      init13_lx = NULL;
      init14_lx = NULL;
      init15_lx = NULL;
      init16_lx = NULL;
      init17_lx = NULL;
      init18_lx = NULL;
      init19_lx = NULL;
#endif  
      
      for (i = 0; i < N_MASTERS; i ++)
      {
#ifdef LXBUILD //FIXME
  #include "amba_ahb_initiator_inst_lx.cpp"
#endif
#ifdef SWARMBUILD
        if (CURRENT_ISS == SWARM)
        {
          if (USING_OCP)
          {
            sprintf(buffer, "AMBA_AHB_OCP_Master_%d", i);
            amba_ahb_ocp_m[i] = new amba_ahb_ocp_master(buffer, i);
            if (FREQSCALING || FREQSCALINGDEVICE)
              amba_ahb_ocp_m[i]->clock(interconnect_clock[MASTER_CONFIG[i].binding]);
            else
              amba_ahb_ocp_m[i]->clock(ClockGen_1);
            amba_ahb_ocp_m[i]->reset(ResetGen_1);
            amba_ahb_ocp_m[i]->MCmd(MCmd[i]);
            amba_ahb_ocp_m[i]->MAtomicLength(MAtomicLength[i]);
            amba_ahb_ocp_m[i]->MBurstLength(MBurstLength[i]);
            amba_ahb_ocp_m[i]->MBurstPrecise(MBurstPrecise[i]);
            amba_ahb_ocp_m[i]->MBurstSeq(MBurstSeq[i]);
            amba_ahb_ocp_m[i]->MBurstSingleReq(MBurstSingleReq[i]);
            amba_ahb_ocp_m[i]->MDataLast(MDataLast[i]);
            amba_ahb_ocp_m[i]->MReqLast(MReqLast[i]);
            amba_ahb_ocp_m[i]->MAddr(MAddr[i]);
            amba_ahb_ocp_m[i]->MAddrSpace(MAddrSpace[i]);
            amba_ahb_ocp_m[i]->MData(MData[i]);
            amba_ahb_ocp_m[i]->MDataValid(MDataValid[i]);
            amba_ahb_ocp_m[i]->MByteEn(MByteEn[i]);
            amba_ahb_ocp_m[i]->SCmdAccept(SCmdAccept[i]);
            amba_ahb_ocp_m[i]->SDataAccept(SDataAccept[i]);
            amba_ahb_ocp_m[i]->SResp(SResp[i]);
            amba_ahb_ocp_m[i]->SRespLast(SRespLast[i]);
            amba_ahb_ocp_m[i]->SData(SData[i]);
            amba_ahb_ocp_m[i]->MRespAccept(MRespAccept[i]);
            amba_ahb_ocp_m[i]->MFlag(MFlag[i]);
            amba_ahb_ocp_m[i]->SFlag(SFlag[i]);
            amba_ahb_ocp_m[i]->SInterrupt(SInterrupt[i]);
            amba_ahb_ocp_m[i]->master_port(*amba_ahb_ic[MASTER_CONFIG[i].binding]);
            // #ifndef OLD_INT_STYLE //FIXME
            for (j = 0; j < NUMBER_OF_EXT; j ++)
              amba_ahb_ocp_m[i]->extint[j](extint[i * NUMBER_OF_EXT + j]);
            // #endif
          }
          else
          {
            sprintf(buffer, "AMBA_AHB_Master_%d", i);
            amba_ahb_m[i] = new amba_ahb_master(buffer, i);
            if (FREQSCALING || FREQSCALINGDEVICE)
              amba_ahb_m[i]->clock(interconnect_clock[MASTER_CONFIG[i].binding]);
            else
              amba_ahb_m[i]->clock(ClockGen_1);
            amba_ahb_m[i]->reset(ResetGen_1);
            if ((FREQSCALING && M_DIVIDER[i] != I_DIVIDER[MASTER_CONFIG[i].binding]) || FREQSCALINGDEVICE)
            {
              amba_ahb_m[i]->request_from_wrapper(requestmast_interconnect[i]);
              amba_ahb_m[i]->ready_to_wrapper(readymast_interconnect[i]);
              amba_ahb_m[i]->pinout(pinoutmast_interconnect[i]);
            }
            else
            {
              amba_ahb_m[i]->request_from_wrapper(requestmast[i]);
              amba_ahb_m[i]->ready_to_wrapper(readymast[i]);
              amba_ahb_m[i]->pinout(pinoutmast[i]);
            }
            amba_ahb_m[i]->master_port(*amba_ahb_ic[MASTER_CONFIG[i].binding]);
          }
        }
#endif
      }
 
      // ------------------ AMBA AHB bus bridges ------------------
      amba_ahb_bridge *bridge [N_BRIDGES];
     
      for (i = 0; i < N_BRIDGES; i++)
      {
        sprintf(buffer, "AMBA_AHB_Bridge_%d", i);
          
        bridge[i] = new amba_ahb_bridge(buffer, i);
        if(FREQSCALING || FREQSCALINGDEVICE)
        {
          // Clock to slave side
          bridge[i]->clk_slave(interconnect_clock[BRIDGE_CONFIG[i].slave_binding]);
          
          //  Clock to master side
          bridge[i]->clk_master(interconnect_clock[BRIDGE_CONFIG[i].master_binding]);
        }
        else
        {     
          // Clock to slave side
          bridge[i]->clk_slave(ClockGen_1);
          
          //  Clock to master side 
          bridge[i]->clk_master(ClockGen_1);
        }

        amba_ahb_ic[BRIDGE_CONFIG[i].slave_binding]->slave_port(*bridge[i]->bsif);
        bridge[i]->master_port(*amba_ahb_ic[BRIDGE_CONFIG[i].master_binding]);
      }
      
      // ------------------ System processors ------------------
#ifdef SIMITARMBUILD
      Wrapper *soc[N_CORES];
#elif defined POWERPCBUILD
      PowerPC *soc[N_CORES];
#elif defined SWARMBUILD
      ocpswarm *ocp_soc[N_CORES];
      armsystem *soc[N_CORES];
#endif
#ifdef TGBUILD
      TGMaster *TGen[N_CORES];
#endif
#ifdef LXBUILD
      ASTISSWrapper *lx[N_CORES];
#endif

      for (i = 0; i < N_CORES; i ++)
      {
        if (CURRENT_ISS == TG)
        {
#ifdef TGBUILD
          sprintf(buffer, "TrafficGen_%d", i);
          char pgm_bin_name[150];
          char pgm_par_name[150];
          
          char *env;
          env = getenv("TGBASEPATH");
          if (env)
          {
            sprintf(pgm_bin_name, "%s-%dP/trace_%d_%u_0x%08x_0x%08x.BIN", env, N_CORES, i, CLOCKPERIOD,
              addresser->ReturnSlavePhysicalAddress(addresser->SemaphoreStartID()),
              addresser->ReturnSemaphoreSize(addresser->SemaphoreStartID())); // FIXME assumes only one semaphore will be instantiated
            sprintf(pgm_par_name, "%s-%dP/TG%d_mem_parameter.mem", env, N_CORES, i);
          }
          else
          {
            printf("Fatal Error: cannot find TG programs to run. Please set the environment variable TGBASEPATH and retry!\n");
            printf("             Example: to use the program and parameter files in \"/home/user/traces-2P/\", please set it to\n");
            printf("             \"/home/user/traces\" and launch the simulation with 2 processors.\n");
            exit(1);
          }
            
          TGen[i] = new TGMaster(buffer, i, pgm_bin_name, 0, i);
          if (FREQSCALING || FREQSCALINGDEVICE)
            TGen[i]->Clk(init_clock[i]);
          else
            TGen[i]->Clk(ClockGen_1);
          TGen[i]->Reset(ResetGen_1);
          
          TGen[i]->MCmd(MCmd[i]);
          TGen[i]->MAddr(MAddr[i]);
          TGen[i]->MData(MData[i]);
          TGen[i]->MRespAccept(MRespAccept[i]);
          TGen[i]->MBurstLength(MBurstLength[i]);
          TGen[i]->MBurstSeq(MBurstSeq[i]);
          TGen[i]->MBurstPrecise(MBurstPrecise[i]);
          TGen[i]->MReqLast(MReqLast[i]);
          TGen[i]->MReqInfo(MReqInfo[i]);
          TGen[i]->MBurstSingleReq(MBurstSingleReq[i]);
          TGen[i]->MByteEn(MByteEn[i]);
          TGen[i]->MThreadID(MThreadID[i]);
          TGen[i]->MDataValid(MDataValid[i]);
          TGen[i]->MFlag(MFlag[i]);
          TGen[i]->MConnID(MConnID[i]);
          
          TGen[i]->mSCmdAccept(SCmdAccept[i]);
          TGen[i]->mSResp(SResp[i]);
          TGen[i]->mSData(SData[i]);
          TGen[i]->mSDataAccept(SDataAccept[i]);
          TGen[i]->mSRespLast(SRespLast[i]);
          TGen[i]->mSThreadID(SThreadID[i]);
          TGen[i]->mSFlag(SFlag[i]);
          TGen[i]->mSInterrupt(SInterrupt[i]);
#endif
        }
        else
        {
#ifdef SIMITARMBUILD
          sprintf(buffer, "StrongARM_%d", i);
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
          if (FREQSCALING || FREQSCALINGDEVICE)
            soc[i]->clock(init_clock[i]);
          else
            soc[i]->clock(ClockGen_1);
          soc[i]->pinout_ft_master[0](pinoutmast[i]);
          soc[i]->ready_from_master[0](readymast[i]);
          soc[i]->request_to_master[0](requestmast[i]);
          for (j = 0; j < NUMBER_OF_EXT; j ++)
            soc[i]->extint[j](extint[i * NUMBER_OF_EXT + j]);
#elif defined POWERPCBUILD
          sprintf(buffer, "PowerPC_%d", i);
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
          if (FREQSCALING || FREQSCALINGDEVICE)
            soc[i]->clock(init_clock[i]);
          else
            soc[i]->clock(ClockGen_1);
          soc[i]->pinout_ft_master[0](pinoutmast[i]);
          soc[i]->ready_from_master[0](readymast[i]);
          soc[i]->request_to_master[0](requestmast[i]);
          for (j = 0; j < NUMBER_OF_EXT; j ++)
            soc[i]->extint[j](extint[i * NUMBER_OF_EXT + j]);
#endif
#ifdef LXBUILD
          if (CURRENT_ISS == LX)
          {
            sprintf(buffer, "LX_%d", i);
            char targetFilename[40];
            sprintf(targetFilename, "my_appli%d.elf",i);
            char* execPath="$SWARMDIR/bin/mpsim.x";
            char* statPath=".";   
            
            std::vector<std::string> fakeArguments;
            fakeArguments.push_back(targetFilename);
            
            lx[i] = new ASTISSWrapper(buffer,      // UNCHANGED
                                      execPath,    // Should be the MPARM name
                                      ASTISSWrapper::LITTLE,
                                      targetFilename,
                                      fakeArguments,
                                      new_envp,    // UNCHANGED
                                      0,           // UNCHANGED
                                      0,
                                      0,
                                      false,       // enables debug prints in the wrapper
                                      false,
                                      false,       // should dump at end (however destructor must be invoked explicitly)
                                      statPath);
                                
            if (FREQSCALING || FREQSCALINGDEVICE)
              lx[i]->clk(init_clock[i]);
            else
              lx[i]->clk(ClockGen_1);
 
            for (j = 0; j < NUMBER_OF_EXT; j ++)
              lx[i]->irq_port(extint[i * NUMBER_OF_EXT + j], 32 + j);
          }
#endif
#ifdef SWARMBUILD //FIXME this will always override SIMITARM or POWERPC cores
          if (CURRENT_ISS == SWARM && USING_OCP)
          {
            sprintf(buffer, "OCP_ARM_%d", i);
            ocp_soc[i] = new ocpswarm(buffer, (unsigned int)i, datascratch[i], queue[i]);
            if (FREQSCALING || FREQSCALINGDEVICE)
              ocp_soc[i]->clock(init_clock[i]);
            else
              ocp_soc[i]->clock(ClockGen_1);
            ocp_soc[i]->MCmd[0](MCmd[i]);
            ocp_soc[i]->MAtomicLength[0](MAtomicLength[i]);
            ocp_soc[i]->MBurstLength[0](MBurstLength[i]);
            ocp_soc[i]->MBurstPrecise[0](MBurstPrecise[i]);
            ocp_soc[i]->MBurstSeq[0](MBurstSeq[i]);
            ocp_soc[i]->MBurstSingleReq[0](MBurstSingleReq[i]);
            ocp_soc[i]->MDataLast[0](MDataLast[i]);
            ocp_soc[i]->MReqLast[0](MReqLast[i]);
            ocp_soc[i]->MAddr[0](MAddr[i]);
            ocp_soc[i]->MAddrSpace[0](MAddrSpace[i]);
            ocp_soc[i]->MData[0](MData[i]);
            ocp_soc[i]->MByteEn[0](MByteEn[i]);
            ocp_soc[i]->MDataValid[0](MDataValid[i]);
            ocp_soc[i]->SCmdAccept[0](SCmdAccept[i]);
            ocp_soc[i]->SDataAccept[0](SDataAccept[i]);
            ocp_soc[i]->SResp[0](SResp[i]);
            ocp_soc[i]->SRespLast[0](SRespLast[i]);
            ocp_soc[i]->SData[0](SData[i]);
            ocp_soc[i]->MRespAccept[0](MRespAccept[i]);
            ocp_soc[i]->MFlag[0](MFlag[i]);
            ocp_soc[i]->SFlag[0](SFlag[i]);
            ocp_soc[i]->SInterrupt[0](SInterrupt[i]);
            for (j = 0; j < NUMBER_OF_EXT; j ++)
              ocp_soc[i]->extint[j](extint[i * NUMBER_OF_EXT + j]);
          }
          else if (CURRENT_ISS == SWARM)
          {
            sprintf(buffer, "ARM_%d", i);
            soc[i] = new armsystem(buffer, (unsigned int)i, datascratch[i], queue[i]);
            if (FREQSCALING || FREQSCALINGDEVICE)
              soc[i]->clock(init_clock[i]);
            else
              soc[i]->clock(ClockGen_1);
            soc[i]->pinout_ft_master[0](pinoutmast[i]);
            soc[i]->ready_from_master[0](readymast[i]);
            soc[i]->request_to_master[0](requestmast[i]);
            for (j = 0; j < NUMBER_OF_EXT; j ++)
              soc[i]->extint[j](extint[i * NUMBER_OF_EXT + j]);
          }
#endif
        }
      }
      
#ifdef LXBUILD
      if (CURRENT_ISS == LX)
      {
        switch (N_CORES)
        {
          case 18:  lx[17]->io_port(*init17_lx);
          case 17:  lx[16]->io_port(*init16_lx);
          case 16:  lx[15]->io_port(*init15_lx);
          case 15:  lx[14]->io_port(*init14_lx);
          case 14:  lx[13]->io_port(*init13_lx);
          case 13:  lx[12]->io_port(*init12_lx);
          case 12:  lx[11]->io_port(*init11_lx);
          case 11:  lx[10]->io_port(*init10_lx);
          case 10:  lx[9]->io_port(*init9_lx);
          case 9:   lx[8]->io_port(*init8_lx);
          case 8:   lx[7]->io_port(*init7_lx);
          case 7:   lx[6]->io_port(*init6_lx);
          case 6:   lx[5]->io_port(*init5_lx);
          case 5:   lx[4]->io_port(*init4_lx);
          case 4:   lx[3]->io_port(*init3_lx);
          case 3:   lx[2]->io_port(*init2_lx);
          case 2:   lx[1]->io_port(*init1_lx);
          case 1:   lx[0]->io_port(*init0_lx);
                    break;
          default:  printf("Fatal Error: you can't instantiate more then 19 LX cores!"); //FIXME check in parser! (why 19?)
                    exit(1);
        }
      }
#endif
      
      // ------------------ DMA devices ------------------
      if (DMA)
      {
        amba_ahb_dma_control_scratch  *dmacont[N_CORES];
        amba_ahb_dma_transfer_scratch *transf[N_CORES];

        for (i = 0; i < N_CORES; i ++)
        {
          // DMA controller
          sprintf(buffer, "DMA_Controller_%d", i);
          dmacont[i] = new
            amba_ahb_dma_control_scratch(buffer, i, INTERNAL_MAX_OBJ, 1, addresser->ReturnDMAPhysicalAddress());
          if (FREQSCALING || FREQSCALINGDEVICE)
            dmacont[i]->clock(init_clock[i]);
          else
            dmacont[i]->clock(ClockGen_1);
      
          // DMA transfer module
          sprintf(buffer,"DMA_Transfer_%d", i);
          transf[i] = new
            amba_ahb_dma_transfer_scratch(buffer, i, INTERNAL_DIM_BURST, INTERNAL_MAX_OBJ, 1, datascratch[i]);
          if (FREQSCALING || FREQSCALINGDEVICE)
            transf[i]->clock(init_clock[i]);
          else
            transf[i]->clock(ClockGen_1);

          // connect the DMA controller to a core
#if defined(WITHOUT_OCP) && defined(SWARMBUILD)  // FIXME SWARM is not the only "soc" around
          if (CURRENT_ISS == SWARM)              // (and how do you plug DMAs to LX?)
          {
            soc[i]->pinout_ft_master[1](pinoutwrappertodma[i]);
            soc[i]->ready_from_master[1](readywrappertodma[i]);
            soc[i]->request_to_master[1](requestwrappertodma[i]);
          }
#endif
          
          dmacont[i]->dmapin(pinoutwrappertodma[i]);
          dmacont[i]->readydma(readywrappertodma[i]);
          dmacont[i]->requestdma(requestwrappertodma[i]);

          // connect the DMA controller to its DMA transfer module
          dmacont[i]->datadma(datadmacontroltotransfer[i]);
          dmacont[i]->finished(finishedtransfer[i]);
          dmacont[i]->requestdmatransfer(requestcontroltotransfer[i]);
          transf[i]->datadma(datadmacontroltotransfer[i]);
          transf[i]->finished(finishedtransfer[i]);
          transf[i]->requestdmatransfer(requestcontroltotransfer[i]);

          // connect the DMA transfer module to a bus master
          if (FREQSCALING || FREQSCALINGDEVICE)
          {
            transf[i]->pinout(pinoutmast_interconnect[N_CORES + i]);
            transf[i]->ready_from_master(readymast_interconnect[N_CORES + i]);
            transf[i]->request_to_master(requestmast_interconnect[N_CORES + i]);
          }
          else
          {
            transf[i]->pinout(pinoutmast[N_CORES + i]);
            transf[i]->ready_from_master(readymast[N_CORES + i]);
            transf[i]->request_to_master(requestmast[N_CORES + i]);
          }
        }
      }
      
      
      // ------------------ Tracing for platform-wide signals ------------------
      if (tracing)
      {
        sc_trace(tf, ClockGen_1, "ClockGen_1");
        sc_trace(tf, ResetGen_1, "ResetGen_1");
        if (FREQSCALING || FREQSCALINGDEVICE)
        {
          for (i = 0; i < N_BUSES; i ++)
          {
            sprintf(buffer, "%s_interconnect_clock_%d", name(), i);
            sc_trace(tf, interconnect_clock[i], buffer);
            sprintf(buffer, "%s_interconnect_div_%d", name(), i);
            sc_trace(tf, interconnect_div[i], buffer);
          }
        }
        
        for (i = 0; i < N_MASTERS; i ++)
        {
          if ((FREQSCALING || FREQSCALINGDEVICE) && (i < N_FREQ_DEVICE))
          {
            sprintf(buffer, "%s_init_clock_%d", name(), i);
            sc_trace(tf, init_clock[i], buffer);
            sprintf(buffer, "%s_init_div_%d", name(), i);
            sc_trace(tf, init_div[i], buffer);
          }
#ifdef SWARMBUILD //FIXME
          if (USING_OCP || CURRENT_ISS == TG)
          {
            sprintf(buffer, "%s_master_%d_MCmd", name(), i);
            sc_trace(tf, amba_ahb_ocp_m[i]->MCmd, buffer);
            sprintf(buffer, "%s_master_%d_MAddr", name(), i);
            sc_trace(tf, amba_ahb_ocp_m[i]->MAddr, buffer);
            sprintf(buffer, "%s_master_%d_MData", name(), i);
            sc_trace(tf, amba_ahb_ocp_m[i]->MData, buffer);
            sprintf(buffer, "%s_master_%d_MByteEn", name(), i);
            sc_trace(tf, amba_ahb_ocp_m[i]->MByteEn, buffer);
            sprintf(buffer, "%s_master_%d_MDataValid", name(), i);
            sc_trace(tf, amba_ahb_ocp_m[i]->MDataValid, buffer);
            sprintf(buffer, "%s_master_%d_MBurstPrecise", name(), i);
            sc_trace(tf, amba_ahb_ocp_m[i]->MBurstPrecise, buffer);
            sprintf(buffer, "%s_master_%d_MBurstSingleReq", name(), i);
            sc_trace(tf, amba_ahb_ocp_m[i]->MBurstSingleReq, buffer);
            sprintf(buffer, "%s_master_%d_MBurstSeq", name(), i);
            sc_trace(tf, amba_ahb_ocp_m[i]->MBurstSeq, buffer);
            sprintf(buffer, "%s_master_%d_MBurstLength", name(), i);
            sc_trace(tf, amba_ahb_ocp_m[i]->MBurstLength, buffer);
            sprintf(buffer, "%s_master_%d_MReqLast", name(), i);
            sc_trace(tf, amba_ahb_ocp_m[i]->MReqLast, buffer);
            sprintf(buffer, "%s_master_%d_MRespAccept", name(), i);
            sc_trace(tf, amba_ahb_ocp_m[i]->MRespAccept, buffer);
            sprintf(buffer, "%s_master_%d_MFlag", name(), i);
            sc_trace(tf, amba_ahb_ocp_m[i]->MFlag, buffer);
            sprintf(buffer, "%s_master_%d_SCmdAccept", name(), i);
            sc_trace(tf, amba_ahb_ocp_m[i]->SCmdAccept, buffer);
            sprintf(buffer, "%s_master_%d_SDataAccept", name(), i);
            sc_trace(tf, amba_ahb_ocp_m[i]->SDataAccept, buffer);
            sprintf(buffer, "%s_master_%d_SResp", name(), i);
            sc_trace(tf, amba_ahb_ocp_m[i]->SResp, buffer);
            sprintf(buffer, "%s_master_%d_SRespLast", name(), i);
            sc_trace(tf, amba_ahb_ocp_m[i]->SRespLast, buffer);
            sprintf(buffer, "%s_master_%d_SData", name(), i);
            sc_trace(tf, amba_ahb_ocp_m[i]->SData, buffer);
            sprintf(buffer, "%s_master_%d_SFlag", name(), i);
            sc_trace(tf, amba_ahb_ocp_m[i]->SFlag, buffer);
            sprintf(buffer, "%s_master_%d_SInterrupt", name(), i);
            sc_trace(tf, amba_ahb_ocp_m[i]->SInterrupt, buffer);
          }
          else
          {
            sprintf(buffer, "%s_master_%d_pinoutmast", name(), i);
            sc_trace(tf, pinoutmast[i], buffer);
            sprintf(buffer, "%s_master_%d_requestmast", name(), i);
            sc_trace(tf, requestmast[i], buffer);
            sprintf(buffer, "%s_master_%d_readymast", name(), i);
            sc_trace(tf, readymast[i], buffer);
            
            sprintf(buffer, "%s_master_%d_pinoutmast_interconnect", name(), i);
            sc_trace(tf, pinoutmast_interconnect[i], buffer);
            sprintf(buffer, "%s_master_%d_requestmast_interconnect", name(), i);
            sc_trace(tf, requestmast[i], buffer);
            sprintf(buffer, "%s_master_%d_readymast_interconnect", name(), i);
            sc_trace(tf, readymast_interconnect[i], buffer);
          }
#endif
        }
        
        if (DMA)
          for (i = 0; i < N_CORES; i ++)
          {
            sprintf(buffer, "%s_pinoutwrappertodma_%d", name(), i);
            sc_trace(tf, pinoutwrappertodma[i], buffer);
            sprintf(buffer, "%s_readywrappertodma_%d", name(), i);
            sc_trace(tf, readywrappertodma[i], buffer);
            sprintf(buffer, "%s_requestwrappertodma_%d", name(), i);
            sc_trace(tf, requestwrappertodma[i], buffer);
            sprintf(buffer, "%s_datadmacontroltotransfer_%d", name(), i);
            sc_trace(tf, datadmacontroltotransfer[i], buffer);
            sprintf(buffer, "%s_requestcontroltotransfer_%d", name(), i);
            sc_trace(tf, requestcontroltotransfer[i], buffer);
            sprintf(buffer, "%s_finishedtransfer_%d", name(), i);
            sc_trace(tf, finishedtransfer[i], buffer);
            sprintf(buffer, "%s_spinoutscratchdma_%d", name(), i);
            sc_trace(tf, spinoutscratchdma[i], buffer);
            sprintf(buffer, "%s_sreadyscratchdma_%d", name(), i);
            sc_trace(tf, sreadyscratchdma[i], buffer);
            sprintf(buffer, "%s_srequestscratchdma_%d", name(), i);
            sc_trace(tf, srequestscratchdma[i], buffer);
          }

        if (SMARTMEM)
          for (i = 0; i < N_SMARTMEM; i ++)
          {
            sprintf(buffer, "%s_smartmem_pinoutslavetodma_%d", name(), i);
            sc_trace(tf, smartmem_pinoutslavetodma[i], buffer);
            sprintf(buffer, "%s_smartmem_readyslavetodma_%d", name(), i);
            sc_trace(tf, smartmem_readyslavetodma[i], buffer);
            sprintf(buffer, "%s_smartmem_requestslavetodma_%d", name(), i);
            sc_trace(tf, smartmem_requestslavetodma[i], buffer);
            sprintf(buffer, "%s_smartmem_datadmacontroltotransfer_%d", name(), i);
            sc_trace(tf, smartmem_datadmacontroltotransfer[i], buffer);
            sprintf(buffer, "%s_smartmem_requestcontroltotransfer_%d", name(), i);
            sc_trace(tf, smartmem_requestcontroltotransfer[i], buffer);
            sprintf(buffer, "%s_smartmem_finishedtransfer_%d", name(), i);
            sc_trace(tf, smartmem_finishedtransfer[i], buffer);
            sprintf(buffer, "%s_smartmem_spinoutscratchdma_%d", name(), i);
            sc_trace(tf, smartmem_spinoutscratchdma[i], buffer);
            sprintf(buffer, "%s_smartmem_sreadyscratchdma_%d", name(), i);
            sc_trace(tf, smartmem_sreadyscratchdma[i], buffer);
            sprintf(buffer, "%s_smartmem_srequestscratchdma_%d", name(), i);
            sc_trace(tf, smartmem_srequestscratchdma[i], buffer);
          }
          
        for (i = 0; i < N_CORES; i ++)
          for (j = 0; j < NUMBER_OF_EXT; j ++)
          {
            sprintf(buffer, "%s_extint_core_%d_line_%d", name(), i, j);
            sc_trace(tf, extint[i * NUMBER_OF_EXT + j], buffer);
          }
      }
    }
};

#endif // __AMBA_AHB_PLATFORM_H__
