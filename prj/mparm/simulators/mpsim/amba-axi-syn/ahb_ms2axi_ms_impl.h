#ifndef _AHB_Ms2AXI_Ms_impl_H
#define _AHB_Ms2AXI_Ms_impl_H

#ifdef __EDG__
#define axi_namespace std
#endif

#include "ahb_bus_if.h"
#include "ahb_monitor_if.h"
#include "ahb_master_port.h"

#include "axi_types.h"
#include "axi_master_if.h"
#include "axi_master_port.h"
#include "axi_slave_if_impl.h"

template<class> class DW_AHB_TL_Master;
struct axi_request_temp;
struct ahb_request;

// This is the long description for AHB_Ms2AXI_Ms_impl
template<class T_BUSIF>
class AHB_Ms2AXI_Ms_impl// This is the short description for AHB_Ms2AXI_Ms_impl
	: public axi_namespace::AXI_SlaveIFimpl<false>
	, public T_BUSIF
	, public ahb_namespace::ahb_monitor_if
{
public:

	// The AXI master port
	axi_namespace::AXI_MasterPort MasterP;

    // The bus module is sensitive to the negative clock edge of this input signal.
	sc_in_clk hclk;

    // The asynchronous reset signal is active LOW.
	sc_in<bool> hresetn;

	SC_HAS_PROCESS(AHB_Ms2AXI_Ms_impl);

	AHB_Ms2AXI_Ms_impl(sc_module_name name_,
						   int ID,
						   int Priority,
						   int DataBusBytes,
						   double CLockFrequency);

	~AHB_Ms2AXI_Ms_impl();

// Direct access method
	virtual bool GetMasterParam(int Idx, int& Priority);

	virtual void
	request(int) {}

	virtual void
	end_request(int) {}

	virtual bool
	has_grant(int) const {return true;}

	virtual void
	lock(int);                            // master id

	virtual void
	unlock(int);                          // master id

	virtual void
	busy(int);                            // master id

	virtual void
	idle(int);                            // master id


	virtual void
	init_transaction(int,                 // master id
	                 ahb_namespace::ahb_hwrite, // read mode
	                 ahb_namespace::ahb_data_t,          // data pointer
	                 ahb_namespace::ahb_addr_t,          // address
	                 ahb_namespace::ahb_hburst,          // burst mode
	                 ahb_namespace::ahb_hsize,           // transfer size
	                 bool = false,        // ARM11: unaligned access
	                 int = 0);            // ARM11: master domain information


	virtual void
	set_data(int);                        // master id

	virtual void
	set_protection(int,                   // master id
	               ahb_namespace::ahb_hprot);            // protection value

	virtual bool
	response(int,                         // master id
	         ahb_namespace::ahb_hresp&) const;           // transfer status response

	virtual void
	set_byte_strobe(int,                  // caller id
	                const unsigned int*); // byte strobe line information
	virtual void
	priority(int,                         // master id
	         int);                        // priority

	virtual int
	priority(int) const;                  // master id

	virtual void
	set_default_master(int);              // master id

	virtual bool
	burst_read(int,                       // master id
	           unsigned int*,             // data buffer
	           ahb_namespace::ahb_addr_t,                // start address
	           int,                       // burst length
	           ahb_namespace::ahb_hburst,                // burst mode
	           ahb_namespace::ahb_hsize);                // transfer size

	virtual bool
	burst_write(int,                      // master id
	            unsigned int*,            // data buffer
	            ahb_namespace::ahb_addr_t,               // start address
	            int,                      // burst length
	            ahb_namespace::ahb_hburst,               // burst mode
	            ahb_namespace::ahb_hsize);               // transfer size

	// these two do not work
	virtual bool
	direct_read(int,                      // master id
	            ahb_namespace::ahb_addr_t,               // address
	            ahb_namespace::ahb_data_t,               // data pointer
	            int = 4);                 // number of bytes

	virtual bool
	direct_write(int,                     // master id
	             ahb_namespace::ahb_addr_t,              // address
	             ahb_namespace::ahb_data_t,              // data pointer
	             int = 4);                // number of bytes


	// Monitoring Methods

	// ID of the bus connected to
	virtual int
	bus_id() const;

	// instance name of the bus or ICM
	virtual const char*
	bus_name() const;

	// Width of the address bus
	virtual int
	bus_addr_width() const {
		return 32;
	}

	// Width of the data bus
	virtual int
	bus_data_width() const {
		return m_DataBusWords*32;
	}

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


	virtual void
	register_port(sc_port_base&,
	              const char*);

private:

	void m_on_hreset();
	void m_on_clk_pos();
	void m_on_clk_neg();

	axi_namespace::Achannel A;
	axi_namespace::Wchannel W;
	axi_namespace::Rchannel R;
	axi_namespace::Bchannel B;
	axi_namespace::AreadySig Ar;
	axi_namespace::WreadySig Wr;
	axi_namespace::RreadySig Rr;
	axi_namespace::BreadySig Br;

	DW_AHB_TL_Master<ahb_namespace::ahb_master_port>* m_master_fsm;

	sc_event          out_event;
	sc_event          m_monitor_event;

	ahb_request*      m_addr_request;
	axi_request_temp* m_addr_request_axi;

	ahb_request*      m_data_request;

	ahb_namespace::ahb_port_config* m_config_port;
	int               m_master_id;

	const unsigned int* m_abstrb;
	unsigned int      m_tmp_bstrb[4];
	unsigned int      m_bstrb[4];

	int               m_transf_state;

    bool              m_ready_addr;
    bool              m_ready_data;
	ahb_namespace::ahb_hresp m_status;
	ahb_namespace::ahb_hresp m_wstatus;

	int               m_addr_cnt;
	int               m_burst_length;

	unsigned int*     m_data;
	unsigned int*     m_rddata;
	unsigned int*     m_wrdata;
	unsigned int*     m_dummy_data;
	bool              m_last;
	bool              m_wflag;
	bool              m_resetn;

	// parameter
	int               m_ID;
	int               m_Priority;
	int               m_DataBusBytes;
	int               m_DataBusWords;
	int               m_StrbWords;
};
#endif //_AXI_MS_H
