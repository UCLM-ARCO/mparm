///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_ahb_mlicm.cpp
// author       DEIS - Universita' di Bologna
//              Martino Ruggiero - mruggiero@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
//              Davide Bertozzi - dbertozzi@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Implements an AMBA AHB MultiLayer InterConnect Matrix.
//
///////////////////////////////////////////////////////////////////////////////

#include "amba_ahb_mlicm.h"

///////////////////////////////////////////////////////////////////////////////
// register_port - Executed every time the ICM is bound to a port (i.e. to a
//                 layer). Used to autocompute the number of attached layers.
void amba_ahb_mlicm::register_port(sc_port_base& port_, const char* if_typename_)
{
  m_nlayers ++;
#if 0
  printf("%s: m_nlayers = %d\n", name(), m_nlayers);
#endif
  
  if (m_nlayers > AHB_MAX_ICM_LAYERS - 1)
  {
    printf("Fatal Error: no more than %hu layers can be attached to an AMBA AHB ICM device (attempt to attach %hu to %s)!\n",
      AHB_MAX_ICM_LAYERS - 1, m_nlayers, name());
    exit(1);
  }
}

///////////////////////////////////////////////////////////////////////////////
// end_of_elaboration - Executed after port binding. Queries attached slaves
//                      to build a decoding map for the decoder, and checks
//                      to detect any overlapping range. Additionally, it
//                      establishes tracing for the internal signals.
void amba_ahb_mlicm::end_of_elaboration()
{
  bool no_overlap;
  unsigned long int current_counter;
  unsigned short int i, j;

  m_nslaves = (unsigned short int)icm_slave_port.size();
  range_count = 0;
    
  for (i = 0; i < icm_slave_port.size(); i ++)
  {
    ahb_slave_if *slave1 = icm_slave_port[i];
    range_count += slave1->addressing_ranges();
  }
  
  //FIXME should be in reset_ic, but m_nslaves is unknown there
  for (unsigned char i = 0; i < m_nslaves; i ++)
    assigned_slave_table[i] = -1;
  
  starting_addresses = new unsigned long int [range_count];
  ending_addresses = new unsigned long int [range_count];
  mapping_target = new unsigned short int [range_count];
  
  for (i = 0, current_counter = 0; i < icm_slave_port.size(); i ++)
  {
    ahb_slave_if *slave1 = icm_slave_port[i];
    
    for (j = 0; j < slave1->addressing_ranges(); j ++)
    {
      starting_addresses[current_counter] = slave1->start_address(j);
      ending_addresses[current_counter] = slave1->end_address(j);
      mapping_target[current_counter] = i;
      
#if 0
      printf("%s: starting_address 0x%08lx - ending_address 0x%08lx for ICM slave %hu\n",
        name(), starting_addresses[current_counter], ending_addresses[current_counter],
        mapping_target[current_counter]);
#endif
      
      if (starting_addresses[current_counter] >= ending_addresses[current_counter])
      {
        printf("Fatal error: %s: Invalid address range 0x%08lx - 0x%08lx for slave %hu!\n",
          name(), starting_addresses[current_counter], ending_addresses[current_counter],
          mapping_target[current_counter]);
        exit(1);
      }
      
      current_counter ++;
    }
  }
  
  for (i = 0; i < range_count; i ++)
  {
    for (j = i + 1; j < range_count; j ++)
    {
      no_overlap = (ending_addresses[i] < starting_addresses[j]) || (starting_addresses[i] > ending_addresses[j]);
      if (!no_overlap)
      {
        printf("Fatal error: %s: Overlapping slave ranges found: 0x%08lx - 0x%08lx and 0x%08lx - 0x%08lx!\n",
          name(), starting_addresses[i], ending_addresses[i], starting_addresses[j], ending_addresses[j]);
        exit(1);
      }
    }
  }
  
  // Tracing for internal signals (higher-level ones will be managed
  // by higher-level instantiation code)
  if (tracing)
  {
    char buffer[100];
    unsigned char i;
    
    // Slave-side signals
    // ------------------
    for (i = 0; i < m_nslaves; i ++)
    {
      sprintf(buffer, "%s_assigned_slave_table_%d", name(), i);
      sc_trace(tf, assigned_slave_table[i], buffer);
#if 0
      sprintf(buffer,     "%s_slave_s_hrdata_%d",     name(), i);
      sc_trace(tracefile, s_hrdata[i],          buffer);
      sprintf(buffer,     "%s_slave_s_hready_%d",     name(), i);
      sc_trace(tracefile, s_hready[i],          buffer);
      sprintf(buffer,     "%s_slave_hsel_%d",         name(), i);
      sc_trace(tracefile, hsel[i],              buffer);
      sprintf(buffer,     "%s_slave_delayed_hsel_%d", name(), i);
      sc_trace(tracefile, delayed_hsel[i],      buffer);
      sprintf(buffer,     "%s_slave_s_hresp_%d",      name(), i);
      sc_trace(tracefile, s_hresp[i],           buffer);
#endif
#if 0
      for (i = AHB_BOOT_MASTER; i <= AHB_BOOT_MASTER + m_nmasters; i ++)
      {
        // Skip the default master
        if (master_present[i])
        {
          sprintf(buffer, "%s_slave_hsplit_s%d_m%d", name(), i, m);
          sc_trace(tracefile, hsplit[i][m], buffer);    
        }
      }
#endif
    }

    // Layer-side signals
    // ------------------
    for (i = 0; i < m_nlayers; i ++)
    {
      sprintf(buffer,     "%s_ready_count_%d",            name(), i);
      sc_trace(tf, ready_count[i],                 buffer);
      sprintf(buffer,     "%s_selected_%d",               name(), i);
      sc_trace(tf, selected[i],                    buffer);
      sprintf(buffer,     "%s_blocked_on_input_stage_%d", name(), i);
      sc_trace(tf, blocked_on_input_stage[i],      buffer);
      sprintf(buffer,     "%s_burst_%d",                  name(), i);
      sc_trace(tf, burst[i],                       buffer);
      sprintf(buffer,     "%s_slave_ID_%d",               name(), i);
      sc_trace(tf, slave_ID[i],                    buffer);
      sprintf(buffer,     "%s_release_slave_%d",          name(), i);
      sc_trace(tf, release_slave[i],               buffer);
#if 0
      sprintf(buffer,     "%s_layer_s_hrdata_%d",     name(), i);
      sc_trace(tracefile, s_hrdata[i],          buffer);
      sprintf(buffer,     "%s_layer_s_hready_%d",     name(), i);
      sc_trace(tracefile, s_hready[i],          buffer);
      sprintf(buffer,     "%s_layer_hsel_%d",         name(), i);
      sc_trace(tracefile, hsel[i],              buffer);
      sprintf(buffer,     "%s_layer_delayed_hsel_%d", name(), i);
      sc_trace(tracefile, delayed_hsel[i],      buffer);
      sprintf(buffer,     "%s_layer_s_hresp_%d",      name(), i);
      sc_trace(tracefile, s_hresp[i],           buffer);
#endif
#if 0
      for (i = AHB_BOOT_MASTER; i <= AHB_BOOT_MASTER + m_nmasters; i ++)
      {
        // Skip the default master
        if (master_present[i])
        {
          sprintf(buffer, "%s_layer_hsplit_s%d_m%d", name(), i, m);
          sc_trace(tracefile, hsplit[i][m], buffer);    
        }
      }
#endif
    }
    
#if 0
    AHB_slave_port slave_port[AHB_MAX_ICM_LAYERS];
#endif
  }
}

///////////////////////////////////////////////////////////////////////////////
// reset_icm - Provides reset functionality.
void amba_ahb_mlicm::reset_icm()
{
  for (int i = 0; i < AHB_MAX_ICM_LAYERS; i ++)
  {
    ready_count[i] = 0;
    blocked_on_input_stage[i] = false;
    selected[i] = false;
    release_slave[i] = false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// addressing_ranges - Returns the number of addressing ranges that this slave
//                     is mapping (usually just one, but MultiLayer devices
//                     will have more).
unsigned short int amba_ahb_mlicm::addressing_ranges()
{
  return range_count;
}

///////////////////////////////////////////////////////////////////////////////
// start_address - Returns the starting address for the range_ID addressing
//                 range mapped by the slave.
unsigned long int amba_ahb_mlicm::start_address(unsigned short int range_ID)
{  
  return starting_addresses[range_ID];
}

///////////////////////////////////////////////////////////////////////////////
// end_address - Returns the starting address for the range_ID addressing
//               range mapped by the slave.
unsigned long int amba_ahb_mlicm::end_address(unsigned short int range_ID)
{
  return ending_addresses[range_ID];
}

///////////////////////////////////////////////////////////////////////////////
// ahb_slave_query - Implements the slave read interface, which is used by the
//                   AHB ICs (attached layers) to request transactions.
//                   Upon first selection, the ICM will sample control
//                   and address signals. Then, it will skip wait states (if
//                   any) by just returning control to the AMBA AHB layer after
//                   taking note of the current progress. Finally, the real
//                   transaction will be performed by forwarding the transaction to
//                   the proper attached slave.
//                   *******************
//                   BEWARE: the ICM MUST set a proper value for its slave outputs
//                   (hresp, hreadyout, hrdata, hsplit) before EVERY return!
//                   BEWARE: ICM computation must complete within any number
//                   of delta cycles (i.e., be instantaneous or combinational).
//                   Sequential logic or delays are not allowed, since all of the
//                   slaves must be queried during any given clock cycle, so
//                   the total time for their queries must not exceed the clock
//                   period length.
void amba_ahb_mlicm::ahb_slave_query(unsigned char IC_ID, AHB_slave_port *port)
{
  // We can now release the slave owned by layer IC_ID, since the
  // transaction was completed in the previous cycle
  if (release_slave[IC_ID])
  {
    assigned_slave_table[slave_ID[IC_ID]] = -1;
    release_slave[IC_ID] = false;
  }

  if (!((port->hsel == true &&
        port->hready == true &&
        (port->htrans == AHB_NONSEQ || port->htrans == AHB_SEQ)) || selected[IC_ID]))
  {
    // Not selected
    port->hreadyout = true;
    port->hresp     = AHB_OKAY;
    port->hrdata    = 0xdeadbeef;
    for (int i = 0; i < AHB_MAX_MASTERS; i ++)
      port->hsplit[i] = false;
    return;
  }
  else
  {
    // Selected
    TRACEX(WHICH_TRACEX, 12, "%s: Selected by layer %hu (wanting address 0x%08x of slave %u) at %10.1f ns\n",
      name(), IC_ID, (uint)port->haddr, slave_ID[IC_ID], sc_simulation_time());
    
    // If we're still trying to get slave access,
    // let's remember relevant info
    if (assigned_slave_table[slave_ID[IC_ID]] != IC_ID && (ready_count[IC_ID] == 0))
    {

      slave_ID[IC_ID] = decode(IC_ID, port); // ID of the slave requested by layer IC_ID
      selected[IC_ID] = true;                // this ICM is the selected slave on layer IC_ID
      
      switch (port->hburst)
      {
        case AHB_SINGLE: burst[IC_ID] = 1;
                         break;
        case AHB_INCR4:  burst[IC_ID] = 4;
                         break;
        case AHB_INCR8:  burst[IC_ID] = 8;
                         break;
        case AHB_INCR16: burst[IC_ID] = 16;
                         break;
        case AHB_INCR:   burst[IC_ID] = port->hburst; //FIXME only LX is maybe doing it right...
                         break;
        default:         burst[IC_ID] = 4;
                         break;
      }
    }
    
    // Layer IC_ID wants to talk to slave_ID[IC_ID], let's copy
    // its pinout to be able to pass it to that slave. If other
    // layers own the target slave, this layer will lose arbitration
    // at the ICM input stage; make sure that we don't overwrite
    // the pinout with subsequent garbage in that case
    if (!blocked_on_input_stage[IC_ID])
    {
      slave_port[IC_ID].hsel      = true;
      slave_port[IC_ID].haddr     = port->haddr;
      slave_port[IC_ID].hwrite    = port->hwrite;
      slave_port[IC_ID].htrans    = port->htrans;
      slave_port[IC_ID].hsize     = port->hsize;
      slave_port[IC_ID].hburst    = port->hburst;
      slave_port[IC_ID].hwdata    = port->hwdata;
      slave_port[IC_ID].hmaster   = port->hmaster;
      slave_port[IC_ID].hready    = port->hready;
      slave_port[IC_ID].hmastlock = port->hmastlock;
    }
    
    // The desired slave is assigned to another layer, let's return HREADY low
    // to the caller
    if (assigned_slave_table[slave_ID[IC_ID]] != -1 && assigned_slave_table[slave_ID[IC_ID]] != IC_ID)
    {
      TRACEX(WHICH_TRACEX, 11, "%s: Calling layer %hu wants slave %u, which is assigned to layer %u at %10.1f ns\n",
        name(), IC_ID, slave_ID[IC_ID], assigned_slave_table[slave_ID[IC_ID]], sc_simulation_time());
      
      // This layer is accessing something, but is blocked by another layer
      blocked_on_input_stage[IC_ID] = true;
 
      port->hreadyout = false;
      port->hresp     = AHB_OKAY;
      port->hrdata    = 0xdeadbeef;
      for (int i = 0; i < AHB_MAX_MASTERS; i ++)
        port->hsplit[i] = false;
      return;
    }
    
    // The layer passed arbitration against others
    blocked_on_input_stage[IC_ID] = false;

    // Let's take control of the desired slave
    assigned_slave_table[slave_ID[IC_ID]] = IC_ID;
    
    TRACEX(WHICH_TRACEX, 1, "%s: Calling layer %hu sets slave %u as assigned to itself at %10.1f ns\n",
      name(), IC_ID, slave_ID[IC_ID], sc_simulation_time());
    
    // Let's query the desired slave
    icm_slave_port[slave_ID[IC_ID]]->ahb_slave_query(IC_ID, &slave_port[IC_ID]);
    
    // Let's give a response to the calling layer
    port->hresp = slave_port[IC_ID].hresp;
    port->hreadyout = slave_port[IC_ID].hreadyout;
    if (slave_port[IC_ID].hreadyout == true)
    {
      // This is a READY, so remember that the transaction is progressing
      ready_count[IC_ID] ++;
      TRACEX(WHICH_TRACEX, 12, "%s: Got HREADY, ready_count[%u] = %u at %10.1f ns\n",
        name(), IC_ID, ready_count[IC_ID], sc_simulation_time());
    }
    port->hrdata = slave_port[IC_ID].hrdata;
    for (int m = 0; m < AHB_MAX_MASTERS; m++)
      port->hsplit[m] = slave_port[IC_ID].hsplit[m];

    TRACEX(WHICH_TRACEX, 9, "%s: Returning response to layer %u: HREADY = %d, HRDATA = 0x%08x at %10.1f ns\n",
      name(), IC_ID, port->hreadyout, (uint)port->hrdata, sc_simulation_time());
    
    // Let's see if the number of READY responses equals the number of burst
    // beats to be performed. If yes, we must release the slave
    if (ready_count[IC_ID] == burst[IC_ID])
    {
      // We must release the slave only during next cycle, otherwise other
      // layers might access it in this same clock cycle after some deltas.
      // This safety measure is a potential speed penalty FIXME
      release_slave[IC_ID] = true;
      ready_count[IC_ID] = 0;
      selected[IC_ID] = false;
    }
    
    return;
  }
}


///////////////////////////////////////////////////////////////////////////////
// decode - Provides AMBA AHB ICM decoding. Decoding is based upon static maps
//          built during the end_of_elaboration() stage. Any unknown address
//          stops the simulation with an error.
short int amba_ahb_mlicm::decode(unsigned char IC_ID, AHB_slave_port *port)
{
  short int slave = -1;
  unsigned char i;

  for (i = 0; i < range_count; i ++)
    if (port->haddr >= starting_addresses[i] && port->haddr <= ending_addresses[i])
    {
      slave = mapping_target[i];
      break;
    }

  if (i == range_count && (port->htrans == AHB_NONSEQ || port->htrans == AHB_SEQ))
  {
    //FIXME This should actually be a default slave access with OKAY (htrans = IDLE, BUSY)
    //or ERROR (htrans = NONSEQ, SEQ) response.
    printf("Fatal error: in %s, decoding memory address 0x%08x to any slave is impossible!\n",
      name(), (unsigned int)port->haddr);
    exit(1);
  }

#ifdef PRINTDEBUG
  printf("Decoding finished, address 0x%08x mapped to slave %d\n", (uint32_t)port->haddr, slave);
#endif

  return slave;
}
