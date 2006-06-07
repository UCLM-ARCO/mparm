// ============================================================================
//  Description : Master port for the TLM AHB.
//       Author : Stefan Klostermann
//
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
// ============================================================================

#ifndef AHB_MASTER_PORT_H
#define AHB_MASTER_PORT_H

#include <systemc.h>

#include "ahb_bus_if.h"

namespace ahb_namespace {

class ahb_port_config
{
public:
	ahb_port_config(const char* master_name,    // name (incl. path) of the master
	                int master_id = 0,          // unique master id <16
	                int priority = 1,           // priority of the master
	                bool default_grant = false) // assign default grant
		: _m_name        (master_name)
		, _master_id     (master_id)
		, _priority      (priority)
		, _default_grant (default_grant)
	{}

	int priority() const {
		return _priority;
	}

	// Does the master request the default granted
	bool request_default_grant() {
		return _default_grant;
	}

	const char* master_name() {
		return _m_name;
	}

	int master_id() {
		return _master_id;
	}

protected:

	const char* _m_name;
	int         _master_id;
	int         _priority;
	bool        _default_grant;
};


template <class MIF>
class ahb_port_base
	: public sc_port<MIF>
	, public ahb_port_config
{
public:
	ahb_port_base(const char* master_name,         // name (incl. path) of the master
	              int master_id = 0,               // unique master id <16
	              int priority = 1,                // priority of the master
	              bool default_grant = false)      // assign default grant
		: ahb_port_config(master_name, master_id, priority, default_grant)
	{}
					  
	int
	bus_addr_width() {
		return (*this)->bus_addr_width();
	}

	int
	bus_data_width() {
		return (*this)->bus_data_width();
	}

	// Register a master request for bus access.
	void
	request() {
		(*this)->request(_master_id);
	}

	// Method to unregister a master request.
	void
	end_request() {
		(*this)->end_request(_master_id);
	}

	// Check for bus grant.
	bool
	has_grant() const {
		return (*this)->has_grant(_master_id);
	}

	// lock the bus that no re-arbitration is done
	void
	lock() {
		(*this)->lock(_master_id);
	}

	// un- lock the bus
	void
	unlock() {
		(*this)->unlock(_master_id);
	}

	// indication that master is busy
	void
	busy() {
		(*this)->busy(_master_id);
	}

	// indication that master is busy
	void
	idle() {
		(*this)->idle(_master_id);
	}

	// Read data from the slave. The calling master must granted
	// exclusive bus access.
	void
	init_transaction(ahb_hwrite rflag,              // read mode
	                 ahb_data_t data,               // data transfer array
	                 ahb_addr_t address,            // start address
	                 ahb_hburst burstmode,          // burst mode
	                 ahb_hsize transf_size,         // number of bytes
	                 bool unalign = false,          // unaligned access
	                 int mdomain = 0) {             // master domain information
		(*this)->init_transaction(_master_id,
		                          rflag,
		                          data,
		                          address,
		                          burstmode,
		                          transf_size,
		                          mdomain);
	}

	void
	set_data() {
		(*this)->set_data(_master_id);
	}

	void
	set_protection(ahb_hprot prot_value) {          // protection value
		(*this)->set_protection(_master_id,
		                        prot_value);
	}

	// Query the status response of a master request
	bool
	response(ahb_hresp& status) const {
		return (*this)->response(_master_id, status);
	}

	// blocking read method
	bool
	burst_read(unsigned int* buffer_,               // pointer to local data
	           ahb_addr_t start_address_,           // start address
	           int burst_length_,                   // burst length
	           ahb_hburst burst_mode_ = INCR,       // burst mode
	           ahb_hsize transf_size_ = SIZE_32) {  // transfer size

		return (*this)->burst_read(_master_id,
		                           buffer_,
		                           start_address_,
		                           burst_length_,
		                           burst_mode_,
		                           transf_size_);
	}

	// blocking write method
	bool
	burst_write(unsigned int* buffer_,              // pointer to local data
	            ahb_addr_t start_address_,          // start address
	            int burst_length_,                  // burst length
	            ahb_hburst burst_mode_ = INCR,      // burst mode
	            ahb_hsize transf_size_ = SIZE_32) { // number of bytes

		return (*this)->burst_write(_master_id,
		                            buffer_,
		                            start_address_,
		                            burst_length_,
		                            burst_mode_,
		                            transf_size_);
	}

	// direct read from the slave
	bool direct_read(ahb_addr_t address_,           // address
	                 ahb_data_t data_,              // data
	                 int transf_size_ = 4) {        // number of bytes
		return (*this)->direct_read(_master_id,
		                             address_,
		                             data_,
		                             transf_size_);
	}

	// direct write to the slave
	bool direct_write(ahb_addr_t address_,          // address
	                  ahb_data_t data_,             // data
	                  int transf_size_ = 4) {       // number of bytes
		return (*this)->direct_write(_master_id,
		                             address_,
		                             data_,
		                             transf_size_);
	}

	void
	master_complete() {
		(*this)->master_complete(_master_id);
	};

};

// ----------------------------------------------------------------------------
//  PORT : ahb_master_port
//
//  Master port for the TL bus model.
// ----------------------------------------------------------------------------

class ahb_master_port
	: public ahb_port_base<ahb_bus_if>
{
public:

	// Default constructor
	ahb_master_port(const char* master_name,        // name (incl. path) of the master
	                int master_id,                  // unique master id <16
	                int priority,                   // priority of the master
	                bool default_grant = false)     // assign default grant
		: ahb_port_base<ahb_bus_if>(master_name, master_id, priority, default_grant)
	{}

	// Get and set the priority of a master. The priority must
	// be greater than zero.
	void priority(int priority_) {
		if(get_interface() != 0)
			(*this)->priority(_master_id, priority_);
		_priority = priority_;
	}

	// Establish the default granted master
	void set_default_master(bool default_grant = true) {
		if(get_interface() != 0) {
			if(default_grant)
				(*this)->set_default_master(_master_id);
		}
		_default_grant = default_grant;
	}
};

class ahb_lite_master_port
	: public ahb_port_base<ahb_lite_bus_if>
{
public:

	// Default constructor
	ahb_lite_master_port(const char* master_name)   // name (incl. path) of the master
		: ahb_port_base<ahb_lite_bus_if>(master_name)
	{}
};

class ahb11ext_master_port
	: public ahb_port_base<ahb11ext_bus_if>
{
public:

	ahb11ext_master_port(const char* master_name,   // name (incl. path) of the master
	                  int master_id,                // unique master id <16
	                  int priority,                 // priority of the master
	                  bool default_grant = false)   // assign default grant
		: ahb_port_base<ahb11ext_bus_if>(master_name, master_id, priority, default_grant)
	{}

	// Get and set the priority of a master. The priority must
	// be greater than zero.
	void priority(int priority_) {
		if(get_interface() != 0)
			(*this)->priority(_master_id, priority_);
		_priority = priority_;
	}

	// Establish the default granted master
	void set_default_master(bool default_grant = true) {
		if(get_interface() != 0) {
			if(default_grant)
				(*this)->set_default_master(_master_id);
		}
		_default_grant = default_grant;
	}


// EXTENSIONS FOLLOW HERE:


	void set_byte_strobe(const unsigned int* bytestr_) // byte strobe line information
	{
		(*this)->set_byte_strobe(_master_id,        // caller id
		                         bytestr_);         // byte strobe line information
	}
};

}; // end namespace ahb_namespace

#endif
