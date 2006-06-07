///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_ahb_ocp_master.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         AMBA AHB OCP bus master
//
///////////////////////////////////////////////////////////////////////////////

#include "amba_ahb_ocp_master.h"
#include "stats.h"
#include "config.h"
#include "globals.h"
#include "address.h"

///////////////////////////////////////////////////////////////////////////////
// working - Implements the functionality of an AMBA AHB bus master. First, a
//           CPU request is processed; subsequently, either a read or write
//           code path is taken. Bursts (1, 4, 8 or 16-beat incrementing) are
//           supported.
//           The IC interface is called to query IC status and to request new
//           transactions. Multiple interface calls can be made in the same
//           cycle, as long as they all happen during the rising clock edge
//           (i.e. not even a delta cycle later).
void amba_ahb_ocp_master::working()
{
  AHB_master_port my_port;

  uint32_t din, dout, addr, prev_addr, size, next_dout;
  int burst = 0, i;
  bool is_last;
  
  local_ID = master_port->get_master_ID();
  
  printf("AMBA OCP master %s: global ID %hu, local ID %hu\n", name(), global_ID, local_ID);

  while (true)
  {
    SData.write(0xdeadbeef);
    SCmdAccept.write(false);
    SResp.write(OCPSRESNULL);
    SRespLast.write(false);
    
    // Wait until someone requests something
    while (MCmd.read() == OCPCMDIDLE)
      wait();
    
    // Word, half word or byte?
    // FIXME assumes a fixed set
    switch (MByteEn.read())
    {
      case OCPMBYEWORD :  my_port.hsize = AHB_WORD;
                          size = 0x4;
                          break;
      case OCPMBYEBYTE :  my_port.hsize = AHB_BYTE;
                          size = 0x1;
                          break;
      case OCPMBYEHWRD :  my_port.hsize = AHB_HALFWORD;
                          size = 0x2;
                          break;
      default :           printf("Fatal error: Master %hu detected a malformed data size at %10.1f ns\n",
                            global_ID, sc_simulation_time());
                          exit(1);
    }

    addr = addresser->Logical2Physical((uint32_t)MAddr.read(), global_ID);

    is_last = MReqLast.read();
    
    switch (MCmd.read()) //TODO OCPCMDWRNP
    {
      // ----
      // Read
      // ----
      case OCPCMDREAD:
      
        SCmdAccept.write(true);
        wait();
        SCmdAccept.write(false);
        
        // Is this a Single Request Burst Read, or just a read?
        if (MBurstSingleReq.read() && MBurstPrecise.read())
          burst = (uint16_t)MBurstLength.read();
        else
          burst = 1;
     
        if (STATS)
          statobject->requestsAccess(global_ID);

        my_port.htrans = AHB_NONSEQ;
        my_port.hbusreq = true;
        my_port.haddr = addr;
        my_port.hburst = AHB_INCR;
        my_port.hwrite = AHB_READ;
        
        // Let's request the bus
        master_port->ahb_interconnect_write(local_ID, &my_port);

        // Wait for the grant. Actually, we might get a zero-cycle
        // grant (e.g. if we're the only master on the bus).
        master_port->ahb_interconnect_read(local_ID, &my_port);
        while (!(my_port.hgrant && my_port.hready))
        {
          wait();
          master_port->ahb_interconnect_read(local_ID, &my_port);
        }
        
        // If in a single transfer, it's already time to lower
        // HBUSREQ
        if (burst == 1)
          my_port.hbusreq = false;
        master_port->ahb_interconnect_write(local_ID, &my_port);
      
        do
        {
          wait();
          master_port->ahb_interconnect_read(local_ID, &my_port);
        } while (!(my_port.hready));
      
        if (STATS)
          statobject->beginsAccess(2, true, burst, global_ID);
        
        // Only write this information down once we got bus arbitration. In this way,
        // traces contain consistent information wrt the statistics file.
        if (TG_TRACE_COLLECTION)
        {
          fprintf(ftrace, "%s", buffer_string);
          buffer_string[0] = '\0';
        }
          
        // Increment the address for the next beat
        // (if in a single transaction, the address bus isn't ours any more by now)
        prev_addr = addr;
        addr += size;
        my_port.haddr = addr;
        
        // If in a two-beat transfer, it's already time to lower
        // HBUSREQ
        if (burst == 2)
          my_port.hbusreq = false;
        master_port->ahb_interconnect_write(local_ID, &my_port);
              
        if (burst == 1)
          my_port.htrans = AHB_IDLE;
        else
          my_port.htrans = AHB_BUSY;

        master_port->ahb_interconnect_write(local_ID, &my_port);
        
        for (i = 0; i < burst; i ++)
        {
          // Wait for read response, while the following
          // address (if not single transaction or last beat)
          // is out but with HTRANS BUSY
          do
          {
            wait();
            SResp.write(OCPSRESNULL);
            master_port->ahb_interconnect_read(local_ID, &my_port);
          } while (my_port.hready != true);

          // Data is available, while the next address is
          // still paired with HTRANS BUSY
          din = my_port.hrdata;

          TRACEX(MASTER_TRACEX, 8, "%s: %s read  0x%08x at address 0x%08x at %10.1f ns\n%s",
            name(), burst == 1 ? "single" : "burst ", uint(din),
            (uint)prev_addr, sc_simulation_time(), (i == burst - 1) ? "\n" : "");
            
          // Push data on OCP and wait for MRespAccept
          SData.write(din);
          SResp.write(OCPSRESDVA);
          if ((burst > 1 && i == burst - 1) || (burst == 1 && is_last))
            SRespLast.write(true);
            
          do
            wait();
          while (MRespAccept.read() != true);
             
          SResp.write(OCPSRESNULL);
          
          // Allow processing of the following address (if not
          // single transaction or last beat)
          if (burst > 1 && i < burst - 1)
          {
            my_port.htrans = AHB_SEQ;
            master_port->ahb_interconnect_write(local_ID, &my_port);
            
            // Let the response to the last BUSY transfer go by
            wait();
          }
          
          // If needed, push out the address for the new transaction,
          // otherwise go IDLE. New addresses must be marked with
          // HTRANS BUSY
          prev_addr = addr;
          if (i >= burst - 2)
            my_port.htrans = AHB_IDLE;
          else
          {
            // Increment the address for the next beat
            addr += size;
            my_port.haddr = addr;
            my_port.htrans = AHB_BUSY;
          }
          
          if (i == burst - 3)
            my_port.hbusreq = false;
          
          master_port->ahb_interconnect_write(local_ID, &my_port);
        } // end for
          
        if (STATS)
          statobject->endsAccess(true, burst, global_ID);
        
        break;

      // ------------
      // Posted write
      // ------------
      case OCPCMDWRITE:
      
        SCmdAccept.write(true);
        wait();
        SCmdAccept.write(false);
        
        // Is this a Single Request Burst Read, or just a read?
        if (MBurstSingleReq.read() && MBurstPrecise.read())
          burst = (uint16_t)MBurstLength.read();
        else
          burst = 1;
          
        if (burst == 1)
          my_port.hburst = AHB_SINGLE;
        if (burst == 4)
          my_port.hburst = AHB_INCR4;
        if (burst == 8)
          my_port.hburst = AHB_INCR8;
        if (burst == 16)
          my_port.hburst = AHB_INCR16;
        //TODO other burst lengths
     
        if (STATS)
          statobject->requestsAccess(global_ID);
          
        next_dout = (uint32_t)MData.read();
          
        my_port.htrans = AHB_NONSEQ;
        my_port.hbusreq = true;
        my_port.haddr = addr;
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
          statobject->beginsAccess(2, false, burst, global_ID);
        
        // Only write this information down once we got bus arbitration. In this way,
        // traces contain consistent information wrt the statistics file.
        // TODO traces of burst writes
        if (TG_TRACE_COLLECTION)
        {
          fprintf(ftrace, "%s", buffer_string);
          buffer_string[0] = '\0';
        }

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
            next_dout = MData.read();
            my_port.haddr = addr;
          }

          // Either ending the burst, or going on
          if (i == burst - 1)
            my_port.htrans = AHB_IDLE;
          else
            my_port.htrans = AHB_SEQ;
          master_port->ahb_interconnect_write(local_ID, &my_port);
          
          // Let's wait for the slave to finish writing
          do
          {
            wait();
            master_port->ahb_interconnect_read(local_ID, &my_port);
          } while (my_port.hready != true);
          
          if (STATS && (i == burst - 1))
            statobject->endsAccess(false, burst, global_ID);

          TRACEX(MASTER_TRACEX, 8, "%s: %s write 0x%08x at address 0x%08x at %10.1f ns\n%s",
            name(), burst == 1 ? "single" : "burst ", uint(dout),
            (uint)prev_addr, sc_simulation_time(), (i == burst - 1) ? "\n" : "");
        } // end for

        break;
      
      // -----------
      // Unsupported
      // -----------
      default:
        printf("Fatal error: %s: Trace requested for an invalid transaction type\n",
          name());
        exit(1);
        
        break;
    }
      
    if (STATS) //FIXME should be one cycle later for writes
      statobject->busFreed(global_ID);
  }   
}


///////////////////////////////////////////////////////////////////////////////
// tg_trace_collection - Collects TG traces during execution, if needed. Used
//                       to sample MCmd continuously without constraints due to
//                       waiting loops
void amba_ahb_ocp_master::tg_trace_collection()
{
  bool int_sent = false, int_acked = false;
  bool was_read = true;
  char thistraceline[200];
  already_written_cmd.write(false);
  already_written_resp.write(false);
  
  //TODO see list in xpipes module
  
  while (true)
  {
    // We must produce TG execution traces if:
    // - TG_TRACE_COLLECTION is enabled (guaranteed by the fact that this process is running), AND
    // - we have a valid MCmd, AND
    // - we haven't yet written this specific line down
    if (MCmd.read() != OCPCMDIDLE && !already_written_cmd.read())
    {
      if (MCmd.read() == OCPCMDREAD)
      {
        sprintf(thistraceline, "MCmd RD MAddr 0x%08x MBurstSingleReq %d MBurstSeq INCR 0x4 MBurstLength %u Time %.0f\n",
          (uint)MAddr.read(),
          MBurstSingleReq.read(),
          (uint)MBurstLength.read(),
          sc_simulation_time() - CLOCKPERIOD);
        was_read = true;
      }
      else if (MCmd.read() == OCPCMDWRITE || MCmd.read() == OCPCMDWRNP)
      {
        sprintf(thistraceline, "MCmd %s MAddr 0x%08x MData 0x%08x MBurstSingleReq %d MBurstSeq INCR 0x4 MBurstLength %u Time %.0f\n",
          MCmd.read() == OCPCMDWRITE ? "WR" : "WRNP",
          (uint)MAddr.read(),
          (uint)MData.read(),
          MBurstSingleReq.read(),
          (uint)MBurstLength.read(),
          sc_simulation_time() - CLOCKPERIOD);
        was_read = false;
      }
      else
      {
        printf("Fatal error: %s: Trace requested for an invalid transaction type\n", name());
        exit(1);
      }
      strcat(buffer_string, thistraceline);
      already_written_cmd.write(true);
    }
    
    if (SResp.read() != OCPSRESNULL && !already_written_resp.read())
    {
      if (was_read)
        fprintf(ftrace, "Resp Data 0x%08x Time %.0f\n", (uint)SData.read(), sc_simulation_time());
      else
        fprintf(ftrace, "Resp Time %.0f\n", sc_simulation_time());
      already_written_resp.write(true);
    }
    
    if (SCmdAccept.read() && MCmd.read() != OCPCMDIDLE)
    {
      sprintf(thistraceline, "SCmdAccept Time %.0f\n", sc_simulation_time());
      strcat(buffer_string, thistraceline);
      already_written_cmd.write(false);
    }
    
    if (MRespAccept.read())
      already_written_resp.write(false);

    if (SInterrupt.read() && !int_sent)
    {
      sprintf(thistraceline, "SInterrupt SFlag 0x%08x Time %.0f\n", (uint32_t)SFlag.read(), sc_simulation_time());
      strcat(buffer_string, thistraceline);
      int_acked = false;
      int_sent = true;
    }
    else if (MFlag.read() && !int_acked)
    {
      sprintf(thistraceline, "MFlag Time %.0f\n", sc_simulation_time() - CLOCKPERIOD);
      strcat(buffer_string, thistraceline);
      int_sent = false;
      int_acked = true;
    }

    wait();
  }
}


///////////////////////////////////////////////////////////////////////////////
// int_forwarding - Forwards interrupts through the OCP interface
void amba_ahb_ocp_master::int_forwarding()
{
  SInterrupt.write(false);
  SFlag.write(0x0);

  while (true)
  {
    int_mask = 0;
    
    for (int i = 0; i < NUMBER_OF_EXT; i ++)
    {
      if (extint[i].read()) 
      {
        //printf("Master %s detected an int on wire %d @ %7.1f\n", name(), i, sc_simulation_time());
        if (SInterrupt.read() || MFlag.read())
          printf("Warning: %s: Incoming interrupt on wire %d while still handshaking previous one!\n", name(), i);
        
        //FIXME does not handle the case when a 2nd interrupt arrives at the end of the handshake of
        // a previous int (with SInterrupt already sampled)
        SInterrupt.write(true);
        int_mask |= (1 << i);
        SFlag.write(SFlag.read() || int_mask);
        extint[i].write(false);
      }
    }
    
    if (MFlag.read())
    {
      SInterrupt.write(false);
      SFlag.write(0x0);
      //printf("Master %s detected an int ack @ %7.1f\n", name(), sc_simulation_time());
    }

    wait();
  }
}
