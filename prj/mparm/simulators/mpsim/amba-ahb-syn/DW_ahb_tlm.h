#ifndef __DW_ahb_tlm_h
#define __DW_ahb_tlm_h

#include <systemc.h>
#include <ccss_systemc.h>

#include "ahb_types.h"

#include "ahb_arbiter_if.h"
#include "ahb_bus_if.h"
#include "ahb_decoder_if.h"
#include "ahb_direct_if.h"
#include "ahb_master_port.h"
#include "ahb_monitor_if.h"
#include "ahb_slave_if.h"

#include "ahb_dw_misc.h"

template<class T> class DW_AHB_TL_Master;
class ahb_lite_dummy_arbiter;

#ifdef CCSS_USE_SC_CTOR
#define CCSS_INIT_MEMBERS_PREFIX : 
#undef CCSS_USE_SC_CTOR
#else
#define CCSS_INIT_MEMBERS_PREFIX , 
#endif

#ifndef SYNTHESIS
#define CCSS_INIT_MEMBERS  CCSS_INIT_MEMBERS_PREFIX \
    hclk("hclk") \
    , hresetn("hresetn") \
    , slave_port("slave_port") \
    , arbiter_port("arbiter_port") \
    , decoder_port("decoder_port")
#else
#define CCSS_INIT_MEMBERS 
#endif

// The AHB bus module is used to manage the communication between Master and Slave
// modules. Since only one master can have exclusive access to the bus, the module has to
// implement an arbitration scheme. For a modular approach, the arbiter is implemented
// as a separate slave module that provides services to the bus. 
// 
// The bus interface methods register all master requests at the positive clock edge, which is
// the time when the master modules are activated. The evaluation of the requests is
// executed at the negative clock edge, which is when the bus is sensitive. The evaluation
// is divided into the data phase and address phase (two pipeline stages).
// 
// The data phase first updates the pointer to the data where the slave should execute
// the request of the last address phase (slave method: set_data(...)). The pointer is not
// updated if the bus is in wait state. The bus model then polls and stores
// the response of the slave (slave method: response()). 
// 
// The address phase determines which slave is addressed and transmitts the address and
// number of bytes to the slave in advance of the data. The slave that is bound to the given
// address is determined by the address deocoder. Depending on read or write mode, the
// slave methods read(...) or write(...) are used. A so-called default slave is
// addressed if: 
// 
//     - no slave can be assigned to the address
//     - the bus is in idle mode
// 
// The status of the default slave is OKAY if no read or write operation was requested
// from the slave. If a read or write access is issued to the default slave, the status is
// ERROR.
// 
// During the address phase, all master requests are collected. If more than one request is
// registered by the bus, the bus grant is arbitrated. Arbitration is not performed when
// the previous bus access was locked by the granted master, or if the bus is in wait state. If no
// master requests the bus, one master is granted bus access by default; the default grant. The
// default-granted master does not have to react, but can request and initialize transactions in the
// same cycle. If no transaction is requested the bus is in idle mode.
template<class MIF = ahb_namespace::ahb_bus_if , class SIF = ahb_namespace::ahb_slave_if  >
class DW_ahb_tlm// Advanced High Performance TL Bus Model
	: public sc_module
	, public MIF
	, public ahb_namespace::ahb_monitor_if
{

public:
    // parameters

    // Identification number of the bus.
    CCSS_PARAMETER(int, BusID);

    // Specifies the priority of the AHB bus layer. This parameter
    // can be ignored for a single layer bus system.
    CCSS_PARAMETER(int, Priority);

    // Delay of the monitor update event after the negative clock edge.
    CCSS_PARAMETER(sc_time, MonitorDelay);

    // ports

    // The bus module is sensitive to the negative clock edge of this input signal.
    sc_in<bool> hclk;

    // The asynchronous reset signal is active LOW.
    sc_in<bool> hresetn;

    // The slave bus interface(s) are connected to this multi-port.
    sc_port<SIF, ahb_namespace::NUM_SLAVES> slave_port;

    // The arbiter interface is connected to this port.
    sc_port<ahb_namespace::ahb_arbiter_if,1> arbiter_port;

    // The decoder interface is connected to this port.
    sc_port<ahb_namespace::ahb_decoder_if,1> decoder_port;

    // initialize parameters
    virtual void InitParameters() {
        int _tmp_BusID = 0;
        BusID.conditional_init(_tmp_BusID);
        int _tmp_Priority = 0;
        Priority.conditional_init(_tmp_Priority);
        sc_time _tmp_MonitorDelay = SC_ZERO_TIME;
        MonitorDelay.conditional_init(_tmp_MonitorDelay);
    }

	SC_HAS_PROCESS(DW_ahb_tlm);

	// constructor
	DW_ahb_tlm(sc_module_name name_,        // bus module instance name
	           int address_bus_width_ = 32, // width of the address bus
	           int data_bus_width_ = 32);   // width of the data bus

	// destructor
	~DW_ahb_tlm();

	virtual void
	register_port(sc_port_base&,
	              const char*);

	// public interface methods

	virtual void
	request(int);                        // master ID

	virtual bool
	has_grant(int) const;                // master ID

	virtual void
	lock(int);                           // master ID

	virtual void
	unlock(int);                         // master ID

	virtual void
	busy(int);                           // master ID

	virtual void
	idle(int);                           // master ID

	virtual void
	end_request(int);                    // master ID

	virtual void
	init_transaction(int,                // master id
	                 ahb_namespace::ahb_hwrite,             // read mode
					 ahb_namespace::ahb_data_t,         // data exchange array
	                 ahb_namespace::ahb_addr_t,         // start address
	                 ahb_namespace::ahb_hburst,      // burst mode
	                 ahb_namespace::ahb_hsize,          // transfer size
	                 bool,               // NEW: unaligned access
	                 int);               // NEW: master domain information

	virtual void
	set_data(int);                       // master ID

	virtual void
	set_protection(int,                  // master ID
	               ahb_namespace::ahb_hprot);            // protection value

	virtual bool
	response(int,                        // master ID
	         ahb_namespace::ahb_hresp&) const;         // status struct

	// ARM V6 extensions

	virtual void
	set_byte_strobe(int,                 // caller id
	                const unsigned int*) // byte strobe line information
	{}

	virtual void
	priority(int,                        // master ID
	         int);                       // priority

	virtual int
	priority(int) const;                 // master ID

	virtual void
	set_default_master(int);             // master ID

	virtual bool
	burst_read(int,                      // master ID
	           unsigned int*,            // data buffer
	           ahb_namespace::ahb_addr_t,               // start address
	           int,                      // burst length
	           ahb_namespace::ahb_hburst,            // burst mode
	           ahb_namespace::ahb_hsize);               // transfer size

	virtual bool
	burst_write(int,                     // master ID
	            unsigned int*,           // data buffer
	            ahb_namespace::ahb_addr_t,              // start address
	            int,                     // burst length
	            ahb_namespace::ahb_hburst,           // burst mode
	            ahb_namespace::ahb_hsize);              // transfer size

	virtual bool
	direct_read(int,                     // master ID
	            ahb_namespace::ahb_addr_t,              // address
	            ahb_namespace::ahb_data_t,              // data pointer
	            int = 4);                // number of bytes

	virtual bool
	direct_write(int,                    // master ID
	             ahb_namespace::ahb_addr_t,             // address
	             ahb_namespace::ahb_data_t,             // data pointer
	             int = 4);               // number of bytes


	// Monitoring Methods

	// ID of the bus connected to
	virtual int
	bus_id() const;

	// instance name of the bus or ICM
	virtual const char*
	bus_name() const;

	// Width of the address bus
	virtual int
	bus_addr_width() const;

	// Width of the data bus
	virtual int
	bus_data_width() const;

	// Number of master ports connected to the bus
	virtual int
	num_masters() const;

	// Name of a master with given master ID
	virtual const char*
	master_name(int) const;          // Master ID (< NUM_MASTERS)

	// Is the master ID active (used)
	virtual bool
	master_is_used(int) const;            // Master ID

	// Number of slave interfaces connected to the bus
	virtual int
	num_slaves() const;

	// Name of a slave with given slave ID
	virtual const char*
	slave_name(int) const;                 // Slave ID (<= NUM_SLAVES)

	// Is the slave ID active (used)
	virtual bool
	slave_is_used(int) const;            // Slave ID


	// Monitoring Module Sensitivity


	virtual const sc_event&
	default_event() const;


	// AHB Master Driven Pseudo Signal Methods


	// Address driven by the master
	virtual ahb_namespace::ahb_addr_t
	m_address(int) const;            // Master ID

	// Transfer type
	virtual int
	m_transfer_type(int) const;      // Master ID

	// Write/Read transfer indication
	virtual bool
	m_write(int) const;              // Master ID

	// Size of a transfer
	virtual int
	m_size(int) const;               // Master ID

	// Burstmode
	virtual int
	m_burst(int) const;              // Master ID

	// Write data value
	virtual const ahb_namespace::ahb_data_t
	m_wdata(int) const;              // Master ID

	// Protection value
	virtual int
	m_prot(int) const;               // Master ID


	// AHB Slave Driven Pseudo Signal Methods


	// Slave ready response
	virtual int
	s_ready(int) const;              // Slave ID

	// Slave status response
	virtual int
	s_response(int) const;           // Slave ID

	// Read data value
	virtual const ahb_namespace::ahb_data_t
	s_rdata(int) const;              // Slave ID

	// Slave response to indicate that a split transfer should be
	// allowed to be re-attempted
	virtual int
	s_splitx(int) const;             // Slave ID


	// AHB Read Pseudo Signal Methods


	// Bus Address
	virtual ahb_namespace::ahb_addr_t
	address(int) const;              // Slave ID (used for ICM systems only)

	// Transfer type
	virtual int
	transfer_type(int) const;        // Slave ID (used for ICM systems only)

	// Write/Read transfer indication
	virtual bool
	write(int) const;                // Slave ID (used for ICM systems only)

	// Size of a transfer
	virtual int
	size(int) const;                 // Slave ID (used for ICM systems only)

	// Burstmode
	virtual int
	burst(int) const;                // Slave ID (used for ICM systems only)

	// Read data value
	virtual const ahb_namespace::ahb_data_t
	rdata(int) const;                // Master ID (used for ICM systems only)

	// Write data value
	virtual const ahb_namespace::ahb_data_t
	wdata(int) const;                // Slave ID (used for ICM systems only)

	// Ready response
	virtual int
	ready(int) const;                // Master ID

	// Status response
	virtual int
	response(int) const;             // Master ID (used for ICM systems only)

	// Protection value
	virtual int
	prot(int) const;                 // Slave ID (used for ICM systems only)

	// Bit representation of slaves indication that a split transfer
	// should be allowed to be re-attempted
	virtual int
	splitx(int) const;               // Master ID (used for ICM systems only)


	// AHB arbitration control pseudo signals


	// Master requesting bus access
	virtual bool
	m_busrequest(int) const;         // Master ID

	// Master requests locked transfers
	virtual bool
	m_lock(int) const;               // Master ID

	// Indicates that the current master is performing a locked
	// sequence of transfers
	virtual bool
	masterlocked(int) const;         // Slave ID (used for ICM systems only)

	// Bit representation of the master grant lines
	virtual bool
	grant(int) const;                // Master ID

	// ID of the master that owns the address bus
	virtual int
	addr_master_id(int) const;       // Slave ID (used for ICM systems only)

	// ID of the master that owns the data bus
	virtual int
	data_master_id(int) const;       // Slave ID (used for ICM systems only)

	// ID of the addressed slave (address phase)
	virtual int
	addr_slave_id(int) const;        // Master ID (used for ICM systems only)

	// ID of the data slave (data phase)
	virtual int
	data_slave_id(int) const;        // Master ID (used for ICM systems only)

	// ARM11 extensions:

	virtual const unsigned int*
	m_byte_strobe(int) const;             // Master ID

	virtual const unsigned int*
	byte_strobe(int) const;              // Slave ID (used for ICM systems only)

	virtual bool
	m_unaligned(int) const;              // Master ID

	virtual bool
	unaligned(int) const;                 // Slave ID (used for ICM systems only)

	virtual int
	m_domain(int) const;                  // Master ID

	virtual int
	domain(int) const;                   // Slave ID (used for ICM systems only)

protected:

	// private methods
	void
	end_of_elaboration();

	void
	initialize();

	void
	construct();

	void on_reset_event();
	void evaluation_phase();

	void
	address_slave();

	void
	set_arm11_info();

	void
	add_to_queue(int,
	             int*,
	             int&);

	void
	remove_from_queue(int,
	                  int*,
	                  int&);

	// private data members

	// event to notify connected monitor objects
	sc_event         _monitor_event;

	// array to collect all requests
	ahb_request*     _request_vec[ahb_namespace::NUM_MASTERS];
	ahb_namespace::ahb_request_arb* _request_vec_arb[ahb_namespace::NUM_MASTERS];
	ahb_namespace::ahb_data_t       _m_data[ahb_namespace::NUM_MASTERS];
	int              _num_registered_masters;
	int             _default_master_id;

	// array to collect split responses
	int              _split_req[ahb_namespace::NUM_MASTERS];
	int              _num_splits;

	// array to collect blocking transfers
	DW_AHB_TL_Master<ahb_namespace::ahb_master_port>* _master_tlm[ahb_namespace::NUM_MASTERS];

	// pointer to the address and data request
	int              _addr_req, _data_req;

	// store the width of the address and data pointer
	int              _address_width_bit;
	int              _data_width_bit;
	int              _data_width_word;

	SIF*             _slave_if_vec[ahb_namespace::NUM_SLAVES];
	SIF*             _default_slave;

	// pointer to the address and data slave
	SIF*             _addr_slave, * _data_slave;
	int              _addr_slave_id, _data_slave_id;

	// status of the data phase
	ahb_namespace::ahb_hresp       _status_addr, _status_data;
	bool             _ready_addr, _ready_data;

	// dummy data value
	ahb_namespace::ahb_data_t       _dummy_data;
	ahb_lite_dummy_arbiter* dummy_arb;

	// pointer to the data value
	ahb_namespace::ahb_data_t       _r_data;
	ahb_namespace::ahb_data_t       _w_data;
	ahb_htransf_state _transf_state;

	bool             _lock_transf;

	int              _splitx[ahb_namespace::NUM_MASTERS];

	// indication of the bus idle mode
	sc_signal<bool>  _reset;
	bool             _is_initialized;

	// bus handle
	int              _bus_id;
}; // end module DW_ahb_tlm
#undef CCSS_INIT_MEMBERS_PREFIX
#undef CCSS_INIT_MEMBERS

#endif
