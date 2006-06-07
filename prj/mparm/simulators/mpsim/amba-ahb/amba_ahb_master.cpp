///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_ahb_master.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         AMBA AHB bus master
//
///////////////////////////////////////////////////////////////////////////////

#include "amba_ahb_master.h"
#include "stats.h"
#include "config.h"
#include "globals.h"
#include "address.h"
#include "debug.h"

///////////////////////////////////////////////////////////////////////////////
// working - Implements the functionality of an AMBA AHB bus master. First, a
//           CPU request is processed; subsequently, either a read or write
//           code path is taken. Bursts (1, 4, 8 or 16-beat incrementing) are
//           supported.
//           The IC interface is called to query IC status and to request new
//           transactions. Multiple interface calls can be made in the same
//           cycle, as long as they all happen during the rising clock edge
//           (i.e. not even a delta cycle later).
void amba_ahb_master::working()
{
  AHB_master_port my_port;

  uint32_t din, dout, addr, prev_addr, size, next_dout;
  int burst = 0, i;
  bool wr;
  PINOUT mast_pinout;
  
  local_ID = master_port->get_master_ID();
  
  // printf("AMBA master %s: global ID %hu, local ID %hu\n", name(), global_ID, local_ID);

  ready_to_wrapper.write(false);

  while (true)
  {
    // Wait until someone requests something
    wait_until(request_from_wrapper.delayed() == true);
    // What's up?
    mast_pinout = pinout.read();
    // Word, half word or byte?
    switch (mast_pinout.bw)
    {
      case 0 :  my_port.hsize = AHB_WORD;
                size = 0x4;
                break;
      case 1 :  my_port.hsize = AHB_BYTE;
                size = 0x1;
                break;
      case 2 :  my_port.hsize = AHB_HALFWORD;
                size = 0x2;
                break;
      default : printf("Fatal error: Master %hu detected a malformed data size at %10.1f ns\n",
                         global_ID, sc_simulation_time());
                exit(1);
    }

    burst = (int)mast_pinout.burst;
    wr = mast_pinout.rw;
    addr = addresser->Logical2Physical(mast_pinout.address, global_ID);

    if (burst == 1)
      my_port.hburst = AHB_SINGLE;
    if (burst == 4)
      my_port.hburst = AHB_INCR4;
    if (burst == 8)
      my_port.hburst = AHB_INCR8;
    if (burst == 16)
      my_port.hburst = AHB_INCR16;
    //TODO other burst lengths

    my_port.htrans = AHB_NONSEQ;
     
    if (STATS)
      statobject->requestsAccess(global_ID);

    my_port.hbusreq = true;
    my_port.haddr = addr;
      
    // ------------------------------ READ ACCESS ------------------------------
    if (!wr)
    {
      my_port.hwrite = AHB_READ;
      // Let's request the bus
      master_port->ahb_interconnect_write(local_ID, &my_port);

      // Wait for the grant. Actually, we might get a zero-cycle
      // grant (e.g. if we're the only master on the bus)
      master_port->ahb_interconnect_read(local_ID, &my_port);
      while (!(my_port.hgrant && my_port.hready))
      {
        wait();
        master_port->ahb_interconnect_read(local_ID, &my_port);
      }

      // Here, we really got the bus and pushed out the first address,
      // deassert hbusreq
      my_port.hbusreq = false;
      
      master_port->ahb_interconnect_write(local_ID, &my_port);
      do
      {
        wait();
        master_port->ahb_interconnect_read(local_ID, &my_port);
      } while (!(my_port.hready));
      
      if (STATS)
        statobject->beginsAccess(2, !wr, burst, global_ID);
          
      // Increment the address for the next beat
      // (if in a single transaction, the address bus isn't ours any more by now)
      prev_addr = addr;
      addr += size;
      my_port.haddr = addr;
              
      if (burst == 1)
        my_port.htrans = AHB_IDLE;
      else
        my_port.htrans = AHB_SEQ;

      master_port->ahb_interconnect_write(local_ID, &my_port);

      for (i = 0; i < burst; i ++)
      {
        do
        {
          wait();
          ready_to_wrapper.write(false);
          master_port->ahb_interconnect_read(local_ID, &my_port);
        } while (my_port.hready != true);

        din = my_port.hrdata;
        mast_pinout.data = din;

        TRACEX(MASTER_TRACEX, 8, "%s: %s read  0x%08x at address 0x%08x at %10.1f ns\n%s",
          name(), burst == 1 ? "single" : "burst ", uint(din),
          (uint)prev_addr, sc_simulation_time(), (i == burst - 1) ? "\n" : "");
           
        prev_addr = addr;
        
        // Either ending the burst, or going on
        if (i >= burst - 2)
          my_port.htrans = AHB_IDLE;
        else
        {
          // Increment the address for the next beat
          addr += size;
          my_port.haddr = addr;
          my_port.htrans = AHB_SEQ;
        }

        master_port->ahb_interconnect_write(local_ID, &my_port);

        pinout.write(mast_pinout);
        ready_to_wrapper.write(true);
      } // end for
        
      if (STATS)
        statobject->endsAccess(!wr, burst, global_ID);

      wait();
      ready_to_wrapper.write(false);
    }
    // ------------------------------ WRITE ACCESS ------------------------------
    else
    {
      next_dout = mast_pinout.data;
      my_port.hwrite = AHB_WRITE;
      ready_to_wrapper.write(true);
      
      // Let's request the bus
      master_port->ahb_interconnect_write(local_ID, &my_port);
      
      // Wait for the grant. Actually, we might get a zero-cycle
      // grant (e.g. if we're the only master on the bus)
      master_port->ahb_interconnect_read(local_ID, &my_port);
      while (!(my_port.hgrant && my_port.hready))
      {
        wait();
        ready_to_wrapper.write(false);
        master_port->ahb_interconnect_read(local_ID, &my_port);
      }

      // Here, we really got the bus and pushed out the first address,
      // deassert hbusreq
      my_port.hbusreq = false;
      
      master_port->ahb_interconnect_write(local_ID, &my_port);
      do
      {
        wait();
        ready_to_wrapper.write(false);
        master_port->ahb_interconnect_read(local_ID, &my_port);
      } while (!(my_port.hready));

      if (STATS)
        statobject->beginsAccess(2, !wr, burst, global_ID);

      for (i = 0; i < burst; i++)
      {
        // If we're in a burst transfer, at this point we already have subsequent
        // data to process
        prev_addr = addr;
        dout = next_dout;
        my_port.hwdata = dout;
        
        if (burst != 1)
        {
          addr += size;
          next_dout = pinout.read().data;
          my_port.haddr = addr;
          ready_to_wrapper.write(true);
        }

        // Either ending the burst, or going on
        if (i == burst - 1)
          my_port.htrans = AHB_IDLE;
        else
          my_port.htrans = AHB_SEQ;
        master_port->ahb_interconnect_write(local_ID, &my_port);
          
        // Let's signal the input module that we're done with this beat
        wait();
        ready_to_wrapper.write(false);
          
        // Let's wait for the slave to finish writing (it may have already,
        // during the single cycle we were keeping ready_to_wrapper high)
        master_port->ahb_interconnect_read(local_ID, &my_port);
        while (my_port.hready != true)
        {
          wait();
          master_port->ahb_interconnect_read(local_ID, &my_port);
        }
        
        if (STATS && (i == burst - 1))
          statobject->endsAccess(!wr, burst, global_ID);
        
        TRACEX(MASTER_TRACEX, 8, "%s: %s write 0x%08x at address 0x%08x at %10.1f ns\n%s",
          name(), burst == 1 ? "single" : "burst ", uint(dout),
          (uint)prev_addr, sc_simulation_time(), (i == burst - 1) ? "\n" : "");
      } // end for
    }

    if (STATS) //FIXME should be one cycle later for writes
      statobject->busFreed(global_ID);
  }
}
