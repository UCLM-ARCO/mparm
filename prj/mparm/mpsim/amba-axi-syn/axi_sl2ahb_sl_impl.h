#ifndef _AXI_SL2AHB_SL_IMPL_H
#define _AXI_SL2AHB_SL_IMPL_H

#ifdef __EDG__
#define axi_namespace std
#endif

#include "ahb_types.h"
#include "ahb_slave_if.h"
#include "ahb_monitor_if.h"

#include "axi_types.h"
#include "axi_slave_if.h"
#include "axi_master_if_impl.h"
#include "axi_slave_port.h"
#include "axi_memory_impl.h"

struct ahb_request;

// This module converts the AXI slave transaction-level interface
// into AHB transaction-level method calls. One AHB Slave transaction-level
// interface can be connected to this adapter.

template<class T_SIF = ahb_namespace::ahb_slave_if  >
class AXI_Sl2AHB_Sl_impl// A Slave AXI Transaction-Level-Interface to AHB Transaction-Level-Interface Adapter 
	: public axi_namespace::AXI_MasterIFimpl<false>
	, public ahb_namespace::ahb_monitor_if
{
public:

    // The AHB slave bus interface is connected to this port.
	axi_namespace::AXI_SlavePort AXI_SlaveP;

    // The AHB slave bus interface is connected to this port.
    sc_port<T_SIF, 1> AHB_SlaveP;

    // The bus module is sensitive to the negative clock edge of this input signal.
	sc_in_clk hclk;

    // The asynchronous reset signal is active LOW.
	sc_in<bool> hresetn;

	SC_HAS_PROCESS(AXI_Sl2AHB_Sl_impl);

	AXI_Sl2AHB_Sl_impl(sc_module_name Name,
						   int ID,
						   axi_namespace::axi_addr_t StartAddress,
						   axi_namespace::axi_addr_t EndAddress,
						   bool MonitorOn,
						   int DataBusBytes,
						   double ClockFrequency);

	~AXI_Sl2AHB_Sl_impl();

	// Direct access methods Master to Slave direction.
	virtual bool GetSlaveParam(int ID, int& IntrlDepth, axi_namespace::AXI_AddressMap& M);
	virtual bool PutDirect(int ID, bool Write, axi_namespace::axi_addr_t Addr,
						   axi_namespace::axi_data_t Data, int NumBytes);
	virtual bool PutDirect(int ID, int BeatNumber, axi_namespace::Achannel &A, const axi_namespace::Wchannel& W,
						   axi_namespace::axi_resp_t &Status);
	virtual bool PutDirect(int ID, int BeatNumber, axi_namespace::Achannel &A, axi_namespace::Rchannel &R,
						   axi_namespace::axi_resp_t &Status);


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

	void
	do_trace(bool do_trace_) {
		m_DoTrace = do_trace_;
	}

private:

	virtual void end_of_elaboration();

	void m_on_Aresetn();

	void m_on_Aclk_pos();

	void m_on_Aclk_neg();

	bool m_addr_translate(axi_namespace::axi_addr_t, ahb_namespace::ahb_addr_t&);

	sc_event m_monitor_event;

	// parameter
	int                    m_ID;
	axi_namespace::axi_addr_t             m_StartAddress;
	axi_namespace::axi_addr_t             m_EndAddress;
	bool                   m_MonitorOn;
	int                    m_DataBusBytes;

	// constants
	int                    m_DataBusWords; // Width of the data bus in units of word.
                    // Wordsize is sizeof(axi_data_bt)
                    // A word is 4 bytes for unsigned int

	// channels
	axi_namespace::Achannel               A;
	axi_namespace::Wchannel               W;
	axi_namespace::Rchannel               R;
	axi_namespace::Bchannel               B;
	axi_namespace::AreadySig              Ar;
	axi_namespace::WreadySig              Wr;
	axi_namespace::RreadySig              Rr;
	axi_namespace::BreadySig              Br;

	// members:
	int                    m_state;
	int                    m_beat_count;
	bool                   m_success;
	bool                   m_Aresetn;

	const ahb_namespace::ahb_address_map  *m_addr_map;
	unsigned int           *m_offsets;

	int                    m_ready;
	ahb_request*           m_ahb_transf;
	int                    m_transf_state;
	bool                   m_data_pending;
	unsigned int*          m_ahb_rdata;
	unsigned int*          m_ahb_wdata;
	bool                   m_locked;

	bool                   m_DoTrace;
};
#endif //_AXI_SL_H
