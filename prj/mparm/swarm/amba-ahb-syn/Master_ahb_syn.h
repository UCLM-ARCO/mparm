// Master_ahb_syn.h: header file

#ifndef __Master_ahb_syn_h
#define __Master_ahb_syn_h

#include <systemc.h>
#include <ccss_systemc.h>

#include "config.h"
#include "globals.h"
#include "address.h"
#include "stats.h"
#include "ahb_types.h"
#include "ahb_master_port.h"

using namespace ahb_namespace;

class Master_ahb
: public sc_module
{
private:

ahb_namespace::ahb_addr_t address;

    typedef enum {
		ATTESA    = 0,
		RICHIESTA = 1,
		TRANSA    = 2,
		FINISH    = 3,
	} Stato;
 

public:

    // ports
    sc_in_clk hclk;
    sc_in<bool> hresetn;
    ahb_namespace::ahb_master_port p;
    sc_out<bool> ready_to_wrapper;
    sc_in<bool> request_from_wrapper;
    sc_inout<PINOUT> pinout;
    
    uint16_t my;
    bool PRINT;
   
    
    unsigned int _data[16];
    unsigned int buffer[16];
    ahb_namespace::ahb_hwrite _WR;
    ahb_namespace::ahb_addr_t _address;
    ahb_namespace::ahb_hburst _burst_mode;
    ahb_namespace::ahb_hsize  _hsize;
    
void working();	
void write_buffer();

  SC_HAS_PROCESS(Master_ahb);
  Master_ahb(sc_module_name name_, int MasterID, int Priority);


}; // end module Master_ahb

#endif
