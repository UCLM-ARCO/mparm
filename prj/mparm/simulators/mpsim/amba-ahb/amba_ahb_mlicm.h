///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_ahb_mlicm.h
// author       DEIS - Universita' di Bologna
//              Martino Ruggiero - mruggiero@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
//              Davide Bertozzi - dbertozzi@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Implements an AMBA AHB MultiLayer InterConnect Matrix.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __AMBA_AHB_MLICM_H__
#define __AMBA_AHB_MLICM_H__

#include <systemc.h>
#include "amba_ahb_common.h"
#include "amba_ahb_slave_if.h"
#include "address.h"
#include "debug.h"

///////////////////////////////////////////////////////////////////////////////
// amba_ahb_mlicm - This class encapsulates an AMBA AHB MultiLayer
//                  InterConnect Matrix.
//                  nm: ICM name
//                  ID: ICM ID
//                  tf: file for waveform tracing
//                  tracing: enable or disable waveform tracing
class amba_ahb_mlicm : public ahb_slave_if, public sc_module
{
  public:
    sc_port<ahb_slave_if, AHB_MAX_SLAVES> icm_slave_port;
  
  private:
    char *type;
    unsigned char ID;
    sc_trace_file *tf;
    bool tracing;
    unsigned long int WHICH_TRACEX;
    int assigned_slave_table[AHB_MAX_SLAVES];
    unsigned short int m_nlayers;
    unsigned short int m_nslaves;
    
    unsigned long int *starting_addresses;
    unsigned long int *ending_addresses;
    unsigned short int *mapping_target;
    unsigned long int range_count;

    void end_of_elaboration();
    void register_port(sc_port_base& port_, const char* if_typename_);
    
    short int decode(unsigned char IC_ID, AHB_slave_port *port);
    void reset_icm();
    
  protected:
    unsigned int ready_count[AHB_MAX_ICM_LAYERS];
    bool selected[AHB_MAX_ICM_LAYERS], blocked_on_input_stage[AHB_MAX_ICM_LAYERS], release_slave[AHB_MAX_ICM_LAYERS];
    unsigned int burst[AHB_MAX_ICM_LAYERS], slave_ID[AHB_MAX_ICM_LAYERS];
    AHB_slave_port slave_port[AHB_MAX_ICM_LAYERS];

  public:
    void ahb_slave_query(unsigned char IC_ID, AHB_slave_port *port);
    unsigned short int addressing_ranges();
    unsigned long int start_address(unsigned short int range_ID);
    unsigned long int end_address(unsigned short int range_ID);
    
    amba_ahb_mlicm(sc_module_name nm,
                   unsigned char ID,
                   sc_trace_file *tf,
                   bool tracing) : ID(ID), tf(tf), tracing(tracing)
    {
      type = "amba_ahb_matrix";
      WHICH_TRACEX = ICM_TRACEX;
      
      m_nlayers = 0;
      
      reset_icm();
    }
};

#endif // __AMBA_AHB_MLICM_H__
