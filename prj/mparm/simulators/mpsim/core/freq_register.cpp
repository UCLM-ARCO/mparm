///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Università di Bologna
//
// name         freq_register.cpp
// author       DEIS - Università di Bologna
//              Martino Ruggiero - mruggiero@deis.unibo.it
//              José L. Ayala - jayala@die.upm.es
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Davide Bertozzi - dbertozzi@deis.unibo.it
// info         Programmable register which feeds the frequency divider
//
///////////////////////////////////////////////////////////////////////////////

#include "freq_register.h"
#include "debug.h"

void feeder::simulfeeder()
{
  while(true)
  {
    for (int i = 0; i < outputs; i ++)
      feed[i].write(feed_t[i]);

    wait();
  }
};

void feeder::update(unsigned int addr, uint32_t value)
{
  ASSERT(addr < outputs);
  feed_t[addr] = value;
  //printf("%s: addr = %d value = %d at %10.1f ns\n",name(),addr,value, sc_simulation_time()); 
  // FIXME fragile!!!!!!!
  if (addr < (unsigned int)N_FREQ_DEVICE)
    M_DIVIDER[addr] = value;
  else
    I_DIVIDER[addr - N_FREQ_DEVICE] = value;
};

uint32_t feeder::get_value(unsigned int addr)
{
  ASSERT(addr < outputs);
  uint32_t value = feed_t[addr];
  return value;
};
