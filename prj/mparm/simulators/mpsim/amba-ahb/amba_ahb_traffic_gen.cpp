///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_ahb_traffic_gen.cpp
// author       DEIS - Universita' di Bologna
//              Martino Ruggiero - mruggiero@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
// info         AMBA AHB traffic generator
//
///////////////////////////////////////////////////////////////////////////////

#include "amba_ahb_traffic_gen.h"
#include "stats.h"
#include "config.h"
#include "globals.h"
#include "address.h"
#include "debug.h"

///////////////////////////////////////////////////////////////////////////////
// working - Implements the functionality of an AMBA AHB bus master. Traffic
//           transactions are first collected from a transaction generator;
//           subsequently, either a read or write code path is taken.
//           Bursts (1, 4, 8 or 16-beat incrementing) are supported.
//           The IC interface is called to query IC status and to request new
//           transactions. Multiple interface calls can be made in the same
//           cycle, as long as they all happen during the rising clock edge
//           (i.e. not even a delta cycle later).
void amba_ahb_traffic_gen::working()
{
  AHB_master_port my_port;

  uint32_t *data = new uint32_t [8]; //FIXME!!!!!!!!
  uint32_t prev_address = 0x0, address = 0x0;
  uint burst = 0, size = 0;
  int idlewait = 0;
  TGEN_BENABLE benable = TGEN_WORD;
  bool wr = false;
  
  local_ID = master_port->get_master_ID();
  
  //printf("AMBA master %s: global ID %hu, local ID %hu\n", name(), global_ID, local_ID);
  
  if (STATS)
    statobject->startMeasuring(global_ID);

  while (true)
  {
    transgen->get_trans(data, &address, &burst, &wr, &benable, &idlewait);
    
    if (idlewait == -1)
    {
      if (STATS)
        statobject->stopMeasuring(global_ID);
      statobject->quit(global_ID);
      break;
    }
    
    if (!wr)
    {
      if (data)
        delete data;
      data = new uint32_t [burst];
    }
    
    for (int i = 0; i < idlewait; i ++)
      wait();

    // Word, half word or byte?
    switch (benable)
    {
      case TGEN_WORD:      my_port.hsize = AHB_WORD;
                           size = 0x4;
                           break;
      case TGEN_BYTE:      my_port.hsize = AHB_BYTE;
                           size = 0x1;
                           break;
      case TGEN_HALFWORD:  my_port.hsize = AHB_HALFWORD;
                           size = 0x2;
                           break;
      default :            printf("Fatal error: Traffic generator master %hu detected a malformed data size at %10.1f ns\n",
                                    global_ID, sc_simulation_time());
                           exit(1);
    }

    switch (burst)
    {
      case 1:  my_port.hburst = AHB_SINGLE;
               break;
      case 4:  my_port.hburst = AHB_INCR4;
               break;
      case 8:  my_port.hburst = AHB_INCR8;
               break;
      case 16: my_port.hburst = AHB_INCR16;
               break;
      default: printf("Fatal error: Traffic generator master %hu detected an unsupported burst length %u at %10.1f ns\n",
                  global_ID, burst, sc_simulation_time());
               exit(1);
               //TODO other burst lengths
    }

    my_port.htrans = AHB_NONSEQ;
     
    if (STATS)
      statobject->requestsAccess(global_ID);

    my_port.hbusreq = true;
    my_port.haddr = address;
      
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
      prev_address = address;
      address += size;
      my_port.haddr = address;
              
      if (burst == 1)
        my_port.htrans = AHB_IDLE;
      else
        my_port.htrans = AHB_SEQ;

      master_port->ahb_interconnect_write(local_ID, &my_port);

      for (uint i = 0; i < burst; i ++)
      {
        do
        {
          wait();
          master_port->ahb_interconnect_read(local_ID, &my_port);
        } while (my_port.hready != true);

        data[i] = my_port.hrdata;

        TRACEX(MASTER_TRACEX, 8, "%s: %s read  0x%08x at address 0x%08x at %10.1f ns\n%s",
          name(), burst == 1 ? "single" : "burst ", data[i],
          (uint)prev_address, sc_simulation_time(), (i == burst - 1) ? "\n" : "");
           
        prev_address = address;
        
        // Either ending the burst, or going on
        if (i >= burst - 2)
          my_port.htrans = AHB_IDLE;
        else
        {
          // Increment the address for the next beat
          address += size;
          my_port.haddr = address;
          my_port.htrans = AHB_SEQ;
        }

        master_port->ahb_interconnect_write(local_ID, &my_port);
      } // end for
        
      if (STATS)
        statobject->endsAccess(!wr, burst, global_ID);

      wait();
    }
    // ------------------------------ WRITE ACCESS ------------------------------
    else
    {
      my_port.hwrite = AHB_WRITE;

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

      for (uint i = 0; i < burst; i++)
      {
        // If we're in a burst transfer, at this point we already have subsequent
        // data to process
        prev_address = address;
        my_port.hwdata = data[i];
        
        if (burst != 1)
        {
          address += size;
          my_port.haddr = address;
        }

        // Either ending the burst, or going on
        if (i == burst - 1)
          my_port.htrans = AHB_IDLE;
        else
          my_port.htrans = AHB_SEQ;
        master_port->ahb_interconnect_write(local_ID, &my_port);
          
        // Let's signal the input module that we're done with this beat
        wait();
          
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
          name(), burst == 1 ? "single" : "burst ", data[i],
          (uint)prev_address, sc_simulation_time(), (i == burst - 1) ? "\n" : "");
      } // end for
    }

    if (STATS) //FIXME should be one cycle later for writes
      statobject->busFreed(global_ID);
  }
}
