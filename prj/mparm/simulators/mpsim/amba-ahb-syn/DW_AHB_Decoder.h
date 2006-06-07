#ifndef __DW_AHB_Decoder_h
#define __DW_AHB_Decoder_h

#include <systemc.h>
#include <ccss_systemc.h>

#include "ahb_types.h"
#include "ahb_decoder_if.h"

struct slave_lookup {
	ahb_namespace::ahb_addr_t start_addr;
	ahb_namespace::ahb_addr_t end_addr;
	ahb_namespace::ahb_addr_t offset;
	int id;
};

#ifdef CCSS_USE_SC_CTOR
#define CCSS_INIT_MEMBERS_PREFIX : 
#undef CCSS_USE_SC_CTOR
#else
#define CCSS_INIT_MEMBERS_PREFIX , 
#endif

#ifndef SYNTHESIS
#define CCSS_INIT_MEMBERS  CCSS_INIT_MEMBERS_PREFIX \
    remap_n("remap_n")
#else
#define CCSS_INIT_MEMBERS 
#endif

// The bus address decoder selects the slave interface that is registered to the
// address given by the bus. The address range of a slave is given by a start
// and end address that must be aligned to the 1 KB boundary. Each slave that is
// connected to the bus can have up to eight non-contiguous address ranges.
// 
// Remap Operation
// ---------------
// 
// The remap feature of the address decoder is typically used by Processors that
// may boot from a slow ROM and run from a faster RAM during normal operation.
// This means, for example, that the slave starting at address
// 0x00000000 is often remaped to a slave at another location. The amba_ahb library
// provides functionality to allow two different modes of address spaces: the
// Boot Mode and the Normal Mode. The address ranges of both modes may differ and
// are configured by the slaves connected to the bus.
// 
// The active mode of the address decoder is controlled by the "Remap" input
// signal. If the signal is asserted, the decoder switches to the Normal Mode
// address map. By default, the "Remap" input port is connected to an internal
// signal that is tied off to true. This default signal binding can be overwritten
// by an external signal.
class DW_AHB_Decoder// Decoder Module with Address Remap Scheme
	: public sc_module
	, public ahb_namespace::ahb_decoder_if
{

public:
    // parameters

    // Enables the information trace output for address mode changes.
    CCSS_PARAMETER(bool, DoTrace);

    // ports

    // This signal allows to remap the address space during runtime.
    sc_port<sc_signal_in_if<bool>,2> remap_n;

    // initialize parameters
    virtual void InitParameters() {
        bool _tmp_DoTrace = true;
        DoTrace.conditional_init(_tmp_DoTrace);
    }

	SC_HAS_PROCESS(DW_AHB_Decoder);

	DW_AHB_Decoder(sc_module_name name_);

	~DW_AHB_Decoder();

	virtual void
	initialize_map(int bus_id,                           // bus caller id
	               int slave_id,                         // slave id to register
	               const ahb_namespace::ahb_address_map& address_map);   // address map to initialize

	virtual void
	initialize(int bus_id,
	           int address_width_);

	virtual int
	get_slave(int,               // bus id
	          ahb_namespace::ahb_addr_t& addr); // decoding address reference

protected:

	void
	end_of_elaboration();

	void on_remap_event();

	sc_signal<bool>  default_remap;
	bool             _remap;

	ccss_monitor_msg m_address_map;
	bool             m_boot_address_overlap[ahb_namespace::NUM_MASTERS];
	bool             m_normal_address_overlap[ahb_namespace::NUM_MASTERS];

	slave_lookup**   lookup_array_normal[ahb_namespace::NUM_MASTERS];
	slave_lookup**   lookup_array_boot[ahb_namespace::NUM_MASTERS];
	unsigned int     lookup_array_size[ahb_namespace::NUM_MASTERS];

	slave_lookup***  lookup_array;
	slave_lookup*    lookup_cache[ahb_namespace::NUM_MASTERS];

	int              num_slaves;
	int              address_width;

	sc_unsigned      m_tmp;

}; // end module DW_AHB_Decoder
#undef CCSS_INIT_MEMBERS_PREFIX
#undef CCSS_INIT_MEMBERS

#endif
