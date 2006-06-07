///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_ahb_interconnect.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Describes an AMBA AHB interconnect.
//
///////////////////////////////////////////////////////////////////////////////

#include "amba_ahb_interconnect.h"
#include "debug.h"
#include "power.h"

///////////////////////////////////////////////////////////////////////////////
// amba_ahb_interconnect - This class encapsulates an AMBA AHB interconnect.
//                         nm: IC name
//                         ID: IC ID (multiple ICs are present in a ML setup!)
//                         tf: VCD trace file (used if tracing is enabled)
//                         tracing: enables/disables VCD tracing
amba_ahb_interconnect::amba_ahb_interconnect(sc_module_name nm,
                                             unsigned char ID,
                                             sc_trace_file *tf,
                                             bool tracing = false) : sc_module(nm),
                                                                     ID(ID),
                                                                     tracefile(tf),
                                                                     tracing(tracing)
{
  m_nmasters = 0;
  
  identified_masters = 0;
  last_master_id = AHB_BOOT_MASTER - 1;

  reset_ic();

  SC_METHOD(operate);
  dont_initialize();
  sensitive << clock.pos();
}

///////////////////////////////////////////////////////////////////////////////
// end_of_elaboration - Executed after port binding. Queries attached slaves
//                      to build a decoding map for the decoder, and checks
//                      to detect any overlapping range. Additionally, it
//                      establishes tracing for the internal signals.
void amba_ahb_interconnect::end_of_elaboration()
{
  bool no_overlap;
  unsigned long int current_counter;
  unsigned short int i, j;
  
  // If the default master overlaps with the range of master IDs,
  // skip it from "typical" master operations
  if (AHB_DEFAULT_MASTER >= AHB_BOOT_MASTER && AHB_DEFAULT_MASTER < (AHB_BOOT_MASTER + m_nmasters))
  {
    for (i = 0; i < AHB_MAX_MASTERS; i ++)
    {
      if (i >= AHB_BOOT_MASTER && i <= (AHB_BOOT_MASTER + m_nmasters) && i!= AHB_DEFAULT_MASTER)
        master_present[i] = true;
      else
        master_present[i] = false;
    }
  }
  else
  {
    for (i = 0; i < AHB_MAX_MASTERS; i ++)
    {
      if (i >= AHB_BOOT_MASTER && i < (AHB_BOOT_MASTER + m_nmasters))
        master_present[i] = true;
      else
        master_present[i] = false;
    }
  }
  
  m_nslaves = (unsigned short int)slave_port.size();
  range_count = 0;
  
  if (POWERSTATS)
    monitor_activity.n_slaves = m_nslaves;

  //FIXME should be in reset_ic, but m_nslaves is unknown there
  for (unsigned char s = 0; s < m_nslaves; s ++)
    hsel[s] = false;
    
  //FIXME should be in reset_ic
  for (unsigned char m = AHB_BOOT_MASTER; m <= AHB_BOOT_MASTER + m_nmasters; m ++)
  {
    // Skip the default master
    if (master_present[i])
    {
      m_hgrant[m] = false;
      m_hgrant_next[m] = false;
      m_hlock[m] = false;
      m_hlock_next[m] = false;
    }
  }

  for (i = 0; i < slave_port.size(); i ++)
  {
    ahb_slave_if *slave1 = slave_port[i];
    range_count += slave1->addressing_ranges();
  }
  
  starting_addresses = new unsigned long int [range_count];
  ending_addresses = new unsigned long int [range_count];
  mapping_target = new unsigned short int [range_count];
  
  for (i = 0, current_counter = 0; i < slave_port.size(); i ++)
  {
    ahb_slave_if *slave1 = slave_port[i];
    
    for (j = 0; j < slave1->addressing_ranges(); j ++)
    {
      starting_addresses[current_counter] = slave1->start_address(j);
      ending_addresses[current_counter] = slave1->end_address(j);
      mapping_target[current_counter] = i;
      
#if 0
      printf("%s: starting_address 0x%08lx - ending_address 0x%08lx for slave %hu\n",
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
  
  certainly_valid_address = starting_addresses[0];

  // Tracing for internal signals (higher-level ones will be managed
  // by higher-level instantiation code)
  if (tracing)
  {
    char buffer[100];
    unsigned char i;

    // System-wide signals
    // -------------------
    sprintf(buffer,     "%s_haddr",             name());
    sc_trace(tracefile, haddr,                  buffer);
    sprintf(buffer,     "%s_hwdata",            name());
    sc_trace(tracefile, hwdata,                 buffer);
    sprintf(buffer,     "%s_hwrite",            name());
    sc_trace(tracefile, hwrite,                 buffer);
    sprintf(buffer,     "%s_htrans",            name());
    sc_trace(tracefile, htrans,                 buffer);
    sprintf(buffer,     "%s_hsize",             name());
    sc_trace(tracefile, hsize,                  buffer);
    sprintf(buffer,     "%s_hburst",            name());
    sc_trace(tracefile, hburst,                 buffer);
    sprintf(buffer,     "%s_hrdata",            name());
    sc_trace(tracefile, hrdata,                 buffer);
    sprintf(buffer,     "%s_hready",            name());
    sc_trace(tracefile, hready,                 buffer);
    sprintf(buffer,     "%s_hmaster",           name());
    sc_trace(tracefile, hmaster,                buffer);
    sprintf(buffer,     "%s_delayed_hmaster",   name());
    sc_trace(tracefile, delayed_hmaster,        buffer);
    sprintf(buffer,     "%s_hresp",             name());
    sc_trace(tracefile, hresp,                  buffer);
    sprintf(buffer,     "%s_hmastlock",         name());
    sc_trace(tracefile, hmastlock,              buffer);
    
    // Master signals
    // --------------
    for (i = AHB_BOOT_MASTER; i <= AHB_BOOT_MASTER + m_nmasters; i ++)
    {
      // Skip the default master
      if (master_present[i])
      {
        sprintf(buffer,     "%s_m_hgrant_%d",     name(), i);
        sc_trace(tracefile, m_hgrant[i],          buffer);
        sprintf(buffer,     "%s_m_htrans_%d",     name(), i);
        sc_trace(tracefile, m_htrans[i],          buffer);
        sprintf(buffer,     "%s_m_hwrite_%d",     name(), i);
        sc_trace(tracefile, m_hwrite[i],          buffer);
        sprintf(buffer,     "%s_m_hsize_%d",      name(), i);
        sc_trace(tracefile, m_hsize[i],           buffer);
        sprintf(buffer,     "%s_m_hburst_%d",     name(), i);
        sc_trace(tracefile, m_hburst[i],          buffer);
        sprintf(buffer,     "%s_m_haddr_%d",      name(), i);
        sc_trace(tracefile, m_haddr[i],           buffer);
        sprintf(buffer,     "%s_m_hwdata_%d",     name(), i);
        sc_trace(tracefile, m_hwdata[i],          buffer);
        sprintf(buffer,     "%s_m_hbusreq_%d",    name(), i);
        sc_trace(tracefile, m_hbusreq[i],         buffer);
        sprintf(buffer,     "%s_m_hlock_%d",      name(), i);
        sc_trace(tracefile, m_hlock[i],           buffer); 
      }
    }

    // Slave signals
    // -------------
    for (i = 0; i < m_nslaves; i ++)
    {
      sprintf(buffer,     "%s_s_hrdata_%d",     name(), i);
      sc_trace(tracefile, s_hrdata[i],          buffer);
      sprintf(buffer,     "%s_s_hready_%d",     name(), i);
      sc_trace(tracefile, s_hready[i],          buffer);
      sprintf(buffer,     "%s_hsel_%d",         name(), i);
      sc_trace(tracefile, hsel[i],              buffer);
      sprintf(buffer,     "%s_delayed_hsel_%d", name(), i);
      sc_trace(tracefile, delayed_hsel[i],      buffer);
      sprintf(buffer,     "%s_s_hresp_%d",      name(), i);
      sc_trace(tracefile, s_hresp[i],           buffer);
#if 0
      for (i = AHB_BOOT_MASTER; i <= AHB_BOOT_MASTER + m_nmasters; i ++)
      {
        // Skip the default master
        if (master_present[i])
        {
          sprintf(buffer, "%s_hsplit_s%d_m%d", name(), i, m);
          sc_trace(tracefile, hsplit[i][m], buffer);    
        }
      }
#endif
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// register_port - Executed every time the IC is bound to a port (i.e. to a
//                 master). Used to autocompute the number of attached masters.
void amba_ahb_interconnect::register_port(sc_port_base& port_, const char* if_typename_)
{
  m_nmasters ++;
  
  if (POWERSTATS)
    monitor_activity.n_masters ++;
  
  if (m_nmasters > AHB_MAX_MASTERS - 1)
  {
    printf("Fatal Error: no more than %hu masters can be attached to an AMBA AHB layer (attempt to attach %hu to %s)!\n",
      AHB_MAX_MASTERS - 1, m_nmasters, name());
    exit(1);
  }
}

///////////////////////////////////////////////////////////////////////////////
// get_master_ID - Returns a master ID local to this bus.
unsigned char amba_ahb_interconnect::get_master_ID()
{
  identified_masters ++;
  last_master_id ++;
  
  // Skip one more ID if overlapping with the default master
  if (last_master_id == AHB_DEFAULT_MASTER)
    last_master_id ++;
  
  ASSERT(last_master_id >= 0);
  
  if (identified_masters > m_nmasters)
  {
    printf("Fatal Error: %hu masters registered for layer %s, but %hu are requesting an ID!\n",
      m_nmasters, name(), identified_masters);
    exit(1);
  }
  if (last_master_id > AHB_MAX_MASTERS - 1)
  {
    printf("Fatal Error: when assigning master IDs in %s, the sequence went beyond the maximum allowed of %hu!\n",
      name(), AHB_MAX_MASTERS - 1);
    exit(1);
  }
 
  return ((unsigned char)last_master_id);
  
  //FIXME It would be nice to check whether ALL of the bound masters make this call
}

///////////////////////////////////////////////////////////////////////////////
// reset_ic - Provides reset functionality.
void amba_ahb_interconnect::reset_ic()
{
  haddr                         = certainly_valid_address;
  hmaster                       = (sc_uint<AHB_HMASTER_BITS>)(AHB_DEFAULT_MASTER);
  hmaster_next                  = (sc_uint<AHB_HMASTER_BITS>)(AHB_DEFAULT_MASTER);
  m_hwdata[AHB_DEFAULT_MASTER]  = 0xdeadbeef;
  m_hbusreq[AHB_DEFAULT_MASTER] = false;
  hready                        = true;
  m_hlock[AHB_DEFAULT_MASTER]   = false;
  hmastlock                     = false;
  
  next_ic_state                 = ic_sampling;
  next_arb_state                = arb_init;
  arb_previous                  = AHB_BOOT_MASTER - 1;
  arb_prev                      = AHB_DEFAULT_MASTER;
  arb_burst                     = 1;
  
  for (int i = 0; i < AHB_MAX_SLAVES; i ++)
    for (int m = 0; m < AHB_MAX_MASTERS; m ++)
      hsplit[i][m] = false;
  
  for (int i = 0; i < AHB_MAX_MASTERS; i ++)
    arb_master_split[i] = false;
  
  arb_sampled_first       = true;
  arb_counter             = 31;  //FIXME why fixed value?
  arb_rearbitrate         = false;
  
  for (int i = 0; i < AHB_MAX_MASTERS; i ++) 
    arb_bus_locked[i] = false;
  
  monitor_activity.htrans          = false;
  monitor_activity.hmaster         = false;
  monitor_activity.hready          = false;
  monitor_activity.n_masters       = 0;
  monitor_activity.n_slaves        = 0;
}

///////////////////////////////////////////////////////////////////////////////
// ahb_interconnect_read - Implements the IC read interface, which is used by
//                         AHB masters to query the IC status.
void amba_ahb_interconnect::ahb_interconnect_read(unsigned char master_ID, AHB_master_port *port)
{
  port->hgrant = m_hgrant[master_ID];
  port->hready = hready;
  port->hrdata = hrdata;
  port->hresp  = hresp;
}

///////////////////////////////////////////////////////////////////////////////
// ahb_interconnect_write - Implements the IC write interface, which is used by
//                          AHB masters to execute transactions.
void amba_ahb_interconnect::ahb_interconnect_write(unsigned char master_ID, AHB_master_port *port)
{
  m_hburst_next[master_ID]  = port->hburst;
  m_htrans_next[master_ID]  = port->htrans;
  m_hsize_next[master_ID]   = port->hsize;
  m_hwrite_next[master_ID]  = port->hwrite;
  m_haddr_next[master_ID]   = port->haddr;
  m_hwdata_next[master_ID]  = port->hwdata;
  m_hbusreq_next[master_ID] = port->hbusreq;
  m_hlock_next[master_ID]   = port->hlock;
}

///////////////////////////////////////////////////////////////////////////////
// operate - Moves forward the IC state. This FSM is composed of two states,
//           which must be executed during every clock cycle in succession.
//           ic_sampling: executed on the rising clock edge. Samples the value
//                        of internal signals and queries them to the AHB
//                        slaves. (Slave querying might be instantaneous or
//                        take some delta cycles, depending on slave behaviour).
//                        Additionally, this state performs rearbitration.
//           ic_updating: executed one delta cycle later. Updates the value of
//                        the internal signals and performs decoding.
void amba_ahb_interconnect::operate()
{
  AHB_slave_port slaveport;
  unsigned char i, j, m;
  
  current_ic_state = next_ic_state;
  
  switch (current_ic_state)
  {
    case ic_sampling : // Let's sample the outputs of the previous cycle
                       for (i = 0; i < m_nslaves; i ++)
                       {
                         slaveport.hsel      = hsel[i];
                         slaveport.haddr     = haddr;
                         slaveport.hwrite    = hwrite;
                         slaveport.htrans    = htrans;
                         slaveport.hsize     = hsize;
                         slaveport.hburst    = hburst;
                         slaveport.hwdata    = hwdata;
                         slaveport.hmaster   = hmaster;
                         slaveport.hready    = hready;
                         slaveport.hmastlock = hmastlock;
                         slave_port[i]->ahb_slave_query(ID, &slaveport);
                         s_hresp[i]          = slaveport.hresp;
                         s_hready[i]         = slaveport.hreadyout;
                         s_hrdata[i]         = slaveport.hrdata;
                         for (m = AHB_BOOT_MASTER; m <= last_master_id; m ++)
                           // Skip the default master
                           if (master_present[m])
                             hsplit[i][m]      = slaveport.hsplit[m];
                       }
                       
                       for (i = 0; i < m_nslaves; i ++)
                         for (m = AHB_BOOT_MASTER; m <= last_master_id; m ++)
                           // Skip the default master
                           if (master_present[m] && hsplit[i][m])
                             arb_master_split[m] = false;
                       
                       if (hready)
                       {
                         delayed_hmaster = hmaster;
                         for (j = 0; j < AHB_MAX_SLAVES; j ++)
                           delayed_hsel[j] = hsel[j];
                       }

                       grant();

                       next_ic_state = ic_updating;
                       next_trigger(SC_ZERO_TIME);
                       
                       break;
                       
    case ic_updating : // Let's update values for the next cycle
                       for (i = AHB_BOOT_MASTER; i <= last_master_id; i++)
                       {
                         m_hburst[i]  = m_hburst_next[i];
                         m_htrans[i]  = m_htrans_next[i];
                         m_hsize[i]   = m_hsize_next[i];
                         m_hwrite[i]  = m_hwrite_next[i];
                         m_haddr[i]   = m_haddr_next[i];
                         m_hwdata[i]  = m_hwdata_next[i];
                         m_hbusreq[i] = m_hbusreq_next[i];
                         m_hgrant[i]  = m_hgrant_next[i];
                         m_hlock[i]   = m_hlock_next[i];
                       }
    
                       if (POWERSTATS)
                       {
                         if (hmaster != hmaster_next)
                           monitor_activity.hmaster = true;
                       }

                       hmaster = hmaster_next;

                       // This is the output of asynchronous muxes
                       if (hmaster == AHB_DEFAULT_MASTER)
                       {
                         if (POWERSTATS)
                         {
                           // The bus is inactive this cycle
                           if (htrans == AHB_NONSEQ || htrans == AHB_SEQ)
                            monitor_activity.htrans = true;
                         }
                         
                         hburst    = AHB_SINGLE;
                         htrans    = AHB_IDLE;
                         hsize     = AHB_WORD;
                         hwrite    = AHB_READ;
                         haddr     = certainly_valid_address;
                         hmastlock = false;
                       }
                       else
                       {
                         if (POWERSTATS)
                         {
                           // The bus is active this cycle, or it was in the previous
                           if (
                               (m_htrans[hmaster] == AHB_NONSEQ || m_htrans[hmaster] == AHB_SEQ) 
                               ||
                               (m_htrans[hmaster] != AHB_NONSEQ && m_htrans[hmaster] != AHB_SEQ && (htrans == AHB_NONSEQ || htrans == AHB_SEQ))      
                              )
                             monitor_activity.htrans = true;
                         }
                         
                         hburst = m_hburst[hmaster];
                         htrans = m_htrans[hmaster];
                         hsize  = m_hsize[hmaster];
                         hwrite = m_hwrite[hmaster];
                         haddr  = m_haddr[hmaster];
                         
                         if (m_hlock[hmaster])
                           hmastlock = true;
                         else
                         {
                           arb_bus_locked[hmaster] = false; 
                           hmastlock = false;
                         }
                       }
    
                       decode();
    
                       hwdata = m_hwdata[delayed_hmaster];
                       
                       for (j = 0; j < AHB_MAX_SLAVES; j ++)
                         if (delayed_hsel[j])
                         {
                           if (POWERSTATS)
                             if (hready != s_hready[j])
                               monitor_activity.hready = true;
                           hready = s_hready[j];
                           hresp  = s_hresp[j];
                           hrdata = s_hrdata[j];
                         }
                         
                       // By now, all of the power modeling factors are known
                       if (POWERSTATS)
                       {
                         statobject->inspectAMBAAHBAccess(ID, monitor_activity);
                         // Reset the flags for the next cycle
                         monitor_activity.htrans  = false;
                         monitor_activity.hmaster = false;
                         monitor_activity.hready  = false;
                       }
                       
                       // Split response: mask master hbusreq
                       if (hresp == AHB_SPLIT)
                         arb_master_split[(int)delayed_hmaster] = true;

                       // Split or error response: force rearbitration
                       // (unless already done, e.g. hburst == AHB_SINGLE)
                       if ( (hresp == AHB_SPLIT || hresp == AHB_ERROR) 
                            && m_hgrant_next[(int)delayed_hmaster]
                            && hburst != AHB_SINGLE
                          )
                         arb_rearbitrate = true;

                       // Okay response: don't force rearbitration
                       if (hresp == AHB_OKAY)
                         arb_rearbitrate = false;
                       
                       next_ic_state = ic_sampling;
                       next_trigger();
                       
                       break;
  }
}

///////////////////////////////////////////////////////////////////////////////
// decode - Provides AMBA AHB decoding. Decoding is based upon static maps
//          built during the end_of_elaboration() stage. Any unknown address
//          should be mapped onto the default slave, which should provide
//          AHB_OKAY (on AHB_IDLE, AHB_BUSY transfers) or AHB_ERROR (on
//          AHB_NONSEQ, AHB_SEQ transfers) responses. As of now, any such
//          transfer stops the simulation with an error.
void amba_ahb_interconnect::decode()
{
  short int slave = AHB_MAX_SLAVES - 1;
  unsigned char i;

  for (i = 0; i < range_count; i ++)
    if (haddr >= starting_addresses[i] && haddr <= ending_addresses[i])
    {
      slave = mapping_target[i];
      break;
    }

  if (i == range_count && (htrans == AHB_NONSEQ || htrans == AHB_SEQ))
  {
    //FIXME This should actually be a default slave access with OKAY (htrans = IDLE, BUSY)
    //or ERROR (htrans = NONSEQ, SEQ) response.
    printf("Fatal error: in %s, decoding memory address 0x%08x to any slave is impossible!\n",
      name(), (unsigned int)haddr);
    exit(1);
  }
    
  for (i = 0; i < m_nslaves; i ++)
    if (i == slave)
      hsel[i] = true;
    else
      hsel[i] = false;
        
  hsel[AHB_MAX_SLAVES - 1] = false;

#ifdef PRINTDEBUG
  printf("Decoding finished, address 0x%08x mapped to slave %d\n", (uint32_t)haddr, slave);
#endif
}

///////////////////////////////////////////////////////////////////////////////
// arbitrate - Defines the arbitration policy (round-robin). Returns the
//             ID of the highest-priority master request, or -1 if no
//             requests were asserted. In the latter case, the default
//             master should usually be granted bus ownership.
int amba_ahb_interconnect::arbitrate()
{
  int i;

  for (i = arb_previous + 1; i <= last_master_id; i ++)
    if (m_hbusreq[i] == true && arb_master_split[i] == false)
    {
      if (m_hlock[i])
        arb_bus_locked[i] = true;
      
      arb_previous = i;
      if (arb_previous == last_master_id)
        arb_previous = AHB_BOOT_MASTER - 1;
      return (i);
    }
    
  for (i = AHB_BOOT_MASTER; i <= arb_previous; i ++)
    if (m_hbusreq[i] == true && arb_master_split[i] == false)
    {
      if (m_hlock[i])
        arb_bus_locked[i] = true;
      
      arb_previous = i;
      if (arb_previous == last_master_id)
        arb_previous = AHB_BOOT_MASTER - 1;
      return (i);
    }

  return (-1);
}

void amba_ahb_interconnect::grant()
{
  if (m_nmasters == 1)
  {
    hmaster_next = AHB_BOOT_MASTER;
    m_hgrant_next[AHB_BOOT_MASTER] = true;
    //FIXME Does not work with one master and split responses!!
  }
  else
  {
    // Only active when HREADY is high
    if (hready)
    {
      // Time to sample HBURST to see if we're going to do
      // a single or burst transfer
      if (arb_sample_burst && (htrans == AHB_NONSEQ))
      {
        switch (hburst)
        {
          case AHB_SINGLE:
            arb_burst = 1;
            break;
          case AHB_INCR4:
          case AHB_WRAP4:
            arb_burst = 4;
            break;
          case AHB_INCR8:
          case AHB_WRAP8:
            arb_burst = 8;
            break;
          case AHB_INCR16:
          case AHB_WRAP16:
            arb_burst = 16;
            break;
          case AHB_INCR:
            arb_burst = 0;
            break;
          default:
            printf("Fatal error: Unknown HBURST value %u in %s!\n", (unsigned int)hburst, name());
            exit(1);
        }
        
        arb_counter = 0;
        
        arb_sampled_first = true;
        arb_sample_burst = false;
      }
  
      // Push out the new HMASTER, based upon the HGRANT status
      // (But let's remember that if the previous transaction had HTRANS == BUSY,
      // assertion of HREADY is not really showing any progress in the burst)
      if (!arb_busy_transaction)
        hmaster_next = (sc_uint<AHB_HMASTER_BITS>)arb_prev;
    
      // We're one HREADY after rearbitration, so let's remember
      // that after the next one, HBURST will need to be sampled
      if (arb_just_rearbitrated)
      {
        arb_sample_burst = true;
        arb_just_rearbitrated = false;
      }
    
      // One more HREADY has passed, let's update the count (but avoiding overflow!)
      // (But let's remember that if the previous transaction had HTRANS == BUSY,
      // assertion of HREADY is not really showing any progress in the burst)
      if (!arb_busy_transaction && arb_counter < 30)
        arb_counter ++;
        
      if (htrans == AHB_BUSY)
        arb_busy_transaction = true;
      else
        arb_busy_transaction = false;
  
      // If we have a valid arb_burst count (sampled_first == true)
      // (this takes into account single-beat transfers),
      // and we've finished with the penultimate address,
      // it's time to try and rearbitrate. Special warnings:
      // if the transaction was a burst of unspecified length
      // (arb_burst == 0), rearbitration occurs only when the
      // hbusreq wire of the master goes low; an ERROR or SPLIT
      // response can force a rearbitration; if the default master is
      // active, we can rearbitrate
      if (
           (
             (
               arb_sampled_first &&
               arb_counter >= arb_burst - 1 &&
               (
                 arb_burst != 0 ||
                 (
                   arb_burst == 0 &&
                   m_hbusreq[arb_prev] == false &&
                   htrans != AHB_BUSY
                 )
               )
             ) ||
             arb_rearbitrate ||
             arb_prev == AHB_DEFAULT_MASTER
           ) && !arb_bus_locked[hmaster]
         )
      {
        // Take the previous grant away
        m_hgrant_next[arb_prev] = false;

        // Any valid pending requests?
        if ((arb_next = arbitrate()) >= 0)
        {                                        // Yes, give this grant
          m_hgrant_next[arb_next] = true;
          arb_prev = arb_next;
          arb_just_rearbitrated = true;
          arb_sampled_first = false;
        }
        else
        {                                        // No, default master
          m_hgrant_next[AHB_DEFAULT_MASTER] = true;
          arb_prev = AHB_DEFAULT_MASTER;
        }
      }
    }
  }
}
