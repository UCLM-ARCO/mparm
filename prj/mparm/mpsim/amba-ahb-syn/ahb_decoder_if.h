// ============================================================================
//  Description : Decoder interface for the TLM_AHB.
//       Author : Stefan Klostermann
//
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
// ============================================================================

#ifndef AHB_DECODER_IF
#define AHB_DECODER_IF

#include <systemc.h>

#include "ahb_types.h"
#include "ahb_slave_if.h"

namespace ahb_namespace {

// ----------------------------------------------------------------------------
//  INTERFACE : ahb_decoder_if
//
//  Decoder interface for the TL bus model.
// ----------------------------------------------------------------------------

class ahb_decoder_if
	: public sc_interface
{
public:

	// Address map initialization
	virtual void
	initialize_map(int,                       // bus ID
	               int,                       // slave ID to register
	               const ahb_address_map&){}; // slave address map to initialize

	virtual void
	initialize(int,                           // bus ID
	           int) = 0;                      // address width

	// get the slave interface for the calling master,
	// calculate and modify the new address (if remaped)
	virtual int
	get_slave(int,                            // bus ID
	          ahb_addr_t&) = 0;               // address to decode and optional remap
};

}; // end namespace ahb_namespace

#endif
