// ============================================================================
//  Description : Declarations for the direct slave interface
//       Author : Stefan Klostermann
//
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
// ============================================================================

#ifndef AHB_DIRECT_IF_H
#define AHB_DIRECT_IF_H

#include <systemc.h>

#include "ahb_types.h"

// ----------------------------------------------------------------------------
//  INTERFACE : ahb_direct_if
//
//  Direct read/write interface for the TL bus model.
// ----------------------------------------------------------------------------

namespace ahb_namespace {

class ahb_direct_if
	: public sc_interface
{
public:

	// direct read from the slave
	virtual bool direct_read(int,                         // caller id
	                         ahb_addr_t,                  // address
	                         ahb_data_t,                  // data
	                         int = 4) = 0;                // number of bytes

	// direct write to the slave
	virtual bool direct_write(int,                        // caller id
	                          ahb_addr_t,                 // address
	                          ahb_data_t,                 // data
	                          int = 4) = 0;               // number of bytes
};

}; // end namespace ahb_namespace

#endif
