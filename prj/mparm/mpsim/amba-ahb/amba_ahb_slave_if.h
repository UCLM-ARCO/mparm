///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_ahb_slave_if.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Prototype of an AHB slave behaviour.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __AMBA_AHB_SLAVE_IF_H__
#define __AMBA_AHB_SLAVE_IF_H__

#include <systemc.h>
#include "amba_ahb_common.h"

///////////////////////////////////////////////////////////////////////////////
// ahb_slave_if - Interfaces that an AMBA AHB slave must provide.
class ahb_slave_if
  : public virtual sc_interface
{
public:

  virtual void ahb_slave_query(unsigned char IC_ID, AHB_slave_port *port) = 0;

  virtual unsigned short int addressing_ranges() = 0;
  virtual unsigned long int start_address(unsigned short int range_ID) = 0;
  virtual unsigned long int end_address(unsigned short int range_ID) = 0;
};

#endif // __AMBA_AHB_SLAVE_IF_H__
