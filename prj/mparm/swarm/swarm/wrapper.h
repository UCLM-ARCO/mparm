///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         wrapper.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Wraps a C++ ARM ISS in a SystemC envelope
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __WRAPPER_H__
#define __WRAPPER_H__
 
#include <systemc.h>
#include "armproc.h"
#include "mem_class.h"

SC_MODULE(armsystem)
{ 
  CArmProc* pArm;
  uint16_t ID;

  sc_in_clk clock;
  sc_inout<PINOUT> *pinout_ft_master;
  sc_in<bool> *ready_from_master;
  sc_out<bool> *request_to_master;
  sc_inout<bool> *extint;

  void startsimul();
  void DebugMemoryDump(unsigned int address, const char *target, uint16_t TargetID);

  SC_HAS_PROCESS(armsystem);

  armsystem::armsystem(sc_module_name nm, unsigned int id, Mem_class *builderscratch, Mem_class *builderqueue) :
    sc_module(nm)
  {
    ID = id;
    
    if (DMA)
    {
      pinout_ft_master = new sc_inout<PINOUT> [2];
      ready_from_master = new sc_in<bool> [2];
      request_to_master = new sc_out<bool> [2];
    }
    else
    {
      pinout_ft_master = new sc_inout<PINOUT> [1];
      ready_from_master = new sc_in<bool> [1];
      request_to_master = new sc_out<bool> [1];
    }
    
    extint = new sc_inout<bool> [NUMBER_OF_EXT];
      
    pArm = new CArmProc(name(), builderscratch, builderqueue, ID);

    SC_CTHREAD(startsimul, clock.pos());
  }
};

#endif // __WRAPPER_H__
