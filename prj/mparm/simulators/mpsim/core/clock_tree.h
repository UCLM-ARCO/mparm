///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         clock_tree.h
// author       DEIS - Universita' di Bologna
//              José L. Ayala - jayala@die.upm.es
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Davide Bertozzi - dbertozzi@deis.unibo.it
// info         Module implementing a clock tree with parametric dividers
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __CLOCK_TREE_H__
#define __CLOCK_TREE_H__

#include <systemc.h>
#include "globals.h"

#define MAX_DIVIDER 6
										
SC_MODULE(clock_tree)
{
  sc_in<bool>                         Clock_in;
  sc_in<bool>                         Reset;
  sc_in<sc_uint<FREQSCALING_BITS> >   *Div;
  sc_out<bool>                        *Clock_out;

  unsigned short int *counter, *divider, *delay, outputs;
  bool *currentvalue;
  																																				
	void clock_tree_process();
    
  // Constructor
  SC_CTOR(clock_tree);
  clock_tree(sc_module_name nm, unsigned short int outputs) :
    sc_module(nm), outputs(outputs)
  {
    Div          = new sc_in<sc_uint<FREQSCALING_BITS> > [outputs];
    Clock_out    = new sc_out<bool>                      [outputs];
    
    counter      = new unsigned short int                [outputs];
    divider      = new unsigned short int                [outputs];
    delay        = new unsigned short int                [outputs];
    currentvalue = new bool                              [outputs];
  																															
    SC_METHOD(clock_tree_process);
    sensitive << Clock_in;
  }
};

#endif // __CLOCK_TREE_H__
