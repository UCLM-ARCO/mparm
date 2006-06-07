///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_ahb_slave.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Describes an AMBA AHB slave.
//
///////////////////////////////////////////////////////////////////////////////

#include "amba_ahb_slave.h"
#include "debug.h"

///////////////////////////////////////////////////////////////////////////////
// addressing_ranges - Returns the number of addressing ranges that this slave
//                     is mapping (usually just one, but MultiLayer devices
//                     will have more).
unsigned short int amba_ahb_slave::addressing_ranges()
{
  return 1;
}

///////////////////////////////////////////////////////////////////////////////
// start_address - Returns the starting address for the range_ID addressing
//                 range mapped by the slave.
unsigned long int amba_ahb_slave::start_address(unsigned short int range_ID)
{
  return START_ADDRESS;
}

///////////////////////////////////////////////////////////////////////////////
// end_address - Returns the starting address for the range_ID addressing
//               range mapped by the slave.
unsigned long int amba_ahb_slave::end_address(unsigned short int range_ID)
{
  return (START_ADDRESS + TARGET_MEM_SIZE - 1);
}

///////////////////////////////////////////////////////////////////////////////
// ahb_slave_query - Implements the slave query interface, which is used by the
//                   AHB IC to request transactions and evaluate responses.
//                   Upon first selection, the slave will sample control
//                   and address signals. Then, it will skip wait states (if
//                   any) by just returning control to the AMBA AHB IC. Finally,
//                   the real transaction will be performed by using a virtual
//                   method calling a C++ class (this mechanism can be overridden
//                   by inheritance to provide different functionality).
//                   *******************
//                   BEWARE: the slave MUST set a proper value for its outputs
//                   (hresp, hreadyout, hrdata, hsplit) before EVERY return!
//                   BEWARE: slave computation must complete within any number
//                   of delta cycles (i.e., be instantaneous or combinational).
//                   Sequential logic or delays are not allowed, since all of the
//                   slaves must be queried during any given clock cycle, so
//                   the total time for their queries must not exceed the clock
//                   period length.
void amba_ahb_slave::ahb_slave_query(unsigned char IC_ID, AHB_slave_port *port)
{
  unsigned long int data;
  bool must_wait;
  unsigned char i;

  if (!((port->hsel == true &&
        port->hready == true &&
        (port->htrans == AHB_NONSEQ || port->htrans == AHB_SEQ)) || selected))
  {
    // Not selected
    port->hreadyout = true;
    port->hresp     = AHB_OKAY;
    port->hrdata    = 0xdeadbeef;
    for (i = 0; i < AHB_MAX_MASTERS; i ++)  
      port->hsplit[i] = false;
    return;
  }
  else
  {
    // Selected
    if (ws_counter == 0)
    {
      // First transaction cycle, so let's remember relevant info
      address = port->haddr;
      if (port->hwrite == AHB_WRITE)
        is_write = true;
      else
        is_write = false;
      selected = true;
    
      switch (port->hsize)
      {
        case AHB_WORD:     bw = MEM_WORD;
                           break;
        case AHB_HALFWORD: bw = MEM_HWORD;
                           break;
        case AHB_BYTE:     bw = MEM_BYTE;
                           break;
        default:           printf("Fatal error: %s %d: Wrong width %hu!\n", type, ID, (unsigned char)port->hsize);
                           exit(1);
      }
    }
    
    // ------------------------------ WRITE ACCESS ------------------------------
    if (is_write == true)
    {
      // Even if we have no wait states, data will only be correct in the data
      // phase, which will begin one cycle after we've sampled the address
      if (ws_counter == 0)
      {
        //FIXME we have to wait for an HREADY here, not just for one cycle!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        // Keep HREADY low during the wait states
        ws_counter ++;
        port->hreadyout = false;
        port->hresp     = AHB_OKAY;
        port->hrdata    = 0xdeadbeef;
        for (i = 0; i < AHB_MAX_MASTERS; i ++)  
          port->hsplit[i] = false;
        return;
      }

      // Now the actual wait states
      if (ws_counter < mem_in_ws)
      {
        // Keep HREADY low during the wait states
        ws_counter ++;
        port->hreadyout = false;
        port->hresp     = AHB_OKAY;
        port->hrdata    = 0xdeadbeef;
        for (i = 0; i < AHB_MAX_MASTERS; i ++)  
          port->hsplit[i] = false;
        return;
      }

      data = port->hwdata;
      this->Write(address, data, bw, &must_wait);
      
      // We got immediate results
      if (!must_wait)
      {
        port->hreadyout = true;
        port->hresp     = AHB_OKAY;
        port->hrdata    = 0xdeadbeef;
        for (i = 0; i < AHB_MAX_MASTERS; i ++)  
          port->hsplit[i] = false;
        
        TRACEX(WHICH_TRACEX, 8, "%s %d: Addr = 0x%08x Write Data = 0x%08x at %7.1f ns\n",
          type, ID, (unsigned int)address, (unsigned int)data, sc_simulation_time());
        
        ws_counter = 0;
        selected = false;
        return;
      }
      // The device we're querying has additional wait states
      else
      {
        port->hreadyout = false;
        port->hresp     = AHB_OKAY;
        port->hrdata    = 0xdeadbeef;
        for (i = 0; i < AHB_MAX_MASTERS; i ++)  
          port->hsplit[i] = false;
        return;
      }
    }
    // ------------------------------ READ ACCESS ------------------------------
    else
    {
      if (ws_counter < mem_in_ws)
      {
        // Keep HREADY low during the wait states
        ws_counter ++;
        port->hreadyout = false;
        port->hresp     = AHB_OKAY;
        port->hrdata    = 0xdeadbeef;
        for (i = 0; i < AHB_MAX_MASTERS; i ++)  
          port->hsplit[i] = false;
        return;
      }
        
      data = this->Read(address, &must_wait);
      
      // We got immediate results
      if (!must_wait)
      {
        port->hreadyout = true;
        port->hresp     = AHB_OKAY;
        port->hrdata    = data;
        for (i = 0; i < AHB_MAX_MASTERS; i ++)  
          port->hsplit[i] = false;
        
        TRACEX(WHICH_TRACEX, 8, "%s %d: Addr = 0x%08x Read  Data = 0x%08x at %7.1f ns\n",
          type, ID, (unsigned int)address, (unsigned int)data, sc_simulation_time());

        ws_counter = 0;
        selected = false;
        return;
      }
      // The device we're querying has additional wait states
      else
      {
        port->hreadyout = false;
        port->hresp     = AHB_OKAY;
        port->hrdata    = 0xdeadbeef;
        for (i = 0; i < AHB_MAX_MASTERS; i ++)  
          port->hsplit[i] = false;
        return;
      }
    }
  }
}
