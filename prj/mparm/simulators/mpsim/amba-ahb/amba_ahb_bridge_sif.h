///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_ahb_bridge_sif.h
// author       DEIS - Universita' di Bologna
//              Martino Ruggiero - mruggiero@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// info         Implements the slave side of an AMBA AHB bridge.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __AMBA_AHB_BRIDGE_SIF_H__
#define __AMBA_AHB_BRIDGE_SIF_H__

#include <systemc.h>
#include "globals.h"
#include "amba_ahb_slave_if.h"
#include "amba_ahb_common.h"
#include "debug.h"
#include "core_signal.h"

class amba_ahb_bridge_sif
: public ahb_slave_if, public sc_module
{
  public:
    sc_out<PINOUT> pinout_to_wrf;
    sc_in<PINOUT> pinout_from_rdf;

    sc_out<bool> request_to_push;
    sc_in<bool> push_full;
    sc_in<bool> error_from_push;
    sc_in<bool> write_overrun;
    
    sc_out<bool> request_to_pop;
    sc_in<bool> pop_empty;
    sc_in<bool> error_from_pop;
    sc_in<bool> read_overrun;
    
  private:
    typedef enum
    {
      NOT_SELECTED          = 0,
      TWO_CYCLES_ERROR_RESP = 1,
      SELECTED              = 2,
      SINGLE_WRITE          = 3,
      BURST_WRITE           = 4,
      MAKE_REQ_READ         = 5,
      SINGLE_READ           = 6,
      BURST_READ            = 7
    } SLAVE_STATE;

    SLAVE_STATE state;
    PINOUT sl_pinout;
    bool error;
    int beat;
    bool request_made;
    bool split_made;
    bool ok_received;
    int master_ID;
    int burst;
    int countwait;
    int hmaster;
    int bridge_ID;
    unsigned short int bw;
    bool is_write, selected;
    unsigned long int address;
    
    void am_i_selected(AHB_slave_port *port);
    
  public:
    void ahb_slave_query(unsigned char IC_ID, AHB_slave_port *port);
    unsigned short int addressing_ranges();
    unsigned long int start_address(unsigned short int range_ID);
    unsigned long int end_address(unsigned short int range_ID);
    
    amba_ahb_bridge_sif(sc_module_name nm, uint bridge_ID)
      : sc_module(nm),
        request_to_push (false),
        request_to_pop  (false),
        error           (false),
        beat            (0),
        request_made    (false),
        burst           (1),
        countwait       (0),
        bridge_ID       (bridge_ID)
    {
      TRACEX(BRIDGE_TRACEX, 7, "%s %d\n", name(), bridge_ID);

      split_made = false;
      ok_received = false;
      state = NOT_SELECTED;    
      master_ID = AHB_DEFAULT_MASTER;
    }
};

#endif // __AMBA_AHB_BRIDGE_SIF_H__
