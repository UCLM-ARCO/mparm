// ============================================================================
//  Description : Slave interface for the TLM_AHB.
//       Author : Stefan Klostermann
//
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
// ============================================================================

#ifndef AHB_SLAVE_IF_H
#define AHB_SLAVE_IF_H

#include <systemc.h>

#include "ahb_types.h"
#include "ahb_direct_if.h"

namespace ahb_namespace {

// ----------------------------------------------------------------------------
//  INTERFACE : ahb_slave_if
//
//  Slave interface for the TL bus model.
// ----------------------------------------------------------------------------

class ahb_slave_if :
	public ahb_direct_if
{
public:

	// Set the priority, address width, and data width of the bus.
	// Return the ID of the slave.
	// The priority can be ignored by slaves that are only connected to one bus port
	virtual int
	register_bus(int,                // id of the bus
	             int,                // priority of the bus
	             int,                // address width
	             int) = 0;           // data width

	// Submit a read request with corresponding address to the slave.
	// The slave has to respond to this request at the next cycle.
	virtual void
	read(int,                        // id of the bus
	     ahb_addr_t,                 // the address
	     ahb_hsize) = 0;             // number of bytes

	// Submit a write request with corresponding address to the slave.
	// The slave has to respond to this request at the next cycle.
	virtual void
	write(int,                       // id of the bus
	      ahb_addr_t,                // the address
	      ahb_hsize) = 0;            // number of bytes

	// This method is called if burst information is requested
	// and returns the burst mode and the transfer mode.
	virtual void
	control_info(int,                // id of the bus
	             ahb_hburst,         // mode of the active burst
	             ahb_htrans,         // the transfer type
	             ahb_hprot,          // access protection information
	             int,                // master id
	             bool) = 0;          // master locks the bus

	// Set the pointer to the data where the slave should execute
	// the request. This is only allowed if the transaction of the
	// last cycle has finished (no wait state). This method must
	// only be called once per request
	virtual void
	set_data(int,                    // id of the bus
	         ahb_data_t) = 0;        // pointer to data

	// Query the ready response of the slave to the request
	// of the last cycle.
	// Response can be AHB_OKAY, AHB_ERROR, AHB_RETRY, AHB_SPLIT
	virtual bool
	response(int,                    // id of the bus
	         ahb_hresp&) = 0;        // status response

	// Get the address map of the slave.
	virtual const ahb_address_map&
	get_address_map(int) = 0;        // id of the bus

	// Query the name of the slave
	virtual const char*
	name() const = 0;                // name of slave

	// Query the ready response of the slave to the request
	// of the last cycle.
	// This method does not need to be implemented if a slave does
	// not support split transfers, and thus is not allowed to return
	// split response.
	virtual bool
	split_response(int,              // id of the bus
	               int)              // master id
		{return true;}

};

class ahb11ext_slave_if
	: public ahb_slave_if
{
public:

	virtual void
	set_ahb11ext_control(int,                 // bus id
	                     const unsigned int*, // byte strobe lane information
	                     bool,                // unaligned access indication
	                     int) = 0;            // protection domain
};

}; // end namespace ahb_namespace

#endif
