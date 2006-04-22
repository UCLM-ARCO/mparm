#ifndef __DW_AHB_MonitorPinIF_h
#define __DW_AHB_MonitorPinIF_h

#include <systemc.h>
#include <ccss_systemc.h>

// include the declarations of the bus monitor interface
#include "ahb_monitor_if.h"

#ifdef CCSS_USE_SC_CTOR
#define CCSS_INIT_MEMBERS_PREFIX : 
#undef CCSS_USE_SC_CTOR
#else
#define CCSS_INIT_MEMBERS_PREFIX , 
#endif

#ifndef SYNTHESIS
#define CCSS_INIT_MEMBERS  CCSS_INIT_MEMBERS_PREFIX \
    p("p") \
    , haddr("haddr") \
    , hburst("hburst") \
    , hmaster("hmaster") \
    , hmastlock("hmastlock") \
    , hprot("hprot") \
    , hrdata("hrdata") \
    , hready("hready") \
    , hresp("hresp") \
    , hsize("hsize") \
    , htrans("htrans") \
    , hwdata("hwdata") \
    , hwrite("hwrite") \
    , hsel("hsel") \
    , hsplit_s0("hsplit_s0") \
    , hsplit_s1("hsplit_s1") \
    , hsplit_s2("hsplit_s2") \
    , hsplit_s3("hsplit_s3") \
    , hsplit_s4("hsplit_s4") \
    , hsplit_s5("hsplit_s5") \
    , hsplit_s6("hsplit_s6") \
    , hsplit_s7("hsplit_s7") \
    , hsplit_s8("hsplit_s8") \
    , hsplit_s9("hsplit_s9") \
    , hsplit_s10("hsplit_s10") \
    , hsplit_s11("hsplit_s11") \
    , hsplit_s12("hsplit_s12") \
    , hsplit_s13("hsplit_s13") \
    , hsplit_s14("hsplit_s14") \
    , hsplit_s15("hsplit_s15") \
    , hbusreq("hbusreq") \
    , hgrant("hgrant") \
    , hlock("hlock")
#else
#define CCSS_INIT_MEMBERS 
#endif

// This monitor module implements an external monitor that creates
// pseudo signal values that are written to the output ports. The
// monitor can be connected to the AHB_Bus module instance, and generates
// signal values on demand. The pseudo signal values are generated
// by the bus module interface method calls.
// 
// The names of the pseudo signal pins conform to the AMBA (TM)
// Specification Rev 2.0, pages 2-3 to 2-5.
// 
// For more information on the pin interface see the port descriptions
// of the module.
class DW_AHB_MonitorPinIF// An AHB_Bus Monitor Module with Pin Interface
	: public sc_module
{

public:
    // parameters

    // Generate sc_signal obtained by the bus monitor API. 
    CCSS_PARAMETER(bool, SignalTrace);

    // ports

    // The bus monitor interface is connected to this port. 
    sc_port<ahb_namespace::ahb_monitor_if, 1> p;

    // The 32 (64) -bit system address bus.
    sc_out<sc_bv<64> > haddr;

    // Indicates if the transfer forms a part of a burst. Four,
    // eight and sixteen beat bursts are supported and the
    // burst may be either incrementing or wrapping
    sc_out<sc_bv<3> > hburst;

    // These signals from the arbiter indicate which bus master is
    // currently performing a transfer, and are used by the slaves
    // which support SPLIT transfers to determine which master
    // is attempting an access.
    // The timing of HMASTER is aligned with the timing of the
    // address and control signals.
    sc_out<sc_bv<4> > hmaster;

    // Indicates that the current master is performing a locked
    // sequence of transfers. This signal has the same timing as the
    // HMASTER signal.
    sc_out<bool> hmastlock;

    // The protection control signal provides additional
    // information about a bus access.
    sc_out<sc_bv<4> > hprot;

    // The read data bus is used to transfer data from bus
    // slaves to the bus master during read operations.
    sc_out<sc_bv<1024> > hrdata;

    // When HIGH, the HREADY signal indicates that a
    // transfer has finished on the bus. This signal may
    // be driven LOW to extend a transfer.
    sc_out<bool> hready;

    // The transfer response provides additional
    // information on the status of a transfer.
    // Four different responses are provided, OKAY,
    // ERROR, READY and SPLIT.
    sc_out<sc_bv<2> > hresp;

    // Indicates the size of the transfer, which is typically
    // byte (8-bit), halfword (16-bit) or word (32-bit). The
    // protocol allows for larger transfer sizes up to a maximum
    // of 1024 bits.
    sc_out<sc_bv<3> > hsize;

    // Indicates the type of the current transfer, which can be
    // NONSEQUENTIAL, SEQUENTIAL, IDLE or BUSY.
    sc_out<sc_bv<2> > htrans;

    // The write data bus is used to transfer data from the
    // master to the bus slave during write operations.
    sc_out<sc_bv<1024> > hwdata;

    // When HIGH this signal indicates a write transfer
    // and when LOW a read transfer.
    sc_out<bool> hwrite;

    // Each AHB slave has its own slave select signal and
    // this signal indicates that the current transfer is
    // intended for the selected slave.
    sc_out<sc_bv<16> > hsel;

    // This signal is used by a slave to indicate to the 
    // arbiter which bus masters should be allowed to re-attempt a
    // split transaction. Each bit of this split signal corresponds to
    // a single bus master.
    sc_out<sc_bv<16> > hsplit_s0;

    // This signal is used by a slave to indicate to the 
    // arbiter which bus masters should be allowed to re-attempt a
    // split transaction. Each bit of this split signal corresponds to
    // a single bus master.
    sc_out<sc_bv<16> > hsplit_s1;

    // This signal is used by a slave to indicate to the 
    // arbiter which bus masters should be allowed to re-attempt a
    // split transaction. Each bit of this split signal corresponds to
    // a single bus master.
    sc_out<sc_bv<16> > hsplit_s2;

    // This signal is used by a slave to indicate to the 
    // arbiter which bus masters should be allowed to re-attempt a
    // split transaction. Each bit of this split signal corresponds to
    // a single bus master.
    sc_out<sc_bv<16> > hsplit_s3;

    // This signal is used by a slave to indicate to the 
    // arbiter which bus masters should be allowed to re-attempt a
    // split transaction. Each bit of this split signal corresponds to
    // a single bus master.
    sc_out<sc_bv<16> > hsplit_s4;

    // This signal is used by a slave to indicate to the 
    // arbiter which bus masters should be allowed to re-attempt a
    // split transaction. Each bit of this split signal corresponds to
    // a single bus master.
    sc_out<sc_bv<16> > hsplit_s5;

    // This signal is used by a slave to indicate to the 
    // arbiter which bus masters should be allowed to re-attempt a
    // split transaction. Each bit of this split signal corresponds to
    // a single bus master.
    sc_out<sc_bv<16> > hsplit_s6;

    // This signal is used by a slave to indicate to the 
    // arbiter which bus masters should be allowed to re-attempt a
    // split transaction. Each bit of this split signal corresponds to
    // a single bus master.
    sc_out<sc_bv<16> > hsplit_s7;

    // This signal is used by a slave to indicate to the 
    // arbiter which bus masters should be allowed to re-attempt a
    // split transaction. Each bit of this split signal corresponds to
    // a single bus master.
    sc_out<sc_bv<16> > hsplit_s8;

    // This signal is used by a slave to indicate to the 
    // arbiter which bus masters should be allowed to re-attempt a
    // split transaction. Each bit of this split signal corresponds to
    // a single bus master.
    sc_out<sc_bv<16> > hsplit_s9;

    // This signal is used by a slave to indicate to the 
    // arbiter which bus masters should be allowed to re-attempt a
    // split transaction. Each bit of this split signal corresponds to
    // a single bus master.
    sc_out<sc_bv<16> > hsplit_s10;

    // This signal is used by a slave to indicate to the 
    // arbiter which bus masters should be allowed to re-attempt a
    // split transaction. Each bit of this split signal corresponds to
    // a single bus master.
    sc_out<sc_bv<16> > hsplit_s11;

    // This signal is used by a slave to indicate to the 
    // arbiter which bus masters should be allowed to re-attempt a
    // split transaction. Each bit of this split signal corresponds to
    // a single bus master.
    sc_out<sc_bv<16> > hsplit_s12;

    // This signal is used by a slave to indicate to the 
    // arbiter which bus masters should be allowed to re-attempt a
    // split transaction. Each bit of this split signal corresponds to
    // a single bus master.
    sc_out<sc_bv<16> > hsplit_s13;

    // This signal is used by a slave to indicate to the 
    // arbiter which bus masters should be allowed to re-attempt a
    // split transaction. Each bit of this split signal corresponds to
    // a single bus master.
    sc_out<sc_bv<16> > hsplit_s14;

    // This signal is used by a slave to indicate to the 
    // arbiter which bus masters should be allowed to re-attempt a
    // split transaction. Each bit of this split signal corresponds to
    // a single bus master.
    sc_out<sc_bv<16> > hsplit_s15;

    // A signal from bus master with the ID x to the bus arbiter which
    // indicates that the bus master requires the bus. The signals are 
    // collected into a single bit vector.
    sc_out<sc_bv<16> > hbusreq;

    // This signal indicates that the bus master with ID x is currently the
    // highest prioirty master. Ownership of the address/control signals
    // changes at the end of a transfer when HREADY is HIGH, so a
    // master gets access to the bus when both HREADY and HGRANTx
    // are HIGH. The signals are collected into a
    // single bit vector. 
    sc_out<sc_bv<16> > hgrant;

    // When HIGH, this signal indicates that the master requires
    // locked access to the bus and no other master should be
    // granted the bus until this signal is LOW. The signal are
    // collected into a single bit vector.  
    sc_out<sc_bv<16> > hlock;

    // initialize parameters
    virtual void InitParameters() {
        bool _tmp_SignalTrace = true;
        SignalTrace.conditional_init(_tmp_SignalTrace);
    }

	SC_HAS_PROCESS(DW_AHB_MonitorPinIF);

	DW_AHB_MonitorPinIF(sc_module_name name_);

	~DW_AHB_MonitorPinIF();

protected:

	virtual void end_of_elaboration();

	sc_bv<1024> tmp_data;

	// process
	void main_action();

	int addr_width_dw;
	int data_width_dw;

}; // end module DW_AHB_MonitorPinIF
#undef CCSS_INIT_MEMBERS_PREFIX
#undef CCSS_INIT_MEMBERS


#endif
