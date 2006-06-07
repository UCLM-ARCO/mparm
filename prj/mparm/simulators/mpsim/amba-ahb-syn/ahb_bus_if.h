// ============================================================================
//  Description : Bus interface for the TL_AHB.
//       Author : Stefan Klostermann
//
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
// ============================================================================

#ifndef AHB_BUS_IF_H
#define AHB_BUS_IF_H

#include <systemc.h>

#include "ahb_types.h"
#include "ahb_direct_if.h"


namespace ahb_namespace {


// ----------------------------------------------------------------------------
//  INTERFACE : ahb_bus_if
//
//  Bus master interface for the TL bus model.
// ----------------------------------------------------------------------------

class ahb_configure_bus_if
{
public:

	////////////////////////
	// structural methods //
	////////////////////////

	virtual int
	bus_addr_width() const = 0;

	virtual int
	bus_data_width() const = 0;

	// Get and set the priority of a master. The priority must
	// be greater than zero.
	virtual void
	priority(int, int) {}                    // master id

	virtual int
	priority(int) const {return -1;}         // master id

	// Establish the default granted master
	virtual void
	set_default_master(int) {}               // master id
};

class ahb_blocking_bus_if
{
public:
	///////////////////////////////
	// blocking transfer methods //
	///////////////////////////////

	// blocking methods at higher abstraction level
	virtual bool
	burst_read(int,                          // master id
	           ahb_data_t,                   // data buffer
	           ahb_addr_t,                   // start address
	           int,                          // burst length
	           ahb_hburst,                   // burst mode
	           ahb_hsize) = 0;               // number of bytes

	// blocking methods at higher abstraction level
	virtual bool
	burst_write(int,                         // master id
	            ahb_data_t,                  // data buffer
	            ahb_addr_t,                  // start address
	            int,                         // burst length
	            ahb_hburst,                  // burst mode
	            ahb_hsize) = 0;              // number of bytes

	//////////////////////////////////////////
	// non-blocking methods for bus locking //
	//////////////////////////////////////////

	// lock the bus that re-arbitration is not allowed
	virtual void
	lock(int) = 0;                           // master id

	// unlock the bus that re-arbitration is allowed
	virtual void
	unlock(int) = 0;                         // master id
};


// ----------------------------------------------------------------------------
//  INTERFACE : ahb_bus_if
//
//  Virtual methods declarations for the bus interface
//  of the abstract bus model.
// ----------------------------------------------------------------------------

class ahb_bus_if
	: public ahb_direct_if
	, public ahb_blocking_bus_if
	, public ahb_configure_bus_if
{
public:

	///////////////////////////////////
	// non- blocking AHB-Lite subset //
	///////////////////////////////////

	// lock the bus that re-arbitration is not allowed
	virtual void
	lock(int) = 0;                           // master id

	// unlock the bus that re-arbitration is allowed
	virtual void
	unlock(int) = 0;                         // master id

	// bus master is busy
	virtual void
	busy(int) = 0;                           // master id

	// bus master insterts idle explicitly
	virtual void
	idle(int) = 0;                           // master id

	// Initialize a transaction
	virtual void
	init_transaction(int,                    // master id
	                 ahb_hwrite,             // read mode
	                 ahb_data_t,             // data exchange array
	                 ahb_addr_t,             // start address
	                 ahb_hburst,             // burst mode
	                 ahb_hsize,              // number of bytes
	                 bool = false,           // unaligned access
	                 int = 0) = 0;           // master domain information

	// Point to the value where the bus should write
	// or read the data to or from
	virtual void
	set_data(int) = 0;                       // master id

	virtual void
	set_protection(int,                      // master id
	               ahb_hprot) = 0;           // protection value

	// Query the status response of a master request
	virtual bool
	response(int, ahb_hresp&) const = 0;     // master id

	///////////////////////////////////////
	// non- blocking AHB Full Extentions //
	///////////////////////////////////////

	// Register a master request for bus access.
	virtual void
	request(int) = 0;                        // master id

	// Check for bus grant. Returns true if bus access is granted
	virtual bool
	has_grant(int) const  = 0;               // master id

	// Method to unregister a master request.
	virtual void
	end_request(int)  = 0;                   // master id

	// Master completion notification
	virtual void
	master_complete(int) {};                 // master id
};

// ----------------------------------------------------------------------------
//  INTERFACE : ahb_lite_bus_if
//
//  The AHBLite TL interface is inherited from the AHB full interface. The
//  method provide a default implementation for the arbitraton signals that
//  are not required by a lite system anymore. AHB full modules are still
//  allowed to connect to a lite system, but AHBLite masters can not be connected
//  to a AHB full bus channel.
// ----------------------------------------------------------------------------

class ahb_lite_bus_if
	: public ahb_bus_if
{
public:

	// Register a master request for bus access.
	virtual void
	request(int) {}                          // master id

	// Check for bus grant. Returns true if bus access is granted
	virtual bool
	has_grant(int) const {return true;}      // master id

	// Method to unregister a master request.
	virtual void
	end_request(int) {}                      // master id
};

// ----------------------------------------------------------------------------
//  INTERFACE : ahbv6_bus_if
//  
//  Not specified yet!
// ----------------------------------------------------------------------------

class ahb11ext_bus_if
	: public ahb_bus_if
{
public:

	virtual void
	set_byte_strobe(int,                        // caller id
	                const unsigned int*) = 0;   // byte strobe line information
};

}; // end namespace ahb_namespace

#endif
