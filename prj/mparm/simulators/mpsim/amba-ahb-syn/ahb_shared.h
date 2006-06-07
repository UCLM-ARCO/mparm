#ifndef __ahb_shared_h
#define __ahb_shared_h

#include <systemc.h>
#include <ccss_systemc.h>

#include "globals.h"
#include "ahb_master_port.h"
#include "config.h"
#include "ahb_types.h"
#include "ahb_slave_if.h"
#include "DW_ahb_tlm.h"
#include "DW_AHB_Arbiter.h"
#include "DW_AHB_Decoder.h"
#include "DW_AHB_Monitor.h"
#include "DW_AHB_MonitorPinIF.h"
#include "DW_AHB_Memory.h"
#include "Master_ahb_syn.h"
#include "Slave0_ahb.h"
#include "Slave1_ahb.h"
#include "Slave2_ahb.h"
#include "mem_class.h"
#include "scratch_mem.h"
#include "amba_ahb_syn_dma_scratch.h"
#include "stats.h"
#include "address.h"

#ifdef WITHOUT_OCP
  #include "wrapper.h"
  #include "Master_ahb_syn.h"
#else
  #include "swarm_ocp_master_wrapper.h"
  #include "ocp_to_amba_sig_slave.h"
#endif

class SharedAHB
: public sc_module
{

public:
    // parameters
    CCSS_PARAMETER(bool, DoTrace);

    // channels
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
    //Arbiter
    sc_signal<bool> ahb_wt_aps;
    sc_signal<sc_bv<ahb_namespace::NUM_MASTERS> > ahb_wt_mask;
    sc_signal<bool> ahbarbint;
    //Generals
    sc_signal<bool> hresetn;
    sc_signal<bool> pause;
    //MonitorPinIF
    sc_signal<sc_bv<16> > *hlock;
    sc_signal<sc_bv<16> > *hgrant;
    sc_signal<sc_bv<16> > *hbusreq;
    sc_signal<sc_bv<16> > *hsplit_s;
    sc_signal<sc_bv<16> > *hsel;
    sc_signal<bool > *hwrite;
    sc_signal<sc_bv<1024> > *hwdata;
    sc_signal<sc_bv<2> > *htrans;
    sc_signal<sc_bv<3> > *hsize;
    sc_signal<sc_bv<2> > *hresp;
    sc_signal<bool > *hready;
    sc_signal<sc_bv<1024> > *hrdata;
    sc_signal<sc_bv<4> > *hprot;
    sc_signal<bool > *hmastlock;
    sc_signal<sc_bv<4> > *hmaster;
    sc_signal<sc_bv<3> > *hburst;
    sc_signal<sc_bv<64> > *haddr;    
    

    // module instances
    DW_ahb_tlm<ahb_namespace::ahb_bus_if, ahb_namespace::ahb_slave_if > *AHB_1;
    DW_AHB_Arbiter *Arbiter;
    DW_AHB_Decoder *Decoder;
    DW_AHB_Monitor *Monitor;
    DW_AHB_MonitorPinIF *MonitorPinIF;
    
 void InitInstances();
     
 SC_HAS_PROCESS(SharedAHB);
 
 SharedAHB(sc_module_name);
 
}; // end module ahb_syn

#endif
