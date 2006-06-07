///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_ahb_bridge_mif.cpp
// author       DEIS - Universita' di Bologna
//              Martino Ruggiero - mruggiero@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// info         Implements the master side of an AMBA AHB bridge.
//
///////////////////////////////////////////////////////////////////////////////

#include "amba_ahb_bridge_mif.h"
#include "stats.h"
#include "config.h"
#include "globals.h"
#include "address.h"

void amba_ahb_bridge_mif::working()
{
  AHB_master_port my_port;

  uint32_t din, dout, addr=0, prev_addr, error_addr=0, size=0, next_dout;
  bool wr=0;
  PINOUT mast_pinout;
  int burst=0, i=0, passed=0;
  bool error = false;
  MASTER_STATE state = WAITING;

  ID = master_port->get_master_ID();
  
 while(true){
 switch (state)
{
 
case WAITING :
 
     do{
     if(POWERSTATS)
      statobject->inspectAMBAAHBBridge(bridge_ID, 1, false);
        wait();
     }while(pop_empty.read() == true);
     
     
request_to_pop.write(true);
if(POWERSTATS)
      statobject->inspectAMBAAHBBridge(bridge_ID, 1, true);
wait();
request_to_pop.write(false);
if(POWERSTATS)
      statobject->inspectAMBAAHBBridge(bridge_ID, 1, false);
wait();
     
mast_pinout = pinout_from_wrf.read();

     state = QUERY;
     error=false;
     passed=0;
     break;
     
case QUERY :

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
      default : printf("Fatal error: Master %hu detected a malformed data size at %lu ns\n",
                         ID, (unsigned long int)sc_simulation_time());
                exit(1);
    }
     
    wr = mast_pinout.rw;
        if(!error)
    addr = mast_pinout.address;
        else
    addr = error_addr;
   
    burst = (int)mast_pinout.burst;  
      if (burst == 1)
        my_port.hburst = AHB_SINGLE;
      if (burst == 4)
        my_port.hburst = AHB_INCR4;
      if (burst == 8)
        my_port.hburst = AHB_INCR8;
      if (burst == 16)
        my_port.hburst = AHB_INCR16;
              
my_port.htrans = AHB_NONSEQ;  
if (STATS)
statobject->requestsAccess(ID);

my_port.hbusreq = true;
my_port.haddr = addr;

TRACEX(MASTER_TRACEX, 2,"%s : %s %s at 0x%08x burst = %d at %10.1f ns\n",
 name(), burst == 1 ? "single" : "burst ", wr? "write" : "read",addr,burst, sc_simulation_time());

 if (!wr)
  my_port.hwrite = AHB_READ; 
 else
  my_port.hwrite = AHB_WRITE;

  if(!wr)
   state = READ;
  else 
   state = WRITE;
  
  break;

case READ :

    if(burst>1){
       //wait_until(write_overrun.delayed()==false);
      do{
     if(POWERSTATS)
      statobject->inspectAMBAAHBBridge(bridge_ID, 1, false);
        wait();
     }while(write_overrun.read()==true);
            }
    else
       //wait_until(push_full.delayed()==false);
             do{
     if(POWERSTATS)
      statobject->inspectAMBAAHBBridge(bridge_ID, 1, false);
        wait();
     }while(push_full.read()==true);
        
      my_port.hwrite = AHB_READ;
      // Let's request the bus
      master_port->ahb_interconnect_write(ID, &my_port);
     
     master_port->ahb_interconnect_read(ID, &my_port);
      while (!(my_port.hgrant && my_port.hready))
      {
        if(POWERSTATS)
      statobject->inspectAMBAAHBBridge(bridge_ID, 1, false);
        wait();
        master_port->ahb_interconnect_read(ID, &my_port);
      }
      
   if((burst-passed == 1) || (my_port.hburst != AHB_INCR))   
     my_port.hbusreq = false;

master_port->ahb_interconnect_write(ID,&my_port);
      
      do
      {
      if(POWERSTATS)
      statobject->inspectAMBAAHBBridge(bridge_ID, 1, false);
        wait();
        master_port->ahb_interconnect_read(ID, &my_port);
                   if ((my_port.hresp == AHB_ERROR) && (state != QUERY))
                      {
                       passed = 0;
                       error_addr = addr;
                       error = true;
                       my_port.htrans = AHB_IDLE;
                       my_port.hbusreq = false;
                       master_port->ahb_interconnect_write(ID, &my_port);
                       state = QUERY;
                      }
      } while (!(my_port.hready));

    if(state == QUERY)
    {
    if(POWERSTATS)
      statobject->inspectAMBAAHBBridge(bridge_ID, 1, false);
     wait();
     break;
     }

 if(STATS)
  statobject->beginsAccess(2, !wr, burst, ID);
               
   prev_addr = addr;
   addr += size;
   my_port.haddr = addr;
     
 if((burst - passed == 2) && (my_port.hburst == AHB_INCR)){ 
      my_port.hbusreq = false; 
      master_port->ahb_interconnect_write(ID, &my_port);
      } 
      
 if (burst - passed  == 1)
     my_port.htrans = AHB_IDLE;
 else
     my_port.htrans = AHB_SEQ;

 master_port->ahb_interconnect_write(ID, &my_port);
      

 for (i = 0; i < burst-passed; i ++)
      {
       do{
       if(POWERSTATS)
      statobject->inspectAMBAAHBBridge(bridge_ID, 1, true);
          wait();
          request_to_push.write(false);
          master_port->ahb_interconnect_read(ID, &my_port);
          if (my_port.hresp != AHB_OKAY && (state != QUERY)){
               passed = i+passed;
               error_addr = prev_addr;
               error = true;
               my_port.htrans = AHB_IDLE;
               my_port.hbusreq = false;
               master_port->ahb_interconnect_write(ID, &my_port);
               state = QUERY;
                      }
        }while (my_port.hready != true);

       if(state == QUERY){
        if (STATS)
        statobject->endsAccess(!wr, burst, ID);
        if(POWERSTATS)
      statobject->inspectAMBAAHBBridge(bridge_ID, 1, false);
        wait();
        break;
        }

       din = my_port.hrdata;
       mast_pinout.data = din;

       if(din == 0xdeadbeef)
        printf("%s : din == 0xdeadbeef at %10.1f ns\n",
               name(), sc_simulation_time());
        ASSERT(din != 0xdeadbeef);

TRACEX(MASTER_TRACEX, 3, "%s: %s read 0x%08x at address 0x%08x at %10.1f ns\n%s",
    name(), burst == 1 ? "single" : "burst ", uint(din),
    (uint)prev_addr, sc_simulation_time(), (i == burst - 1) ? "\n" : "");

          
        prev_addr = addr;
        
        // Either ending the burst, or going on
        if (i >= burst - 2 - passed)
          my_port.htrans = AHB_IDLE;
        else
        {
          // Increment the address for the next beat
          addr += size;
          my_port.haddr = addr;
          my_port.htrans = AHB_SEQ;
        }
        if ((i == burst - 3 - passed) && (my_port.hburst == AHB_INCR)){ 
            my_port.hbusreq = false; 
            master_port->ahb_interconnect_write(ID, &my_port);
            } 

        master_port->ahb_interconnect_write(ID, &my_port);
        //PUSH
        pinout_to_rdf.write(mast_pinout);

        if(i==0 && push_full.read() == true){
        printf("%s : at %10.1f ns\n",name(),sc_simulation_time());
        ASSERT(push_full.read() != true);
        }
        if(i==0)
        ASSERT(write_overrun.read() != true);

        request_to_push.write(true);

      } // end for
        
      if (STATS)
        statobject->endsAccess(!wr, burst, ID);

     if(state == QUERY){
                   if (STATS)
                   statobject->busFreed(ID);
                   break;
                    }
     if(POWERSTATS)
      statobject->inspectAMBAAHBBridge(bridge_ID, 1, true);
      wait();
      
      request_to_push.write(false);
      state = WAITING;
      
TRACEX(MASTER_TRACEX, 5, "%s : request_to_push.write(false) at %10.1f ns\n",name(), sc_simulation_time());           
    
      if (STATS) 
      statobject->busFreed(ID);
      
      break;
    
    
case WRITE :

     if(burst==1){ //SINGLE WRITE
      next_dout = mast_pinout.data;
      my_port.hwrite = AHB_WRITE;
      
      // Let's request the bus
      master_port->ahb_interconnect_write(ID, &my_port);
      
      // Wait for the grant. Actually, we might get a zero-cycle
      // grant (e.g. if we're the only master on the bus)
      master_port->ahb_interconnect_read(ID, &my_port);
      while (!(my_port.hgrant && my_port.hready))
      {
      if(POWERSTATS)
      statobject->inspectAMBAAHBBridge(bridge_ID, 1, false);
        wait();
        master_port->ahb_interconnect_read(ID, &my_port);
      }

      my_port.hbusreq = false;

      master_port->ahb_interconnect_write(ID, &my_port);
      do
      {
      if(POWERSTATS)
      statobject->inspectAMBAAHBBridge(bridge_ID, 1, false);
        wait();
        master_port->ahb_interconnect_read(ID, &my_port);
        if (my_port.hresp != AHB_OKAY && (state != QUERY))
                      {
                       passed = 0;
                       error_addr = addr;
                       error = true;
                       my_port.htrans = AHB_IDLE;
                       my_port.hbusreq = false;
                       master_port->ahb_interconnect_write(ID, &my_port);
                       state = QUERY;
                      }
      } while (!(my_port.hready));
      
      if(state == QUERY){
      if(POWERSTATS)
      statobject->inspectAMBAAHBBridge(bridge_ID, 1, false);
                  wait();
                  break;
                  }
     
            
      if (STATS)
        statobject->beginsAccess(2, !wr, burst, ID);

      prev_addr = addr;
      dout = next_dout;
      my_port.hwdata = dout;   
      my_port.htrans = AHB_IDLE;
      master_port->ahb_interconnect_write(ID, &my_port);
          
      if(POWERSTATS)
      statobject->inspectAMBAAHBBridge(bridge_ID, 1, false);
      wait();
          
      master_port->ahb_interconnect_read(ID, &my_port);
      while (my_port.hready != true){
        if (my_port.hresp == AHB_ERROR && (state != QUERY))
                      {
                       error_addr = prev_addr;
                       passed = i+passed;
                       error = true;
                       my_port.htrans = AHB_IDLE;
                       my_port.hbusreq = false;
                       master_port->ahb_interconnect_write(ID, &my_port);
                       state = QUERY;
                      }
         if(POWERSTATS)
      statobject->inspectAMBAAHBBridge(bridge_ID, 1, false);
          wait();
           master_port->ahb_interconnect_read(ID, &my_port);
        }
       if(state == QUERY){
       if(POWERSTATS)
      statobject->inspectAMBAAHBBridge(bridge_ID, 1, false);
                    wait();
                    break;
                    }
       
        if (STATS)
          statobject->endsAccess(!wr, burst, ID);
  
  if(state == QUERY){
                 if (STATS)
                   statobject->busFreed(ID);
                    break;
                    }
    
 state = WAITING;
 if (STATS)
  statobject->busFreed(ID);     
    break;

    }      
   } 
  }   
 }   

  



