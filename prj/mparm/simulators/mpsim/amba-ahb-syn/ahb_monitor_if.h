// ============================================================================
//  Description : Monitor interface for the TL AHB.
//       Author : Stefan Klostermann
//
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
// ============================================================================

#ifndef AHB_MONITOR_IF_H
#define AHB_MONITOR_IF_H

#include <systemc.h>

#include "ahb_types.h"

namespace ahb_namespace {

// ---------------------------------------------------------------------------
//  Interface : ahb_monitor_if
//
//  Monitor interface for the TL bus model.
// ---------------------------------------------------------------------------

class ahb_monitor_if
	: public sc_interface
{
public:

	// Bus Structure and Configuration


	// ID of the bus connected to
	virtual int
	bus_id() const = 0;

	// instance name of the bus or ICM
	virtual const char*
	bus_name() const = 0;

	// Width of the address bus
	virtual int
	bus_addr_width() const = 0;

	// Width of the data bus
	virtual int
	bus_data_width() const = 0;

	// Number of master ports connected to the bus
	virtual int
	num_masters() const = 0;

	// Name of a master with given master ID
	virtual const char*
	master_name(int) const = 0;               // Master ID (< NUM_MASTERS)

	// Is the master ID active (used)
	virtual bool
	master_is_used(int) const = 0;            // Master ID (< NUM_MASTERS)

	// Number of slave interfaces connected to the bus
	virtual int
	num_slaves() const = 0;

	// Name of a slave with given slave ID
	virtual const char*
	slave_name(int) const = 0;                // Slave ID (<= NUM_SLAVES)

	// Is the slave ID active (used)
	virtual bool
	slave_is_used(int) const = 0;             // Slave ID (<= NUM_SLAVES)


	// Monitoring Module Sensitivity


	virtual const sc_event&
	default_event() const = 0;


	// AHB Master Driven Pseudo Signal Methods


	// Address driven by the master
	virtual ahb_addr_t
	m_address(int) const = 0;                 // Master ID

	// Transfer type
	virtual int
	m_transfer_type(int) const = 0;           // Master ID

	// Write/Read transfer indication
	virtual bool
	m_write(int) const = 0;                   // Master ID

	// Size of a transfer
	virtual int
	m_size(int) const = 0;                    // Master ID

	// Burstmode
	virtual int
	m_burst(int) const = 0;                   // Master ID

	// Write data value
	virtual const ahb_data_t
	m_wdata(int) const = 0;                   // Master ID

	// Protection value
	virtual int
	m_prot(int) const = 0;                    // Master ID


	// AHB Slave Driven Pseudo Signal Methods


	// Slave ready response
	virtual int
	s_ready(int) const = 0;                   // Slave ID

	// Slave status response
	virtual int
	s_response(int) const = 0;                // Slave ID

	// Read data value
	virtual const ahb_data_t
	s_rdata(int) const = 0;                   // Slave ID

	// Slave response to indicate that a split transfer should be
	// allowed to be re-attempted
	virtual int
	s_splitx(int) const = 0;                  // Slave ID


	// AHB Read Pseudo Signal Methods


	// Bus Address
	virtual ahb_addr_t
	address(int) const = 0;                   // Slave ID (used for ICM systems only)

	// Transfer type
	virtual int
	transfer_type(int) const = 0;             // Slave ID (used for ICM systems only)

	// Write/Read transfer indication
	virtual bool
	write(int) const = 0;                     // Slave ID (used for ICM systems only)

	// Size of a transfer
	virtual int
	size(int) const = 0;                      // Slave ID (used for ICM systems only)

	// Burstmode
	virtual int
	burst(int) const = 0;                     // Slave ID (used for ICM systems only)

	// Read data value
	virtual const ahb_data_t
	rdata(int) const = 0;                     // Master ID (used for ICM systems only)

	// Write data value
	virtual const ahb_data_t
	wdata(int) const = 0;                     // Slave ID (used for ICM systems only)

	// Ready response
	virtual int
	ready(int) const = 0;                     // Master ID

	// Status response
	virtual int
	response(int) const = 0;                  // Master ID (used for ICM systems only)

	// Protection value
	virtual int
	prot(int) const = 0;                      // Slave ID (used for ICM systems only)

	// Bit representation of slaves indication that a split transfer
	// should be allowed to be re-attempted
	virtual int
	splitx(int) const = 0;                    // Master ID (used for ICM systems only)


	// AHB arbitration control pseudo signals


	// Master requesting bus access
	virtual bool
	m_busrequest(int) const = 0;              // Master ID

	// Bit representation of the master grant lines
	virtual bool
	grant(int) const = 0;                     // Master ID

	// Master requests locked transfers
	virtual bool
	m_lock(int) const = 0;                    // Master ID

	// Indicates that the current master is performing a locked
	// sequence of transfers
	virtual bool
	masterlocked(int) const = 0;              // Slave ID (used for ICM systems only)

	// ID of the master that owns the address bus
	virtual int
	addr_master_id(int) const = 0;            // Slave ID (used for ICM systems only)

	// ID of the master that owns the data bus
	virtual int
	data_master_id(int) const = 0;            // Slave ID (used for ICM systems only)

	// ID of the addressed slave (address phase)
	virtual int
	addr_slave_id(int) const = 0;             // Master ID (used for ICM systems only)

	// ID of the data slave (data phase)
	virtual int
	data_slave_id(int) const = 0;             // Master ID (used for ICM systems only)


	// ARM11 extensions:


	virtual const unsigned int*
	m_byte_strobe(int) const                  // Master ID
		{return 0;}

	virtual const unsigned int*
	byte_strobe(int) const                    // Slave ID (used for ICM systems only)
		{return 0;}


	virtual bool
	m_unaligned(int) const                    // Master ID
		{return false;}

	virtual bool
	unaligned(int) const                      // Slave ID (used for ICM systems only)
		{return false;}


	virtual int
	m_domain(int) const                       // Master ID
		{return 0;}

	virtual int
	domain(int) const                         // Slave ID (used for ICM systems only)
		{return 0;}
};

}; // end namespace ahb_namespace

#endif
