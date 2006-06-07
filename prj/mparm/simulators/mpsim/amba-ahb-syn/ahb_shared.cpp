
#include "ahb_shared.h"
#include "clock_tree.h"
#include "reset.h"
#include "freq_register.h"
#include "dual_clock_pinout_adapter.h"

//#define AHB_MONITOR

void SharedAHB::InitInstances()
{
  int i, j;
  char buffer[20];
  char buffer2[20];
  
  // Clock signals and dividers
  init_clock = new sc_signal< bool > [N_MASTERS];
  interconnect_clock = new sc_signal< bool > [N_BUSES];
  init_div = new sc_signal< sc_uint<FREQSCALING_BITS> > [N_MASTERS];
  interconnect_div = new sc_signal< sc_uint<FREQSCALING_BITS> > [N_BUSES];
  // System wires
  extint = new sc_signal<bool> [NUMBER_OF_EXT*N_CORES];
  readymast = new sc_signal<bool> [N_MASTERS];
  pinoutmast = new sc_signal<PINOUT> [N_MASTERS];
  requestmast = new sc_signal<bool> [N_MASTERS];
  readymast_interconnect = new sc_signal<bool> [N_MASTERS];
  pinoutmast_interconnect = new sc_signal<PINOUT> [N_MASTERS];
  requestmast_interconnect = new sc_signal<bool> [N_MASTERS];
  
  hlock = new sc_signal<sc_bv<16> >;
  hgrant = new sc_signal<sc_bv<16> >;
  hbusreq = new sc_signal<sc_bv<16> >;
  hsplit_s = new sc_signal<sc_bv<16> > [16];
  hsel = new sc_signal<sc_bv<16> >;
  hwrite = new sc_signal<bool >;
  hwdata= new sc_signal<sc_bv<1024> >;
  htrans = new sc_signal<sc_bv<2> >;
  hsize = new sc_signal<sc_bv<3> >;
  hresp= new sc_signal<sc_bv<2> >;
  hready = new sc_signal<bool >;
  hrdata = new sc_signal<sc_bv<1024> >;
  hprot = new sc_signal<sc_bv<4> >;
  hmastlock = new sc_signal<bool >;
  hmaster = new sc_signal<sc_bv<4> >;
  hburst = new sc_signal<sc_bv<3> >;
  haddr = new sc_signal<sc_bv<64> >;
 
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
      ctree->Div[i](init_div[i]);
      ctree->Clock_out[i](init_clock[i]);
    }
    for (i = 0; i < N_BUSES; i ++)
    {
      ctree->Div[N_FREQ_DEVICE + i](interconnect_div[i]);
      ctree->Clock_out[N_FREQ_DEVICE + i](interconnect_clock[i]);
    }
    
    // Programmable register for the frequency divider
    feeder *p_register;
    p_register = new feeder("p_register", N_FREQ_DEVICE + N_BUSES);
    p_register->clock(ClockGen_1);
    
    for (i = 0; i < N_FREQ_DEVICE; i ++)
      p_register->feed[i](init_div[i]);
    for (i = 0; i < N_BUSES; i ++)
      p_register->feed[N_FREQ_DEVICE + i](interconnect_div[i]);

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
        queue[i] = new Scratch_queue_mem(buffer,i,addresser->ReturnScratchSize(),addresser->ReturnQueuePhysicalAddress(i));
        queue[i]->sendint(extint[(i*NUMBER_OF_EXT)+N_INTERRUPT]);
      }
     else
       queue[i] = NULL;
    }
//////////////////////////////    
//MODULI INTERCONNECT       //
/////////////////////////////      
    // Identification number of the bus.
    int tmp_AHB_1_BusID = 0; 
    ccss_set_parameter("AHB_1.BusID", tmp_AHB_1_BusID);
    // Specifies the priority of the AHB bus layer. This parameter
    // can be ignored for a single layer bus system.
    int tmp_AHB_1_Priority = 0;
    ccss_set_parameter("AHB_1.Priority", tmp_AHB_1_Priority);
    // Delay of the monitor update event after the negative clock edge.    
    sc_time tmp_AHB_1_MonitorDelay = SC_ZERO_TIME;
    ccss_set_parameter("AHB_1.MonitorDelay", tmp_AHB_1_MonitorDelay);
    AHB_1 = new DW_ahb_tlm<ahb_namespace::ahb_bus_if, ahb_namespace::ahb_slave_if >("AHB_1");
    
    // The slave id of the arbiter.
    int tmp_Arbiter_SlaveID = 0;
    ccss_set_parameter("Arbiter.SlaveID", tmp_Arbiter_SlaveID);
    // When this parameter is enabled, unspecified lenght bursts will not
    // be re-arbitrated.
    bool tmp_Arbiter_AHB_FULL_INCR = false;
    ccss_set_parameter("Arbiter.AHB_FULL_INCR", tmp_Arbiter_AHB_FULL_INCR);
    // This parameter enables the re-arbitration feature of incrementing
    // or wrapping bursts with a defined length.
    bool tmp_Arbiter_EBTEN = false;
    ccss_set_parameter("Arbiter.EBTEN", tmp_Arbiter_EBTEN);
    // Enables the inclusion of a weighted token arbitration scheme.
    // If this parameter is enabled, the arbiter slave interface has to
    // be connected to the AHB bus.
    // NOTE: this feature is not yet implemented.
    bool tmp_Arbiter_AHB_WTEN = false;
    ccss_set_parameter("Arbiter.AHB_WTEN", tmp_Arbiter_AHB_WTEN);
    // Enables the information trace output for pause mode changes.
    bool tmp_Arbiter_DoTrace = false;
    ccss_set_parameter("Arbiter.DoTrace", tmp_Arbiter_DoTrace);
    // The combinatoric arbitration mode is used for the interconnect
    // matrix (DW_ahb_icm_tlm). For regular AHB bus arbitration the value
    // must be set to false.
    bool tmp_Arbiter_CombinatoricMode = false;
    ccss_set_parameter("Arbiter.CombinatoricMode", tmp_Arbiter_CombinatoricMode);
    Arbiter = new DW_AHB_Arbiter("Arbiter");
    
    // Enables the information trace output for address mode changes.
    bool tmp_Decoder_DoTrace = false;
    ccss_set_parameter("Decoder.DoTrace", tmp_Decoder_DoTrace);
    Decoder = new DW_AHB_Decoder("Decoder");
    //BINDING
    Arbiter->pause(pause);
    Arbiter->ahb_wt_aps(ahb_wt_aps);
    Arbiter->ahb_wt_mask(ahb_wt_mask);
    Arbiter->ahbarbint(ahbarbint);
    Decoder->remap_n(hresetn);
    AHB_1->hresetn(hresetn);
    if (FREQSCALING || FREQSCALINGDEVICE)
      AHB_1->hclk(interconnect_clock[0]);  // FIXME [0]
    else
      AHB_1->hclk(ClockGen_1);
    AHB_1->arbiter_port(*Arbiter);
    AHB_1->decoder_port(*Decoder);
    
 //////////////////////////////    
//MODULI MASTER             //
/////////////////////////////        
    Master_ahb *mst[N_MASTERS];   
    for (i=0; i<N_MASTERS; i++)
    {
    sprintf(buffer, "mst%d", i);
    mst[i] = new Master_ahb(buffer //Name
                            ,i+1   //Master ID
                            ,1     //Priority
                            );
//BINDING   
    mst[i]->p(*AHB_1);
    if ((FREQSCALING && M_DIVIDER[i] != I_DIVIDER[MASTER_CONFIG[i].binding]) || FREQSCALINGDEVICE)
    {
      mst[i]->hclk(interconnect_clock[MASTER_CONFIG[i].binding]);
      mst[i]->ready_to_wrapper(readymast_interconnect[i]);
      mst[i]->request_from_wrapper(requestmast_interconnect[i]);
      mst[i]->pinout(pinoutmast_interconnect[i]);
    }
    else
    {
      mst[i]->hclk(ClockGen_1);
      mst[i]->ready_to_wrapper(readymast[i]);
      mst[i]->request_from_wrapper(requestmast[i]);
      mst[i]->pinout(pinoutmast[i]);
    }
    mst[i]->hresetn(hresetn);
    }

///////////////////////////////    
//MODULI SLAVE0             //
/////////////////////////////            
    Slave0_ahb *s0[N_PRIVATE + N_SHARED + N_SEMAPHORE];
    for (i=0; i<N_PRIVATE + N_SHARED + N_SEMAPHORE; i++)
    {
    bool tmp_s0_DoTrace = false;
    sprintf(buffer2, "s0%d.DoTrace", i);
    ccss_set_parameter(buffer2, tmp_s0_DoTrace);
    sprintf(buffer, "s0%d", i);
    if (i<N_PRIVATE)
      s0[i] = new Slave0_ahb(buffer, i*0x01000000,i);
    else if (i<N_PRIVATE + N_SHARED)
           s0[i] = new Slave0_ahb(buffer, 0x19000000+(i - N_PRIVATE)*0x00100000,i);
         else if (i<N_PRIVATE + N_SHARED + N_SEMAPHORE)
                s0[i] = new Slave0_ahb(buffer, 0x20000000+(i - N_PRIVATE - N_SHARED)*0x00004000,i);
//BINDING 
    AHB_1->slave_port(*s0[i]);
    }
    
///////////////////////////////    
//MODULI SLAVE1             //
///////////////////////////// 
    Slave1_ahb *s1[N_INTERRUPT];
    for (i=0; i<N_INTERRUPT; i++)
    {
    bool tmp_s1_DoTrace = false;
    sprintf(buffer2, "s1%d.DoTrace", i);
    ccss_set_parameter(buffer2, tmp_s1_DoTrace);
    sprintf(buffer, "s1%d",N_PRIVATE + N_SHARED + N_SEMAPHORE + i);
    s1[i] = new Slave1_ahb(buffer, 0x21000000+(i*0x00015000),N_PRIVATE + N_SHARED + N_SEMAPHORE +i);
//BINDING   
    AHB_1->slave_port(*s1[i]);
    for (j=0; j<N_CORES; j++)
      s1[i]->extint[j](extint[(j*NUMBER_OF_EXT)+i]);
    }
    
///////////////////////////////    
//MODULI SLAVE2             //
/////////////////////////////      
 //AMBA core-associated slaves. These slaves receive a scratchpad pointer as a parameter!
   if(CORESLAVE) 
   { 
   Slave2_ahb *s2[N_CORESLAVE];
   for (i=0; i<N_CORESLAVE; i++)
    {
    bool tmp_s2_DoTrace = false;
    sprintf(buffer2, "s2%d.DoTrace", i);
    ccss_set_parameter(buffer2, tmp_s2_DoTrace);
    sprintf(buffer, "s2%d", N_PRIVATE + N_SHARED + N_SEMAPHORE + N_INTERRUPT +i);
    s2[i] = new Slave2_ahb(buffer, 0x22000000+(i*0x00020000),addresser->CoreSlaveStartID() + i,datascratch[i], queue[i]);
//BINDING        
    AHB_1->slave_port(*s2[i]);
    }
   } 
     
///////////////////////////////    
//MODULO MONITOR            //
/////////////////////////////      
   #ifdef AHB_MONITOR
    bool tmp_Monitor_MonitorTrace = true;
    ccss_set_parameter("Monitor.MonitorTrace", tmp_Monitor_MonitorTrace);
    bool tmp_Monitor_SignalTrace = true;
    ccss_set_parameter("Monitor.SignalTrace", tmp_Monitor_SignalTrace);
    bool tmp_Monitor_StatisticsTrace = true;
    ccss_set_parameter("Monitor.StatisticsTrace", tmp_Monitor_StatisticsTrace);
    unsigned int tmp_Monitor_StatisticsInterval = 128;
    ccss_set_parameter("Monitor.StatisticsInterval", tmp_Monitor_StatisticsInterval);
    double tmp_Monitor_FileTraceStartTime = 0.0;
    ccss_set_parameter("Monitor.FileTraceStartTime", tmp_Monitor_FileTraceStartTime);
    double tmp_Monitor_FileTraceEndTime = -1.0;
    ccss_set_parameter("Monitor.FileTraceEndTime", tmp_Monitor_FileTraceEndTime);
    sc_time_unit tmp_Monitor_FileTraceTimeUnit = SC_NS;
    ccss_set_parameter("Monitor.FileTraceTimeUnit", tmp_Monitor_FileTraceTimeUnit);
    Monitor = new DW_AHB_Monitor("Monitor", "Monitor.txt", "Monitor_stat.txt");
    //BINDING
    Monitor->p(*AHB_1);
    #endif
   
///////////////////////////////    
//MODULO MONITOR-PIN-IF     //
/////////////////////////////         
    if(VCD){
    bool tmp_MonitorPinIF_SignalTrace = true;
    ccss_set_parameter("MonitorPinIF.SignalTrace", tmp_MonitorPinIF_SignalTrace);
    MonitorPinIF = new DW_AHB_MonitorPinIF("MonitorPinIF");
//BINDING     
    MonitorPinIF->hlock(*hlock);
    MonitorPinIF->hgrant(*hgrant);
    MonitorPinIF->hbusreq(*hbusreq);
    MonitorPinIF->hsplit_s15(hsplit_s[15]);
    MonitorPinIF->hsplit_s14(hsplit_s[14]);
    MonitorPinIF->hsplit_s13(hsplit_s[13]);
    MonitorPinIF->hsplit_s12(hsplit_s[12]);
    MonitorPinIF->hsplit_s11(hsplit_s[11]);
    MonitorPinIF->hsplit_s10(hsplit_s[10]);
    MonitorPinIF->hsplit_s9(hsplit_s[9]);
    MonitorPinIF->hsplit_s8(hsplit_s[8]);
    MonitorPinIF->hsplit_s7(hsplit_s[7]);
    MonitorPinIF->hsplit_s6(hsplit_s[6]);
    MonitorPinIF->hsplit_s5(hsplit_s[5]);
    MonitorPinIF->hsplit_s4(hsplit_s[4]);
    MonitorPinIF->hsplit_s3(hsplit_s[3]);
    MonitorPinIF->hsplit_s2(hsplit_s[2]);
    MonitorPinIF->hsplit_s1(hsplit_s[1]);
    MonitorPinIF->hsplit_s0(hsplit_s[0]);
    MonitorPinIF->hsel(*hsel);
    MonitorPinIF->hwrite(*hwrite);
    MonitorPinIF->hwdata(*hwdata);
    MonitorPinIF->htrans(*htrans);
    MonitorPinIF->hsize(*hsize);
    MonitorPinIF->hresp(*hresp);
    MonitorPinIF->hready(*hready);
    MonitorPinIF->hrdata(*hrdata);
    MonitorPinIF->hprot(*hprot);
    MonitorPinIF->hmastlock(*hmastlock);
    MonitorPinIF->hmaster(*hmaster);
    MonitorPinIF->hburst(*hburst);
    MonitorPinIF->haddr(*haddr);
    MonitorPinIF->p(*AHB_1);
    }
          
    armsystem *soc[N_CORES];
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
    
// DMA support
if (DMA)
  {
Amba_ahb_syn_Dma_control_scratch *dmacont[N_CORES];
Amba_ahb_syn_Dma_transfer_scratch *transf[N_CORES];
for (i=0; i<N_CORES; i++)
    {
      //instance a DMA controller
      sprintf(buffer,"dma%d",i);
      dmacont[i] = new
      Amba_ahb_syn_Dma_control_scratch(buffer,i,INTERNAL_MAX_OBJ,1,addresser->ReturnDMAPhysicalAddress());
      if (FREQSCALING || FREQSCALINGDEVICE)
        dmacont[i]->clock(init_clock[i]);
      else
        dmacont[i]->clock(ClockGen_1);
      
      //instance a DMA transfer module
      sprintf(buffer,"tra%d", i);
      transf[i] = new Amba_ahb_syn_Dma_transfer_scratch(buffer,i,INTERNAL_DIM_BURST,INTERNAL_MAX_OBJ,1,datascratch[i]);
      if (FREQSCALING || FREQSCALINGDEVICE)
        transf[i]->clock(init_clock[i]);
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
 
   hresetn.write(true); 
}


SharedAHB::SharedAHB(sc_module_name name_) 
        : sc_module(name_)      
       {
        bool _tmp_DoTrace = false;
        DoTrace.conditional_init(_tmp_DoTrace);
          
        InitInstances(); 
       }
