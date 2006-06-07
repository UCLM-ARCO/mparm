///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_ahb_interconnect_if.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Prototype of an AHB interconnect behaviour.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __AMBA_AHB_INTERCONNECT_IF_H__
#define __AMBA_AHB_INTERCONNECT_IF_H__

#include <systemc.h>
#include "amba_ahb_common.h"

///////////////////////////////////////////////////////////////////////////////
// ahb_interconnect_if - Interfaces that an AMBA AHB interconnect must provide.
class ahb_interconnect_if
  : public virtual sc_interface
{
public:

  virtual unsigned char get_master_ID() = 0;
  virtual void ahb_interconnect_read(unsigned char ID, AHB_master_port *port) = 0;
  virtual void ahb_interconnect_write(unsigned char ID, AHB_master_port *port) = 0;

};

#endif // __AMBA_AHB_INTERCONNECT_IF_H__
