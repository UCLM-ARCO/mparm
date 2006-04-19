///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         decoder.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Address decoder for an AMBA bus
//
///////////////////////////////////////////////////////////////////////////////

#include "decoder.h"
#include "address.h"

///////////////////////////////////////////////////////////////////////////////
// dec_sel - Monitors haddr values and selects the corresponding slave
//           by means of hsel signals. Based upon library functions
void decoder::dec_sel()
{
  short int slave, i;
  sc_uint<32> address;

  address = haddr.read();
  slave = addresser->MapPhysicalToSlave(address);
  if (slave < 0)
  {
    printf("Fatal error: Decoding memory address 0x%08x to any slave is impossible!\n", (uint32_t)address);
    exit(1);
  }

  for (i=0; i < N_SLAVES; i++)
    if(i == slave)
      hsel[i].write(true);
    else
      hsel[i].write(false);

#ifdef PRINTDEBUG
  printf("Decoding finished, address 0x%08x mapped to slave %d\n", (uint32_t)address, slave);
#endif

};
