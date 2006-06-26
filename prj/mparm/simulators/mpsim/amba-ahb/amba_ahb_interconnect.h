///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_ahb_interconnect.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Describes an AMBA AHB interconnect.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __AMBA_AHB_INTERCONNECT_H__
#define __AMBA_AHB_INTERCONNECT_H__

#include <systemc.h>
#include "amba_ahb_common.h"
#include "amba_ahb_interconnect_if.h"
#include "amba_ahb_slave_if.h"

///////////////////////////////////////////////////////////////////////////////
// ARB_STATE - FSM of the AMBA AHB arbiter.
typedef enum
{
  arb_init,
  arb_read_param,
  arb_wait_hready,
  arb_wait_request
} ARB_STATE;
              
///////////////////////////////////////////////////////////////////////////////
// IC_STATE - FSM of the AMBA AHB interconnect.
typedef enum
{
  ic_sampling,
  ic_updating
} IC_STATE;

///////////////////////////////////////////////////////////////////////////////
// amba_ahb_interconnect - This class encapsulates an AMBA AHB interconnect.
class amba_ahb_interconnect : public ahb_interconnect_if, public sc_module
{
  public:
    sc_in<bool>                                 clock;
    sc_in<bool>                                 reset;
    sc_port<ahb_slave_if, AHB_MAX_SLAVES - 1>   slave_port;
  
  private:
    AHB_HWRITE                  m_hwrite[AHB_MAX_MASTERS];
    AHB_HWRITE                  m_hwrite_next[AHB_MAX_MASTERS];
    AHB_HTRANS                  m_htrans[AHB_MAX_MASTERS];
    AHB_HTRANS                  m_htrans_next[AHB_MAX_MASTERS];
    AHB_HSIZE                   m_hsize[AHB_MAX_MASTERS];
    AHB_HSIZE                   m_hsize_next[AHB_MAX_MASTERS];
    AHB_HBURST                  m_hburst[AHB_MAX_MASTERS];
    AHB_HBURST                  m_hburst_next[AHB_MAX_MASTERS];
    sc_uint<32>                 m_haddr[AHB_MAX_MASTERS];
    sc_uint<32>                 m_haddr_next[AHB_MAX_MASTERS];
    sc_uint<32>                 m_hwdata[AHB_MAX_MASTERS];
    sc_uint<32>                 m_hwdata_next[AHB_MAX_MASTERS];
    bool                        m_hgrant[AHB_MAX_MASTERS];
    bool                        m_hgrant_next[AHB_MAX_MASTERS];
    bool                        m_hbusreq[AHB_MAX_MASTERS];
    bool                        m_hbusreq_next[AHB_MAX_MASTERS];
    bool                        m_hlock[AHB_MAX_MASTERS];
    bool                        m_hlock_next[AHB_MAX_MASTERS];
    sc_uint<32>                 haddr;
    sc_uint<32>                 hwdata;
    AHB_HWRITE                  hwrite;
    AHB_HTRANS                  htrans;
    AHB_HSIZE                   hsize;
    AHB_HBURST                  hburst;
    sc_uint<32>                 hrdata;
    bool                        hready;
    bool                        hmastlock;    
    sc_uint<32>                 s_hrdata[AHB_MAX_SLAVES];
    bool                        s_hready[AHB_MAX_SLAVES];
    AHB_HRESP                   s_hresp[AHB_MAX_SLAVES];
    bool                        hsel[AHB_MAX_SLAVES];
    bool                        delayed_hsel[AHB_MAX_SLAVES];
    bool                        hsplit[AHB_MAX_SLAVES][AHB_MAX_MASTERS];
    sc_uint<AHB_HMASTER_BITS>   hmaster;
    sc_uint<AHB_HMASTER_BITS>   hmaster_next;
    sc_uint<AHB_HMASTER_BITS>   delayed_hmaster;
    AHB_HRESP                   hresp;
    
    ARB_STATE current_arb_state, next_arb_state;
    IC_STATE current_ic_state, next_ic_state;
    int arb_previous, arb_burst, arb_prev, arb_next, arb_counter, arb_busy_transaction;
    bool arb_just_rearbitrated, arb_sample_burst, arb_sampled_first, arb_rearbitrate;
    bool arb_bus_locked[AHB_MAX_MASTERS];
    bool arb_master_split[AHB_MAX_MASTERS];
    bool master_present[AHB_MAX_MASTERS];
    unsigned char ID;
    unsigned short int m_nmasters;
    unsigned short int m_nslaves;
    
    unsigned long int *starting_addresses;
    unsigned long int *ending_addresses;
    unsigned short int *mapping_target;
    unsigned long int range_count;
    
    int identified_masters, last_master_id;
    
    sc_trace_file *tracefile;
    bool tracing;
    
    unsigned long int certainly_valid_address;  // Address to be sent out by the
                                                // default master. Let's make sure
                                                // that it won't break the decoder
    AHB_bus_activity monitor_activity;
    
  public:
    unsigned char get_master_ID();
    void ahb_interconnect_read(unsigned char master_ID, AHB_master_port *port);
    void ahb_interconnect_write(unsigned char master_ID, AHB_master_port *port);
    
    SC_HAS_PROCESS(amba_ahb_interconnect);
    
    amba_ahb_interconnect(sc_module_name nm,
                          unsigned char ID,
                          sc_trace_file *tf,
                          bool tracing);

    void operate();

  private:
    void decode();
    void reset_ic();
    int  arbitrate();
    void grant();

    void end_of_elaboration();
    void register_port(sc_port_base& port_, const char* if_typename_);
};

#endif // __AMBA_AHB_INTERCONNECT_H__
