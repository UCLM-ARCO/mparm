// ============================================================================
//  Description : Arbiter interface for the TL_AHB.
//       Author : Stefan Klostermann
//
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
// ============================================================================

#ifndef AHB_ARBITER_IF
#define AHB_ARBITER_IF

#include <systemc.h>

#include "ahb_types.h"

namespace ahb_namespace {

// ----------------------------------------------------------------------------
//  INTERFACE : ahb_arbiter_if
//
//  Arbiter interface for the TL bus model.
// ----------------------------------------------------------------------------

class ahb_arbiter_if
	: public sc_interface
{
public:

	virtual void
	initialize_priority(int,                      // ID of the bus
	                    int,                      // ID of the master
	                    int) {};                  // master priority value

	virtual void
	initialize(int,                               // ID of the bus
	           ahb_request_arb*[NUM_MASTERS],     // master request struct array
	           int) = 0;                          // default master id

	// the arbitration routine
	virtual bool
	arbitrate(int,                                // ID of the bus
	          ahb_addr_t,                         // address
	          ahb_htrans,                         // transfer type
	          ahb_hburst,                         // burst mode
	          ahb_hresp,                          // status response
	          bool) = 0;                          // response

	virtual int
	address_request(int) const = 0;               // ID of the bus

	virtual int
	data_request(int) const = 0;                  // ID of the bus

	virtual bool
	priority(int,                                 // ID of the bus
	         int,                                 // ID of the master
			 int) = 0;                            // priority value

	virtual int                                   // returns the priotity
	priority(int,                                 // ID of the bus
	         int) const = 0;                      // ID of the master

	virtual bool
	set_default_master(int,                       // ID of the bus
	                   int) = 0;                  // new default master id
};

}; // end namespace ahb_namespace

#endif
