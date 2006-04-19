///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_ahb_bridge_sif.cpp
// author       DEIS - Universita' di Bologna
//              Martino Ruggiero - mruggiero@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// info         Implements the slave side of an AMBA AHB bridge.
//
///////////////////////////////////////////////////////////////////////////////

#include "amba_ahb_bridge_sif.h"
#include "stats.h"

///////////////////////////////////////////////////////////////////////////////
// addressing_ranges - Returns the number of addressing ranges that this bridge
//                     is mapping (usually just one, but MultiLayer devices
//                     will have more).
unsigned short int amba_ahb_bridge_sif::addressing_ranges()
{
  return BRIDGE_CONFIG[bridge_ID].n_ranges;
}

///////////////////////////////////////////////////////////////////////////////
// start_address - Returns the starting address for the range_ID addressing
//                 range mapped by the bridge.
unsigned long int amba_ahb_bridge_sif::start_address(unsigned short int range_ID)
{  
  return BRIDGE_CONFIG[bridge_ID].start_address[range_ID];
}

///////////////////////////////////////////////////////////////////////////////
// end_address - Returns the starting address for the range_ID addressing
//               range mapped by the bridge.
unsigned long int amba_ahb_bridge_sif::end_address(unsigned short int range_ID)
{
  return BRIDGE_CONFIG[bridge_ID].end_address[range_ID];
}

///////////////////////////////////////////////////////////////////////////////
// ahb_slave_query - Implements the slave query interface, which is used by the
//                   AHB IC to request transactions and evaluate responses.
//                   Upon first selection, the slave will sample control
//                   and address signals. Then, it will forward them across FIFOs
//                   to the master side of the bridge. During this time, control
//                   will simply be returned to the calling AHB layer while waiting
//                   for responses.
//                   *******************
//                   BEWARE: the slave MUST set a proper value for its outputs
//                   (hresp, hreadyout, hrdata, hsplit) before EVERY return!
//                   BEWARE: slave computation must complete within any number
//                   of delta cycles (i.e., be instantaneous or combinational).
//                   Sequential logic or delays are not allowed, since all of the
//                   slaves must be queried during any given clock cycle, so
//                   the total time for their queries must not exceed the clock
//                   period length.
void amba_ahb_bridge_sif::ahb_slave_query(unsigned char IC_ID, AHB_slave_port *port)
{
  unsigned long int data;

  am_i_selected(port);

  switch (state)
  {
    case NOT_SELECTED:  TRACEX(BRIDGE_TRACEX, 3, "%s : NOT_SELECTED at %10.1f ns\n",
                          name(), sc_simulation_time());
                        request_to_push.write(false);
                        request_to_pop.write(false);
                        beat = 0;
                        countwait = 0;
                        for (int i = 0; i < AHB_MAX_MASTERS; i ++)
                          port->hsplit[i] = false;
                        if (!ok_received &&
                            !is_write &&
                            (burst == 1  || !read_overrun.read()) &&
                            split_made &&
                            !pop_empty.read()
                           ) 
                        {
                          port->hsplit[master_ID] = true;
                          ok_received = true;
                        }
                        port->hreadyout = true;
                        port->hresp     = AHB_OKAY;
                        port->hrdata    = 0xdeadbeef;
                                 
                        if (POWERSTATS)
                          statobject->inspectAMBAAHBBridge(bridge_ID, 0, false);
                          
                        return;
        
    case TWO_CYCLES_ERROR_RESP: 
                        TRACEX(BRIDGE_TRACEX, 3, "%s : TWO_CYCLES_ERROR_RESP at %10.1f ns\n",
                          name(), sc_simulation_time());
                        for (int i = 0; i < AHB_MAX_MASTERS; i ++)
                          port->hsplit[i] = false;
                        port->hreadyout = true;
                        port->hresp     = AHB_ERROR;
                        port->hrdata    = 0xdeadbeef;
                        selected = false;
                        state = NOT_SELECTED;
                        beat = 0;
                        countwait = 0;
                        
                        if (POWERSTATS)
                          statobject->inspectAMBAAHBBridge(bridge_ID, 0, false);
                          
                        return;                    
           
    case SELECTED:      TRACEX(BRIDGE_TRACEX, 3, "%s : SELECTED at %10.1f ns\n",
                          name(), sc_simulation_time());
                        request_to_push.write(false); 
                        request_to_pop.write(false); 
                        if ((split_made && ((int)port->hmaster!=master_ID) && !ok_received) ||
                            (!split_made && push_full.read())
                           )
                        {//FIXME 2cycles_resp
                          for(int i = 0; i < AHB_MAX_MASTERS; i ++)
                            port->hsplit[i] = false;
                          port->hreadyout = false;
                          port->hresp     = AHB_ERROR;
                          port->hrdata    = 0xdeadbeef;
                          state = TWO_CYCLES_ERROR_RESP;
                          selected = true;
                          
                          if (POWERSTATS)
                            statobject->inspectAMBAAHBBridge(bridge_ID, 0, false);
                          
                          return;
                        }
                        
                        address = port->haddr;
                        request_made = false;
                        selected = true;
        
                        switch (port->hsize)
                        {
                          case AHB_WORD:     bw = MEM_WORD;
                                             break;
                          case AHB_HALFWORD: bw = MEM_HWORD;
                                             break;
                          case AHB_BYTE:     bw = MEM_BYTE;
                                             break;
                          default:           printf("Fatal error: %s: Wrong width %hu!\n",
                                               name(), (unsigned char)port->hsize);
                                             exit(1);
                        }
          
                        switch (port->hburst)
                        {
                          case AHB_SINGLE: burst = 1;
                                           break;  
                          case AHB_INCR4:  burst = 4;
                                           break;  
                          case AHB_INCR8:  burst = 8;
                                           break;  
                          case AHB_INCR16: burst = 16;
                                           break;  
                          case AHB_INCR:   burst = 16;
                                           break;  
                          default:         burst = 4;
                                           break;
                        }
                        
                        if (port->hwrite == AHB_WRITE)
                        {
                          is_write = true;
                          if (burst == 1)
                            state = SINGLE_WRITE;
                          else
                            state = BURST_WRITE;
                        }
                        else
                        {
                          is_write = false;
                          if (split_made == false)
                          {
                            state = MAKE_REQ_READ;
                            countwait=0;
                          }
                          else
                          {
                            port->hsplit[master_ID] = false;
                            if (burst == 1)
                              state = SINGLE_READ;
                            else 
                              state = BURST_READ;
                          }
                        }
                        
                        for (int i = 0; i < AHB_MAX_MASTERS; i ++)
                          port->hsplit[i] = false;
                        port->hreadyout = false;
                        port->hresp     = AHB_OKAY;
                        port->hrdata    = 0xdeadbeef;
                        
                        if (POWERSTATS)
                          statobject->inspectAMBAAHBBridge(bridge_ID, 0, false);
                          
                        return;
    
    case SINGLE_WRITE:  TRACEX(BRIDGE_TRACEX, 2, "%s : SINGLE_WRITE at %10.1f ns\n",
                          name(), sc_simulation_time());
    
                        if (push_full.read() == false)
                        {
                          data = port->hwdata;
                          ASSERT(data != 0xdeadbeef);
                          sl_pinout.data = data;
                          sl_pinout.address = address;
                          sl_pinout.rw = true;
                          sl_pinout.burst = burst;
                          sl_pinout.bw = bw;
                          pinout_to_wrf.write(sl_pinout);
                          request_to_push.write(true);
    
                          TRACEX(BRIDGE_TRACEX, 2,"%s : %s write at 0x%08x burst = %d at %10.1f ns\n",
                            name(), burst == 1 ? "single" : "burst ", address, burst, sc_simulation_time());
                          
                          for (int i = 0; i < AHB_MAX_MASTERS; i ++)
                            port->hsplit[i] = false;
                          port->hreadyout = true;
                          port->hresp     = AHB_OKAY;
                          port->hrdata    = 0xdeadbeef;
                          selected = false;
                          state = NOT_SELECTED;
                          beat = 0;
                          countwait = 0;
                          
                          if (POWERSTATS)
                            statobject->inspectAMBAAHBBridge(bridge_ID, 0, true);
                            
                          return;
                        }
                        else
                        { //FIXME 2cycles_resp
                          for (int i = 0; i < AHB_MAX_MASTERS; i ++)
                            port->hsplit[i] = false;
                          port->hreadyout = false;
                          port->hresp     = AHB_ERROR;
                          port->hrdata    = 0xdeadbeef;
                          state = TWO_CYCLES_ERROR_RESP;
                          selected = true;
                          
                          if (POWERSTATS)
                           statobject->inspectAMBAAHBBridge(bridge_ID, 0, false);
                          
                          return;
                        }
    
    case BURST_WRITE:   TRACEX(BRIDGE_TRACEX, 2, "%s : BURST_WRITE at %10.1f ns\n",
                          name(), sc_simulation_time());
                        if (countwait == 0 && write_overrun.read())
                        {
                          for (int i = 0; i < AHB_MAX_MASTERS; i ++)
                            port->hsplit[i] = false;
                          port->hreadyout = false;
                          port->hresp     = AHB_OKAY;
                          port->hrdata    = 0xdeadbeef;
                          if (POWERSTATS)
                           statobject->inspectAMBAAHBBridge(bridge_ID, 0, false);
                          return;
                        }
                        else
                        {
                          countwait ++;
                          data = port->hwdata;
                          ASSERT(data != 0xdeadbeef);
                          sl_pinout.data = data;
                          sl_pinout.address = address;
                          sl_pinout.rw = 1;
                          sl_pinout.burst = burst;
                          sl_pinout.bw = bw;
                          pinout_to_wrf.write(sl_pinout);
                          TRACEX(BRIDGE_TRACEX, 2, "%s : %s write at 0x%08x burst = %d at %10.1f ns\n",
                            name(), burst == 1 ? "single" : "burst ", address, burst, sc_simulation_time());
                          request_to_push.write(true);
                          for (int i = 0; i < AHB_MAX_MASTERS; i ++)
                            port->hsplit[i] = false;
                          port->hreadyout = true;
                          port->hresp     = AHB_OKAY;
                          port->hrdata    = 0xdeadbeef;
                          selected = true;
                          
                          if (beat == burst)
                          {
                            request_to_push.write(false);
                            selected = false;
                            state = NOT_SELECTED;
                            beat ++;
                             
                            if (POWERSTATS)
                              statobject->inspectAMBAAHBBridge(bridge_ID, 0, true);
                              
                            return;
                          }
                          else
                          {
                            beat ++;
                            
                            if (POWERSTATS)
                              statobject->inspectAMBAAHBBridge(bridge_ID, 0, true);
                            
                            return;
                          }
                        }

    case MAKE_REQ_READ: TRACEX(BRIDGE_TRACEX, 3, "%s : MAKE_REQ_READ at %10.1f ns\n",
                          name(), sc_simulation_time());
                         
                        if (push_full.read() == false)
                        {
                          master_ID = (int) port->hmaster;
                          sl_pinout.address = address;
                          sl_pinout.rw = 0;
                          sl_pinout.burst = burst;
                          sl_pinout.bw = bw;
                          pinout_to_wrf.write(sl_pinout);
                          request_to_push.write(true);
                          request_made = true;
                          countwait ++;
                        } 
                        else
                        {
                          if (request_made)
                            countwait++;
                          else
                            // FIXME add an error message
                            exit(1);
                        }
                        
                        port->hreadyout = false;
                        port->hresp     = AHB_SPLIT;
                        port->hrdata    = 0xdeadbeef;
                        
                        if (request_made && split_made)
                        {
                          if (countwait > 1)
                          request_to_push.write(false);
                          port->hreadyout = true;
                          state = NOT_SELECTED;
                          beat = 0;
                          countwait = 0;
                          selected = false;
                        }
                        split_made = true;
                        ok_received = false;
                        for (int i = 0; i < AHB_MAX_MASTERS; i ++)
                        port->hsplit[i] = false;
                        
                        if (POWERSTATS)
                          statobject->inspectAMBAAHBBridge(bridge_ID, 0, true);
                        
                        return;
    
    case SINGLE_READ:   TRACEX(BRIDGE_TRACEX, 3, "%s : SINGLE_READ at %10.1f ns\n",
                          name(), sc_simulation_time());
                        if(master_ID != (int) port->hmaster)
                        { //FIXME 2cycles_resp
                          for (int i = 0; i < AHB_MAX_MASTERS; i ++)
                            port->hsplit[i] = false;
                          port->hreadyout = false;
                          port->hresp     = AHB_ERROR;
                          port->hrdata    = 0xdeadbeef;
                          state = TWO_CYCLES_ERROR_RESP;
                          selected = true;
                          
                          if (POWERSTATS)
                            statobject->inspectAMBAAHBBridge(bridge_ID, 0, false);
                          
                          return;
                        }
                        request_to_pop.write(true);
                        ok_received = false;
                        if (countwait < 2)
                        {
                          if (countwait >= 1)
                            request_to_pop.write(false);
                          port->hreadyout = false;
                          port->hresp     = AHB_OKAY;
                          port->hrdata    = 0xdeadbeef; 
                          countwait ++;
                          for (int i = 0; i < AHB_MAX_MASTERS; i ++)
                            port->hsplit[i] = false;
                            
                          if (POWERSTATS)
                            statobject->inspectAMBAAHBBridge(bridge_ID, 0, false);
                            
                          return;
                        }
                        
                        request_to_pop.write(false);

                        sl_pinout = pinout_from_rdf.read();
                        data = sl_pinout.data;
                        ASSERT(data != 0xdeadbeef);
                        TRACEX(BRIDGE_TRACEX, 7, "%s : data = 0x%08x at %10.1f ns\n",
                          name(), data, sc_simulation_time());
                        port->hreadyout = true;
                        port->hresp     = AHB_OKAY;
                        port->hrdata    = data;
                        selected = false;
                        state = NOT_SELECTED;
                        beat = 0;
                        countwait = 0;
                        split_made = false;
                        for (int i = 0; i < AHB_MAX_MASTERS; i ++)
                          port->hsplit[i] = false;
                          
                        if (POWERSTATS)
                          statobject->inspectAMBAAHBBridge(bridge_ID, 0, true);
                          
                        return;
    
    case BURST_READ:    TRACEX(BRIDGE_TRACEX, 3, "%s : BURST_READ at %10.1f ns\n",
                          name(), sc_simulation_time());
                        if (master_ID != (int) port->hmaster)
                        { //FIXME 2cycles_resp
                          for (int i = 0; i < AHB_MAX_MASTERS; i ++)
                            port->hsplit[i] = false;
                          port->hreadyout = false;
                          port->hresp     = AHB_ERROR;
                          port->hrdata    = 0xdeadbeef;
                          state = TWO_CYCLES_ERROR_RESP;
                          selected = true;

                          if (POWERSTATS)
                            statobject->inspectAMBAAHBBridge(bridge_ID, 0, false);
                            
                          return;
                        }
                        request_to_pop.write(true);
                        ok_received = false;
                              
                        if (countwait < 2)
                        {      
                          port->hreadyout = false;
                          port->hresp     = AHB_OKAY;
                          port->hrdata    = 0xdeadbeef; 
                          countwait++;    
                          for (int i = 0; i < AHB_MAX_MASTERS; i ++)
                            port->hsplit[i] = false;
                            
                          if (POWERSTATS)
                            statobject->inspectAMBAAHBBridge(bridge_ID, 0, false);
                            
                          return;
                        }
                        
                        sl_pinout = pinout_from_rdf.read();
                        data = sl_pinout.data;
                        ASSERT(data != 0xdeadbeef);
                        TRACEX(BRIDGE_TRACEX, 7, "%s : data = 0x%08x at %10.1f ns\n",
                          name(), data, sc_simulation_time());
                        port->hreadyout = true;
                        port->hresp     = AHB_OKAY;
                        port->hrdata    = data;
                        selected = true;
                        for (int i = 0; i < AHB_MAX_MASTERS; i ++)
                          port->hsplit[i] = false;
                        if (beat >= burst - 2)
                        {
                          request_to_pop.write(false);
                          if (beat == burst - 1)
                          {
                            selected = false;
                            state = NOT_SELECTED;
                            beat = 0;
                            countwait = 0;
                            split_made = false;
                          }
                        }
                          
                        beat ++;
                        
                        if (POWERSTATS)
                          statobject->inspectAMBAAHBBridge(bridge_ID, 0, true);
                            
                        return;
  }
}

///////////////////////////////////////////////////////////////////////////////
// am_i_selected - Checks the slave pinout and the previous status flag to
//                 determine whether the AHB layer is selecting the bridge.
void amba_ahb_bridge_sif::am_i_selected(AHB_slave_port *port)
{
  if ( !
       (
         (
          port->hsel == true &&
          port->hready == true &&
          (port->htrans == AHB_NONSEQ || port->htrans == AHB_SEQ)
         ) ||
         selected
       )
     )
    state = NOT_SELECTED;
  else if (state == NOT_SELECTED)
    state = SELECTED;
}
