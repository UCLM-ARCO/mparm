// ============================================================================
//  Description : Misc functions and declarations of the TLM_AHB.
//       Author : Stefan Klostermann
//
//  Copyright (c) 2003 Synopsys, Inc. All rights reserved.
// ============================================================================

#ifndef AHB_DW_MISC_H
#define AHB_DW_MISC_H

#include <systemc.h>
#include <new>
#include <typeinfo>

#include "ahb_types.h"
#include "ahb_direct_if.h"

struct ahb_req_arb;
struct ahb_request;

typedef enum {
	ahb_htransf_read,
	ahb_htransf_write,
	ahb_htransf_noop
} ahb_htransf_state;

// ----------------------------------------------------------------------------
//  Struct : ahb_direct_null
//
//  Type for the state of a blocking burst transfer.
// ----------------------------------------------------------------------------

struct ahb_direct_dummy
	: public ahb_namespace::ahb_direct_if
{
public:

	virtual bool
	direct_read(int, ahb_namespace::ahb_addr_t, ahb_namespace::ahb_data_t, int) {return false;}

	virtual bool
	direct_write(int, ahb_namespace::ahb_addr_t, ahb_namespace::ahb_data_t, int) {return false;}
};

void
amba_tlm_calc_address(ahb_request&, bool);

// ----------------------------------------------------------------------------
//  Function : expand(sc_string)
//
//  Expand a path that includes environment variables
// ----------------------------------------------------------------------------

extern sc_string
expand(const sc_string&);

// ----------------------------------------------------------------------------
//  Function : amba_tlm_bytes2hsize(const int)
//
//  Returns hsize signal value converted from transfer size in bytes
// ----------------------------------------------------------------------------

extern ahb_namespace::ahb_hsize
amba_tlm_bytes2hsize(const int);

// ----------------------------------------------------------------------------
//  Function : calculate_strobe(ahb_addr_t, int, int, unsigned int*)
//
//  Calculates strobe information from address, number of bytes and data width
// ----------------------------------------------------------------------------

extern void
calculate_strobe(ahb_namespace::ahb_addr_t, int, int, bool, unsigned int*);

extern sc_string
ahb_hexprint(const unsigned int*,
             const unsigned int* = 0,
             int                 = 4);

// internal bus data structure, not visible to the outside

class ahb_request
{
public:
	ahb_namespace::ahb_data_t      data;

	bool            sequential;
	ahb_namespace::ahb_hsize       size;
	ahb_namespace::ahb_hprot       prot;

	// response status
	bool           ready;
	ahb_namespace::ahb_hresp      status;

	// request operation
	ahb_namespace::ahb_hwrite      rwflag;
	bool            busy;
	bool            idle;

	ahb_namespace::ahb_htrans      transf_type;

	ahb_namespace::ahb_hburst      burstmode;

	// arm v6
	bool            unaligned;
	int             master_domain;
	const unsigned int* byte_strobe;

	// address computation
	ahb_namespace::ahb_addr_t      current_address;
	ahb_namespace::ahb_addr_t      new_address;
	ahb_namespace::ahb_addr_t      remap_address;

	// full name (incl. path) of the master/bus
	const char*     master_name;

	// requester identification
	int             master_id;
	int             bus_id;
	int             priority;

	int             data_slave_id;

	ahb_request()
		: data               (0)
		, sequential         (false)
		, size               (ahb_namespace::SIZE_8)
		, prot               (ahb_namespace::DATA | ahb_namespace::PRIVILEGED)
		, ready              (true)
		, status             (ahb_namespace::OKAY)
		, rwflag             (ahb_namespace::RD)
		, busy               (false)
		, idle               (true)
		, transf_type        (ahb_namespace::IDLE)
		, burstmode          (ahb_namespace::SINGLE)
		, unaligned          (false)
		, master_domain      (0)
		, byte_strobe        (0)
		, current_address    (0)
		, new_address        (0)
		, remap_address      (0)
		, master_name        (0)
		, master_id          (0)
		, bus_id             (0)
		, priority           (0)
		, data_slave_id      (-1)
	{}
};

extern const unsigned int
byte_strobe_mask[16];

template <int data_width>
inline sc_bv<data_width>
array2sc_bv(const unsigned int* data) {
	sc_bv<data_width> tmp_data;

	for(int i=(data_width/32)-1; i>=0; --i) {
		tmp_data <<= 32;
		tmp_data |= data[i];
	}
	return tmp_data;
}

template <int data_width>
inline void
sc_bv2array(sc_bv<data_width> tmp_data, unsigned int* data) {
	for(int i=0; i<data_width/32; i++) {
		data[i] = tmp_data.to_uint();
		tmp_data = tmp_data>>32;
	}
}

// Transfer Attribute Constants used by read, write, read_expect
// set_burst_buffer_xfer_attr commands
//                          14     13     12    11         6 5       3 2      0
// |----------------------|-----|------|-------|------------|---------|--------|
// |   Reserved           | REQ | LOCK | UALGN |    PROT    |  BURST  |  SIZE  |
// |----------------------|-----|------|-------|------------|---------|--------|
// size use  bit 2 - 0
// burst use bit 5 - 3
// prot use  bit 11 - 6
// unaligned use bit 12
// lock use  bit 13
// request use  bit 14

const int DW_VIP_AMBA_XFER_SIZE_8       = 0x00000000;
const int DW_VIP_AMBA_XFER_SIZE_16      = 0x00000001;
const int DW_VIP_AMBA_XFER_SIZE_32      = 0x00000002;
const int DW_VIP_AMBA_XFER_SIZE_64      = 0x00000003;
const int DW_VIP_AMBA_XFER_SIZE_128     = 0x00000004;
const int DW_VIP_AMBA_XFER_SIZE_256     = 0x00000005;
const int DW_VIP_AMBA_XFER_SIZE_512     = 0x00000006;
const int DW_VIP_AMBA_XFER_SIZE_1024    = 0x00000007;

const int DW_VIP_AMBA_XFER_BURST_SINGLE = 0x00000000;
const int DW_VIP_AMBA_XFER_BURST_INCR   = 0x00000008;
const int DW_VIP_AMBA_XFER_BURST_WRAP4  = 0x00000010;
const int DW_VIP_AMBA_XFER_BURST_INCR4  = 0x00000018;
const int DW_VIP_AMBA_XFER_BURST_WRAP8  = 0x00000020;
const int DW_VIP_AMBA_XFER_BURST_INCR8  = 0x00000028;
const int DW_VIP_AMBA_XFER_BURST_WRAP16 = 0x00000030;
const int DW_VIP_AMBA_XFER_BURST_INCR16 = 0x00000038;

const int DW_VIP_AMBA_XFER_DATA_ACCESS  = 0x00000040;
const int DW_VIP_AMBA_XFER_PRIV_ACCESS  = 0x00000080;
const int DW_VIP_AMBA_XFER_BUFFERABLE   = 0x00000100;
const int DW_VIP_AMBA_XFER_CACHEABLE    = 0x00000200;
const int DW_VIP_AMBA_XFER_ALLOCATE     = 0x00000400; // NEW
const int DW_VIP_AMBA_XFER_EXCLUSIVE    = 0x00000800; // NEW

const int DW_VIP_AMBA_XFER_UNALIGNED    = 0x00001000; // NEW

const int DW_VIP_AMBA_XFER_LOCK_ON      = 0x00002000; // value changed

const int DW_VIP_AMBA_XFER_REQUEST      = 0x00004000; // value changed


const int VMT_IGNORE               = -1;
const int DW_VIP_AMBA_FLUSH_BUFFER = -2;
const int DW_VIP_AMBA_ALL_BEATS    = -3;

const int AHBMASTER_RESPONSE_NUM_RETRIES       = 1;
const int AHBMASTER_RESPONSE_HRESP             = 2;
const int AHBMASTER_RESPONSE_NUM_WAITSTATES    = 3;
const int AHBMASTER_RESPONSE_EARLY_TERMINATION = 4;

#endif


