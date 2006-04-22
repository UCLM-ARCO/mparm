#ifndef __DW_AHB_Arbiter_h
#define __DW_AHB_Arbiter_h

#include <systemc.h>
#include <ccss_systemc.h>

#include "ahb_arbiter_if.h"
#include "ahb_slave_if.h"

#ifdef CCSS_USE_SC_CTOR
#define CCSS_INIT_MEMBERS_PREFIX : 
#undef CCSS_USE_SC_CTOR
#else
#define CCSS_INIT_MEMBERS_PREFIX , 
#endif

#ifndef SYNTHESIS
#define CCSS_INIT_MEMBERS  CCSS_INIT_MEMBERS_PREFIX \
    pause("pause") \
    , ahbarbint("ahbarbint") \
    , ahb_wt_mask("ahb_wt_mask") \
    , ahb_wt_aps("ahb_wt_aps")
#else
#define CCSS_INIT_MEMBERS 
#endif

// The bus arbiter ensures that only one bus master at a
// time is granted exclusive bus access. The arbiter has access to
// the request structure (ahb_krequest) that stores the request and
// status information of all the masters connected to the bus. For more
// details see the ahb_request section.
// 
// The grant scheme for this arbiter is priority based. Each master
// instance provides a priority number. The master with the highest
// priority that requests bus access is given the grant.
// 
// The arbiter module has two constructors. The first constructor allows
// to specify one memory region that is the same for the boot and the
// normal address modes. The first constructor has the following arguments:
// 
// - sc_module_name, instance name of the module
// 
// - ahb_addr_t, start address of the slave interface
// 
// A second constructor can be used to specify to use different address
// spaces for boot and normal mode.
// 
// - ahb_addr_t, end address of the memory
// 
// The address range is 1k.
// 
// Note: if the slave interface of the arbiter is not connected to the
// bus system the address specifications for the arbiter slave interface
// are unused and ignored.
// 
// 
// Arbiter Slave Interface
// -----------------------
// 
// The arbiter provides a slave interface that allows masters to access and
// modify the priority registers during runtime. The slave interface may be
// used in addition to the arbiter interface. The slave interface can be left
// unbound. The table below gives an overview of the registers.
// 
// +-------------------------------------------------------------+
// | Address |     Type | Width | Description                    |
// | Offset  |          | [bit] |                                |
// |-------------------------------------------------------------|
// |    0x00 |     r/w  |     4 | Arbitration Priority Master 1  |
// |-------------------------------------------------------------|
// |    0x04 |     r/w  |     4 | Arbitration Priority Master 2  |
// |-------------------------------------------------------------|
// |    0x08 |     r/w  |     4 | Arbitration Priority Master 3  |
// |-------------------------------------------------------------|
// |    0x0C |     r/w  |     4 | Arbitration Priority Master 4  |
// |-------------------------------------------------------------|
// |    0x10 |     r/w  |     4 | Arbitration Priority Master 5  |
// |-------------------------------------------------------------|
// |    0x14 |     r/w  |     4 | Arbitration Priority Master 6  |
// |-------------------------------------------------------------|
// |    0x18 |     r/w  |     4 | Arbitration Priority Master 7  |
// |-------------------------------------------------------------|
// |    0x1C |     r/w  |     4 | Arbitration Priority Master 8  |
// |-------------------------------------------------------------|
// |    0x20 |     r/w  |     4 | Arbitration Priority Master 9  |
// |-------------------------------------------------------------|
// |    0x24 |     r/w  |     4 | Arbitration Priority Master 10 |
// |-------------------------------------------------------------|
// |    0x28 |     r/w  |     4 | Arbitration Priority Master 11 |
// |-------------------------------------------------------------|
// |    0x2C |     r/w  |     4 | Arbitration Priority Master 12 |
// |-------------------------------------------------------------|
// |    0x30 |     r/w  |     4 | Arbitration Priority Master 13 |
// |-------------------------------------------------------------|
// |    0x34 |     r/w  |     4 | Arbitration Priority Master 14 |
// |-------------------------------------------------------------|
// |    0x38 |     r/w  |     4 | Arbitration Priority Master 15 |
// |-------------------------------------------------------------|
// |    0x3C |     r/w  |    10 | Minimum number of cycles       |
// |         |          |       | before burst re-arbitration    |
// |-------------------------------------------------------------|
// |    0x40 |     r/w  |     1 | Burst re-arbitration enable    |
// |-------------------------------------------------------------|
// |    0x44 |   read   |     1 | Indication register for if a   |
// |         | to clear |       | burst was re-arbitrated.       |
// |-------------------------------------------------------------|
// |    0x48 |     r/w  |     4 | ID of the default master       |
// +-------------------------------------------------------------+
// 
// The Address is given relative to the base address, specified
// by the constructor argument "base_address_".
class DW_AHB_Arbiter// Pausable Arbiter Module, Slave IF Programmable
	: public sc_module
	, public ahb_namespace::ahb_arbiter_if
	, public ahb_namespace::ahb_slave_if
{

public:
    // parameters

    // The slave id of the arbiter.
    CCSS_PARAMETER(int, SlaveID);

    // When this parameter is enabled, unspecified lenght bursts will not
    // be re-arbitrated.
    CCSS_PARAMETER(bool, AHB_FULL_INCR);

    // This parameter enables the re-arbitration feature of incrementing
    // or wrapping bursts with a defined length.
    CCSS_PARAMETER(bool, EBTEN);

    // Enables the inclusion of a weighted token arbitration scheme.
    // If this parameter is enabled, the arbiter slave interface has to
    // be connected to the AHB bus.
    // 
    // NOTE: this feature is not yet implemented.
    CCSS_PARAMETER(bool, AHB_WTEN);

    // Enables the information trace output for pause mode changes.
    CCSS_PARAMETER(bool, DoTrace);

    // The combinatoric arbitration mode is used for the interconnect
    // matrix (DW_ahb_icm_tlm). For regular AHB bus arbitration the value
    // must be set to false.
    CCSS_PARAMETER(bool, CombinatoricMode);

    // ports

    // This active high input signal causes the arbiter
    // module to be suspended.
    sc_port<sc_signal_in_if<bool>,2> pause;

    // Interrupt signal to Interrupt Controller. The arbiter
    // will flag an interrupt when an Early Burst
    // Termination occurs.
    sc_out<bool> ahbarbint;

    // This signal is included when the weighted
    // token mask is enabled. Each bit of the bus
    // represents the weighted komen mask for that
    // master.
    sc_out<sc_bv<ahb_namespace::NUM_MASTERS> > ahb_wt_mask;

    // This signal indicates when the weighted token
    // arbitration period starts.
    sc_out<bool> ahb_wt_aps;

    // initialize parameters
    virtual void InitParameters() {
        int _tmp_SlaveID = 0;
        SlaveID.conditional_init(_tmp_SlaveID);
        bool _tmp_AHB_FULL_INCR = false;
        AHB_FULL_INCR.conditional_init(_tmp_AHB_FULL_INCR);
        bool _tmp_EBTEN = false;
        EBTEN.conditional_init(_tmp_EBTEN);
        bool _tmp_AHB_WTEN = false;
        AHB_WTEN.conditional_init(_tmp_AHB_WTEN);
        bool _tmp_DoTrace = true;
        DoTrace.conditional_init(_tmp_DoTrace);
        bool _tmp_CombinatoricMode = false;
        CombinatoricMode.conditional_init(_tmp_CombinatoricMode);
    }
	SC_HAS_PROCESS(DW_AHB_Arbiter);

	// First constructor with optional address specification for the slave
	// interface. The base address is the same for normal and boot mode.
	// The address specification is ignored if the slave interface is not used

	DW_AHB_Arbiter(sc_module_name,                     // instance name
	               ahb_namespace::ahb_addr_t base_addr = 0x01000000); // base address

	// Second constructor with optional address region for normal and boot mode
	// The address specification is ignored if the slave interface is not used

	DW_AHB_Arbiter(sc_module_name,                     // instance name
	               ahb_namespace::ahb_addr_t base_addr_normal_,       // normal mode base address
	               ahb_namespace::ahb_addr_t base_addr_boot_);        // boot mode base address

	~DW_AHB_Arbiter();


	// Register a port, make sure that maximum one slave port
	// is connected to the slave interface.
	virtual void
	register_port(sc_port_base&,
	              const char*);

	// Virtual Arbiter Interface Declarations

	virtual void
	initialize_priority(int,                      // ID of the bus
	                    int,                      // ID of the master
	                    int);                     // master priority value

	virtual void
	initialize(int,                               // ID of the bus
	           ahb_namespace::ahb_request_arb*[ahb_namespace::NUM_MASTERS], // master request struct array
	           int);                              // default master id

	// the arbitration routine
	virtual bool
	arbitrate(int,                                // ID of the bus
	          ahb_namespace::ahb_addr_t,                         // address
	          ahb_namespace::ahb_htrans,                          // transfer type
	          ahb_namespace::ahb_hburst,                      // burst mode
	          ahb_namespace::ahb_hresp,                         // status response
	          bool);                              // response

	virtual int
	address_request(int) const;                   // ID of the bus

	virtual int
	data_request(int) const;                      // ID of the bus

	virtual bool
	priority(int,                                 // ID of the bus
	         int,                                 // ID of the master
			 int);                                // priority value

	virtual int                                   // returns the priotity
	priority(int,                                 // ID of the bus
	         int) const;                          // ID of the master

	virtual bool
	set_default_master(int,                       // ID of the bus
	                   int);                      // new default master id


	// Virtual Slave Interface Declarations

	// Set the priority, address width, and data width of the bus.
	// Return the slave id reference.
	// The priority can be ignored by slaves that are only connected to one bus port
	virtual int
	register_bus(int,                         // id of the bus
	             int,                         // priority of the bus
	             int,                         // address width
	             int){return SlaveID;}        // data width

	// Submit a read request with corresponding address to the slave.
	// The slave has to respond to this request at the next cycle.
	virtual void
	read(int,                                 // id of the bus
	     ahb_namespace::ahb_addr_t,                          // the address
	     ahb_namespace::ahb_hsize);                          // number of bytes

	// Submit a write request with corresponding address to the slave.
	// The slave has to respond to this request at the next cycle.
	virtual void
	write(int,                                // id of the bus
	      ahb_namespace::ahb_addr_t,                         // the address
	      ahb_namespace::ahb_hsize);                         // number of bytes

	// This method provides additional control information, e.g.
	// the burst mode and the transfer type.
	virtual void
	control_info(int,                // id of the bus
	             ahb_namespace::ahb_hburst,      // mode of the active burst
	             ahb_namespace::ahb_htrans,          // the transfer type
	             ahb_namespace::ahb_hprot,           // access protection information
	             int,                // master id
	             bool);              // master locks the bus

	// Set the pointer to the data where the slave should execute
	// the request. This is only allowed if the transaction of the
	// last cycle has finished (no wait state). This method must
	// only be called once per request
	virtual void
	set_data(int,                             // id of the bus
	         ahb_namespace::ahb_data_t);                     // pointer to data

	// Query the ready response of the slave to the request
	// of the last cycle.
	// Response can be OKAY, ERROR, RETRY, SPLIT
	virtual bool
	response(int,                             // id of the bus
	         ahb_namespace::ahb_hresp&);                    // status response

	// Get the ID and address map of the slave.
	virtual const ahb_namespace::ahb_address_map&
	get_address_map(int);                  // id of the bus

	// Query the name of the slave
	virtual const char*
	name() const
		{return sc_module::name();}

	// Direct read from the memory for debugging purposes
	virtual bool
	direct_read(int,           // bus handle, unused here
	            ahb_namespace::ahb_addr_t,    // address
	            ahb_namespace::ahb_data_t,    // data
	            int);          // number of bytes

	// Direct write to the memory for debugging purposes
	virtual bool
	direct_write(int,          // bus handle, unused here
	             ahb_namespace::ahb_addr_t,   // address
	             ahb_namespace::ahb_data_t,   // data
	             int);         // number of bytes

protected:

	void construct(ahb_namespace::ahb_addr_t, ahb_namespace::ahb_addr_t);

	void on_pause_event();

	// dummy signal for default binding
	sc_signal<bool>  default_pause;

	int              _priority[ahb_namespace::NUM_SLAVES][ahb_namespace::NUM_MASTERS];

	int              _addr_req[ahb_namespace::NUM_MASTERS];
	int              _data_req[ahb_namespace::NUM_MASTERS];
	int              _default_req[ahb_namespace::NUM_MASTERS];

	int              _grant_req[ahb_namespace::NUM_MASTERS];
	int              _grant_req_lock[ahb_namespace::NUM_MASTERS];
	int              _grant_req_new[ahb_namespace::NUM_MASTERS];

	// early burst termination counter
	int              _ebt_cnt[ahb_namespace::NUM_SLAVES][ahb_namespace::NUM_MASTERS];
	// fair among equals counter
	int              _fae_cnt[ahb_namespace::NUM_SLAVES][ahb_namespace::NUM_MASTERS];
	// burst counter for burst of definite length
	int              _burst_cnt[ahb_namespace::NUM_SLAVES][ahb_namespace::NUM_MASTERS];

	bool             _hmast_lock[ahb_namespace::NUM_SLAVES];
	bool             _flush_lock[ahb_namespace::NUM_SLAVES];

	bool             _pause;

	bool             _ebt_en;
	int              _ebtcount;

	ahb_namespace::ahb_request_arb** _req_array[ahb_namespace::NUM_SLAVES];
	int              _last_resource_id;
	int              _master_id;

	ahb_namespace::ahb_addr_t       _base_address;
	ahb_namespace::ahb_data_t       _data;

	ahb_namespace::ahb_addr_t       _address;
	bool             _rflag;
	ahb_namespace::ahb_hsize        _hsize;
	bool             _error;

	bool             _idlebusy;

	ahb_namespace::ahb_address_map _address_map; // vector of up to 8 memory regions

	sc_port_base*   _slave_port;

}; // end module DW_AHB_Arbiter
#undef CCSS_INIT_MEMBERS_PREFIX
#undef CCSS_INIT_MEMBERS

#endif
