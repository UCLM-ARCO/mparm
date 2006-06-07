#ifndef __AXI_SYN_h
#define __AXI_SYN_h

#include <systemc.h>
#include "axi_types.h"
#include "globals.h"
#include "stats.h"
#include "address.h"
#include "scratch_mem.h"
#include "amba_axi_dma_scratch.h"
#include "mem_class.h"
#include "Master_axi.h"
#include "Slave0_axi.h"
#include "Slave1_axi.h"
#include "Slave2_axi.h"
#include "DW_AXI_Arbiter.h"
#include "DW_AXI_Decoder.h"
#include "DW_AXI_IC.h"
#include "DW_AXI_Monitor.h"
#include "clock_tree.h"
#include "reset.h"
#include "freq_register.h"
#include "dual_clock_pinout_adapter.h"

#ifdef LXBUILD
 using namespace sc_dt;
 #include "AXI_initiator_lx.h"
 #include "ast_iss_wrapper.h"
#endif

#ifdef SWARMBUILD
#ifdef WITHOUT_OCP
  #include "wrapper.h"
  #include "Master_axi.h"
#else  
  #include "swarm_ocp_master_wrapper.h"
  #include "ocp_to_amba_axi_slave.h"
#endif
#endif

// structural constants, do not change
const int    TimeOut           = 100;
const double ClockFrequency    = 1.0e9;

//Per istanziare i monitor utili per DEBUG
//#define AXIDEBUG

template<bool NegEdge=true , bool Verbose=false >
class axi_syn
: public sc_module
{
char **new_envp;
public:

#ifdef LXBUILD
ASTISSWrapper **lx;
#endif

  ~axi_syn()
{
#ifdef LXBUILD
  if(CURRENT_ISS == LX)
  {
   for(int i=0;i<N_CORES;i++) delete lx[i];
  }   
#endif 
}
 

public:

    // parameters

    // Number of read/write byte lanes of the AXI IC
    // DataBusBytes is the width of the data bus signals
    // (Rdata/ Wdata) in units of bytes.
    // The same number must be used in the entire AXI system
    // DataBusBytes must be a power of 2.
    // Must satisfy: 1 <= DataBusBytes
    // Example: IC with 64 bit read/write data bus
    //          DataBusBytes = 8
    CCSS_PARAMETER(int, DataBusBytes);
          
    // Number of address lines of the IC
    // Must satisfy: NumAlines >= 1
    CCSS_PARAMETER(int, NumAlines);
  
    // Number of write data lines of the IC
    // Must satisfy: NumWlines >= 1
    CCSS_PARAMETER(int, NumWlines);

    // Number of read data lines of the IC
    // Must satisfy: NumRlines >= 1
    CCSS_PARAMETER(int, NumRlines);

    // Number of buffered response lines of the IC
    // Must satisfy: NumBlines >= 1
    CCSS_PARAMETER(int, NumBlines);

    // channels
    sc_signal<bool> Aresetn;
    // Clock signals and dividers
    sc_signal< bool >                       *init_clock;
    sc_signal< bool >                       *interconnect_clock;
    sc_signal< sc_uint<FREQSCALING_BITS> >  *init_div;
    sc_signal< sc_uint<FREQSCALING_BITS> >  *interconnect_div;
    //DMA
    sc_signal<PINOUT> *pinoutwrappertodma;
    sc_signal<bool> *readywrappertodma;
    sc_signal<bool> *requestwrappertodma;
    sc_signal<DMA_CONT_REG1> *datadmacontroltotransfer;
    sc_signal<bool> *requestcontroltotransfer;
    sc_signal<bool> *finishedtransfer;
    sc_signal<PINOUT> *spinoutscratchdma;
    sc_signal<bool> *sreadyscratchdma;
    sc_signal<bool> *srequestscratchdma;
    // module instances
    DW_AXI_Arbiter *AXI_Arbiter;
    DW_AXI_Decoder *AXI_Decoder;
    DW_AXI_IC<NegEdge, Verbose > *AXI_IC;
 
// initialize parameters
virtual void InitParameters(){
        int _tmp_DataBusBytes = 4;
        DataBusBytes.conditional_init(_tmp_DataBusBytes);
        int _tmp_NumAlines = 1;
        NumAlines.conditional_init(_tmp_NumAlines);
        int _tmp_NumWlines = 1;
        NumWlines.conditional_init(_tmp_NumWlines);
        int _tmp_NumRlines = 1;
        NumRlines.conditional_init(_tmp_NumRlines);
        int _tmp_NumBlines = 1;
        NumBlines.conditional_init(_tmp_NumBlines);
                             };

// create the schematic
virtual void InitInstances(){
    int i,j;
    char buffer[20];
#ifdef AXIDEBUG
    char buffer2[20];
    char buffer3[20];
#endif
    // Clock signals and dividers
    init_clock = new sc_signal< bool > [N_MASTERS];
    init_div = new sc_signal< sc_uint<FREQSCALING_BITS> > [N_MASTERS];
    interconnect_clock = new sc_signal< bool > [N_BUSES];
    interconnect_div = new sc_signal< sc_uint<FREQSCALING_BITS> > [N_BUSES];
    // System wires
    extint = new sc_signal<bool> [NUMBER_OF_EXT*N_CORES];
    pinoutmast = new sc_signal<PINOUT> [N_MASTERS];
    readymast = new sc_signal<bool> [N_MASTERS];
    requestmast = new sc_signal<bool> [N_MASTERS];
    pinoutmast_interconnect = new sc_signal<PINOUT> [N_MASTERS];
    readymast_interconnect = new sc_signal<bool> [N_MASTERS];
    requestmast_interconnect = new sc_signal<bool> [N_MASTERS];
  
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
    AXI_Arbiter = new DW_AXI_Arbiter("AXI_Arbiter");
    AXI_Decoder = new DW_AXI_Decoder("AXI_Decoder");
    AXI_IC = new DW_AXI_IC<NegEdge, Verbose >("AXI_IC", 0, 1, 1, NumAlines, NumWlines, NumRlines, NumBlines,"", 31, 0x600000, 0x607FFF, "", false, true, DataBusBytes, 4, ClockFrequency, 100, false);
//BINDING 
    AXI_IC->Aresetn(Aresetn);
    if (FREQSCALING || FREQSCALINGDEVICE)
      AXI_IC->Aclk(interconnect_clock[0]); // FIXME [0]
    else
      AXI_IC->Aclk(ClockGen_1);
    AXI_IC->ArbiterP(*AXI_Arbiter);
    AXI_IC->DecoderP(*AXI_Decoder);

    
//////////////////////////////    
//MODULI MASTER             //
/////////////////////////////        

// CORE LX 
#ifdef LXBUILD

#ifdef WITH_INI_DEBUG
  #define INI_DEBUG true
#else
  #define INI_DEBUG false
#endif
     
      AXIBus_initiator_lx< uint, uint, uchar, 32, 16, 0, true, INI_DEBUG >  *init0_lx=NULL;
      AXIBus_initiator_lx< uint, uint, uchar, 32, 16, 1, true, INI_DEBUG >  *init1_lx=NULL;
      AXIBus_initiator_lx< uint, uint, uchar, 32, 16, 2, true, INI_DEBUG >  *init2_lx=NULL;
      AXIBus_initiator_lx< uint, uint, uchar, 32, 16, 3, true, INI_DEBUG >  *init3_lx=NULL;
      AXIBus_initiator_lx< uint, uint, uchar, 32, 16, 4, true, INI_DEBUG >  *init4_lx=NULL;
      AXIBus_initiator_lx< uint, uint, uchar, 32, 16, 5, true, INI_DEBUG >  *init5_lx=NULL;
      AXIBus_initiator_lx< uint, uint, uchar, 32, 16, 6, true, INI_DEBUG >  *init6_lx=NULL;
      AXIBus_initiator_lx< uint, uint, uchar, 32, 16, 7, true, INI_DEBUG >  *init7_lx=NULL;
      AXIBus_initiator_lx< uint, uint, uchar, 32, 16, 8, true, INI_DEBUG >  *init8_lx=NULL;
      AXIBus_initiator_lx< uint, uint, uchar, 32, 16, 9, true, INI_DEBUG >  *init9_lx=NULL;
      AXIBus_initiator_lx< uint, uint, uchar, 32, 16, 10, true, INI_DEBUG > *init10_lx=NULL;
      AXIBus_initiator_lx< uint, uint, uchar, 32, 16, 11, true, INI_DEBUG > *init11_lx=NULL;
      AXIBus_initiator_lx< uint, uint, uchar, 32, 16, 12, true, INI_DEBUG > *init12_lx=NULL;
      AXIBus_initiator_lx< uint, uint, uchar, 32, 16, 13, true, INI_DEBUG > *init13_lx=NULL;
      AXIBus_initiator_lx< uint, uint, uchar, 32, 16, 14, true, INI_DEBUG > *init14_lx=NULL;
      AXIBus_initiator_lx< uint, uint, uchar, 32, 16, 15, true, INI_DEBUG > *init15_lx=NULL;
      AXIBus_initiator_lx< uint, uint, uchar, 32, 16, 16, true, INI_DEBUG > *init16_lx=NULL;
      AXIBus_initiator_lx< uint, uint, uchar, 32, 16, 17, true, INI_DEBUG > *init17_lx=NULL;
      
#include "initiator_inst_AXIlx.cpp"

#endif

#ifdef SWARMBUILD
//CORE ARM
    Master_axi *AXI_Master[N_MASTERS];
#ifdef AXIDEBUG
    DW_AXI_Monitor<32 > *AXI_MonitorMaster[N_MASTERS];
#endif
    if(CURRENT_ISS == SWARM){
    for (i=0; i<N_MASTERS; i++)
    {
    sprintf(buffer, "AXI_Master%d", i);
    AXI_Master[i] = new Master_axi(buffer, i, 1, DataBusBytes, ClockFrequency);
#ifdef AXIDEBUG
    sprintf(buffer2,"AXI_MonitorMaster%d" , i);
    sprintf(buffer3,"AXI_Master%d.txt" , i);
    AXI_MonitorMaster[i] = new DW_AXI_Monitor<32 >(buffer2, buffer3);
#endif
//BINDING
    AXI_Master[i]->Aresetn(Aresetn);
#ifdef AXIDEBUG
    AXI_MonitorMaster[i]->SlaveP(*AXI_Master[i]);
    AXI_Master[i]->MasterP(*AXI_MonitorMaster[i]);
    AXI_MonitorMaster[i]->MasterP(*AXI_IC);
    AXI_IC->SlaveP(*AXI_MonitorMaster[i]);
#else
    AXI_Master[i]->MasterP(*AXI_IC);
    AXI_IC->SlaveP(*AXI_Master[i]);
#endif
    if ((FREQSCALING && M_DIVIDER[i] != I_DIVIDER[MASTER_CONFIG[i].binding]) || FREQSCALINGDEVICE)
    {
      AXI_Master[i]->Aclk(interconnect_clock[MASTER_CONFIG[i].binding]);
      AXI_Master[i]->ready_to_wrapper(readymast_interconnect[i]);
      AXI_Master[i]->request_from_wrapper(requestmast_interconnect[i]);
      AXI_Master[i]->pinout(pinoutmast_interconnect[i]);
    }
    else
    {
      AXI_Master[i]->Aclk(ClockGen_1);
      AXI_Master[i]->ready_to_wrapper(readymast[i]);
      AXI_Master[i]->request_from_wrapper(requestmast[i]);
      AXI_Master[i]->pinout(pinoutmast[i]);
    }
    }
    }
#endif   
    
///////////////////////////////    
//MODULI SLAVE0             //
/////////////////////////////    
    Slave0_axi *AXI_Slave0[N_PRIVATE + N_LX_PRIVATE + N_SHARED + N_SEMAPHORE];

#ifdef AXIDEBUG
    DW_AXI_Monitor<32 > *AXI_MonitorSlave0[N_PRIVATE + N_LX_PRIVATE + N_SHARED + N_SEMAPHORE];
#endif
    for (i=0; i<N_PRIVATE + N_LX_PRIVATE + N_SHARED + N_SEMAPHORE; i++)
    {
#ifdef AXIDEBUG
    sprintf(buffer2,"AXI_MonitorSlave0%d" , i);
    sprintf(buffer3,"AXI_Slave0%d.txt" , i);
    AXI_MonitorSlave0[i] = new DW_AXI_Monitor<32 >(buffer2, buffer3);
#endif
    sprintf(buffer, "AXI_Slave0%d", i);
    if (i<N_PRIVATE + N_LX_PRIVATE){
#ifdef LXBUILD
    if(CURRENT_ISS == LX)
    AXI_Slave0[i] = new Slave0_axi(buffer, i,0x08000000+i*0x01000000,0x08000000+i*0x01000000+0xffffff, false, true, DataBusBytes, ClockFrequency,MEM_IN_WS);
#endif
#ifdef SWARMBUILD
    if(CURRENT_ISS == SWARM)
    AXI_Slave0[i] = new Slave0_axi(buffer, i,i*0x01000000,i*0x01000000+0x00bfffff, false, true, DataBusBytes, ClockFrequency,MEM_IN_WS);
#endif
    }
    else if (i<N_PRIVATE + N_LX_PRIVATE + N_SHARED)   
    AXI_Slave0[i] = new Slave0_axi(buffer, i,0x19000000+(i - N_PRIVATE - N_LX_PRIVATE)*0x00100000,0x19000000+(i - N_PRIVATE - N_LX_PRIVATE)*0x00100000+0x00100000, false, true, DataBusBytes, ClockFrequency,MEM_IN_WS);    
    else if (i<N_PRIVATE + N_LX_PRIVATE + N_SHARED + N_SEMAPHORE)   
    AXI_Slave0[i] = new Slave0_axi(buffer, i,0x20000000+(i - N_PRIVATE - N_LX_PRIVATE - N_SHARED)*0x00004000,0x20000000+(i - N_PRIVATE - N_LX_PRIVATE - N_SHARED)*0x00004000+0x00004000, false, true, DataBusBytes, ClockFrequency,MEM_IN_WS);    
//BINDING
    if (FREQSCALING || FREQSCALINGDEVICE)
      AXI_Slave0[i]->Aclk(interconnect_clock[SLAVE_CONFIG[i].binding]);
    else
      AXI_Slave0[i]->Aclk(ClockGen_1);
    AXI_Slave0[i]->Aresetn(Aresetn);
#ifdef AXIDEBUG 
    AXI_MonitorSlave0[i]->SlaveP(*AXI_IC);
    AXI_IC->MasterP(*AXI_MonitorSlave0[i]);
    AXI_MonitorSlave0[i]->MasterP(*AXI_Slave0[i]);
    AXI_Slave0[i]->SlaveP(*AXI_MonitorSlave0[i]);
#else
    AXI_Slave0[i]->SlaveP(*AXI_IC);
    AXI_IC->MasterP(*AXI_Slave0[i]);
#endif
    }
    
///////////////////////////////    
//MODULI SLAVE1             //
/////////////////////////////      
    Slave1_axi *AXI_Slave1[N_INTERRUPT];
#ifdef AXIDEBUG
    DW_AXI_Monitor<32 > *AXI_MonitorSlave1[N_INTERRUPT];
#endif
    for (i=0; i<N_INTERRUPT; i++)
    {
#ifdef AXIDEBUG
    sprintf(buffer2,"AXI_MonitorSlave1%d" , i);
    sprintf(buffer3,"AXI_Slave1%d.txt" , i);
    AXI_MonitorSlave1[i] = new DW_AXI_Monitor<32 >(buffer2, buffer3);
#endif
    sprintf(buffer, "AXI_Slave1%d", i);
    AXI_Slave1[i] = new Slave1_axi(buffer, addresser->InterruptStartID() + i,0x21000000+i*0x00015000,0x21000000+(i*0x00015000)+(0x00014fff), false, true, DataBusBytes, ClockFrequency,INT_WS);
//BINDING   
    if (FREQSCALING || FREQSCALINGDEVICE)
      AXI_Slave1[i]->Aclk(interconnect_clock[SLAVE_CONFIG[addresser->InterruptStartID() + i].binding]);
    else
      AXI_Slave1[i]->Aclk(ClockGen_1);
    AXI_Slave1[i]->Aresetn(Aresetn);
#ifdef AXIDEBUG 
    AXI_MonitorSlave1[i]->SlaveP(*AXI_IC);
    AXI_IC->MasterP(*AXI_MonitorSlave1[i]);
    AXI_MonitorSlave1[i]->MasterP(*AXI_Slave1[i]);
    AXI_Slave1[i]->SlaveP(*AXI_MonitorSlave1[i]);
#else
    AXI_Slave1[i]->SlaveP(*AXI_IC);
    AXI_IC->MasterP(*AXI_Slave1[i]);
#endif
    for (j=0; j<N_CORES; j++)
      AXI_Slave1[i]->extint[j](extint[j*NUMBER_OF_EXT + i]);
    }

///////////////////////////////    
//MODULI SLAVE2             //
/////////////////////////////          
//AMBA core-associated slaves. These slaves receive a scratchpad pointer as a parameter!
if(CORESLAVE) 
    {
    Slave2_axi *AXI_Slave2[N_CORESLAVE];
#ifdef AXIDEBUG
    DW_AXI_Monitor<32 > *AXI_MonitorSlave2[N_CORESLAVE];
#endif
    for (i=0; i<N_CORESLAVE; i++)
    {
#ifdef AXIDEBUG
    sprintf(buffer2,"AXI_MonitorSlave2%d" , i);
    sprintf(buffer3,"AXI_Slave2%d.txt" , i);
    AXI_MonitorSlave2[i] = new DW_AXI_Monitor<32 >(buffer2, buffer3);
#endif
    sprintf(buffer, "AXI_Slave2%d", i);
    AXI_Slave2[i] = new Slave2_axi(buffer, N_PRIVATE + N_LX_PRIVATE + N_SHARED + N_SEMAPHORE + N_INTERRUPT+i,0x22000000+i*0x00020000,(0x22000000+i*0x00020000)+0x00014fff, false, true, DataBusBytes, ClockFrequency,datascratch[i], queue[i],1); 
//BINDING    
    if (FREQSCALING || FREQSCALINGDEVICE)
      AXI_Slave2[i]->Aclk(interconnect_clock[SLAVE_CONFIG[addresser->CoreSlaveStartID() + i].binding]);
    else
      AXI_Slave2[i]->Aclk(ClockGen_1);
    AXI_Slave2[i]->Aresetn(Aresetn);
#ifdef AXIDEBUG 
    AXI_MonitorSlave2[i]->SlaveP(*AXI_IC);
    AXI_IC->MasterP(*AXI_MonitorSlave2[i]);
    AXI_MonitorSlave2[i]->MasterP(*AXI_Slave2[i]);
    AXI_Slave2[i]->SlaveP(*AXI_MonitorSlave2[i]);
#else
    AXI_Slave2[i]->SlaveP(*AXI_IC);
    AXI_IC->MasterP(*AXI_Slave2[i]);
#endif    
    }
    
    } 
        
#ifdef LXBUILD
 if(CURRENT_ISS == LX)
 {
 lx=new ASTISSWrapper*[N_CORES];
 for (i=0; i<N_CORES; i++)
    {
     sprintf(buffer, "LX_%d", i);
	    char targetFilename[40];
	    sprintf(targetFilename, "my_appli%d.elf",i);
	    char* execPath="$SWARMDIR/bin/mpsim.x";
	    char* statPath=".";   

	    std::vector<std::string> fakeArguments;
   	    fakeArguments.push_back(targetFilename);
            
lx[i] = new ASTISSWrapper(	buffer,  //INVARIATO
   				execPath,		//nome di mparm
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
	      
            for (j = 0; j < NUMBER_OF_EXT; j ++)
            lx[i]->irq_port(extint[i*NUMBER_OF_EXT+j], 32+j);
  }
  
  switch(N_CORES){

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
#endif

#ifdef SWARMBUILD
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
 }
#endif
    
// DMA support
  if (DMA)
  {
  Amba_axi_Dma_control_scratch *dmacont[N_CORES];
  Amba_axi_Dma_transfer_scratch *transf[N_CORES];

    for (i=0; i<N_CORES; i++)
    {
      //instance a DMA controller
      sprintf(buffer,"dma%d",i);
      dmacont[i] = new
      Amba_axi_Dma_control_scratch(buffer,i,INTERNAL_MAX_OBJ,1,addresser->ReturnDMAPhysicalAddress());
      if (FREQSCALING || FREQSCALINGDEVICE)
        dmacont[i]->clock(init_clock[i]);
      else
        dmacont[i]->clock(ClockGen_1);
      
      //instance a DMA transfer module
      sprintf(buffer,"tra%d", i);
      transf[i] = new Amba_axi_Dma_transfer_scratch(buffer,i,INTERNAL_DIM_BURST,INTERNAL_MAX_OBJ,1,datascratch[i]);
      if (FREQSCALING || FREQSCALINGDEVICE)
        transf[i]->clock(init_clock[i]);
      else
        transf[i]->clock(ClockGen_1);

      //connect this wrapper to the DMA controller
#if defined WITHOUT_OCP && defined SWARMBUILD
 if(CURRENT_ISS == SWARM)    
     {
      soc[i]->pinout_ft_master[1](pinoutwrappertodma[i]);
      soc[i]->ready_from_master[1](readywrappertodma[i]);
      soc[i]->request_to_master[1](requestwrappertodma[i]);
     }
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
    
   Aresetn.write(true); 

};

SC_HAS_PROCESS(axi_syn);
 
  axi_syn(sc_module_name nm,char *_new_envp[]): 
       new_envp(_new_envp)
      {
      
       axi_syn::InitParameters();
       // process declarations
       axi_syn::InitInstances();
      }

}; // end module axi_syn


#endif


