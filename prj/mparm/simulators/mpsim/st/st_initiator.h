///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         st_initiator.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         STBus initiator interface
//
///////////////////////////////////////////////////////////////////////////////


#ifndef __ST_INITIATOR_H__
#define __ST_INITIATOR_H__

#include <armproc.h>
#include "address.h"
#include "systemc.h"
#include "config.h"
#include "debug.h"
#include "SC_INITIATOR_T2.h"
#include "SC_INITIATOR_T3.h"




template <class TDATA, class TADDRESS, class TBE, uint STBUS_SIZE, uint TRANS_TABLE_SIZE, uchar SOURCE, bool TRAFFIC_GEN, bool INI_DEBUG>
class STBus_initiator : public INITIATOR_T3<TDATA, TADDRESS, TBE, STBUS_SIZE, TRANS_TABLE_SIZE, SOURCE, TRAFFIC_GEN, INI_DEBUG>
{

  public:

    sc_in<bool> request_from_wrapper;
    sc_out<bool> ready_to_wrapper;
    sc_inout<PINOUT> pinout;


    STBus_initiator(sc_module_name nm) :
      INITIATOR_T3<TDATA, TADDRESS, TBE, STBUS_SIZE, TRANS_TABLE_SIZE, SOURCE, TRAFFIC_GEN, INI_DEBUG>(nm)
    {
      TRACEX(STBUS_OP_TRACEX, 10, 
             "SOURCE: %d - ARM_STbus_interface::ARM_STbus_interface(%s)\n", 
             (int)SOURCE, (const char*)nm);
    }



  uchar PREPARE_OP(char op, TADDRESS address, int size, int total_bursts, uchar *buffer)
  {
    uchar vtid;
    BETYPE vbe;      /*  byte enable  */

    ASSERT( (op==0) || (op==1) );   /*  0 : read  -  1: write  */

    ASSERT(size<=4);  /* bus a 32 bit */
    ASSERT( (size==4) || (size==2) || (size==1) );
    ASSERT(bus_width==4); /* 4 bytes */

    TRACEX(STBUS_OP_TRACEX, 10, 
          "SOURCE: %d - "
          "ARM_STbus_interface::PREPARE_OP(%d, 0x%x, %d, %d, 0x%x)\n",
          (int)SOURCE, (int)op, (int)address, size, total_bursts, (int)buffer);

    // Check alignment
    ASSERT( (size != 4) || (address%4 == 0) );
    ASSERT( (size != 2) || (address%2 == 0) );

    if(trtable.full()) {
      cout << name() << " Warning: The request Table is full !" << endl;
      return (TOO_MANY_REQUESTS);
    }

    if (size==4) vbe=0xF;
    else if (size==2) vbe=0x3;
    else if (size==1) vbe=0x1;

    // Data realignment (if needed)
    TDATA dummy=0;
    realign(size, address, vbe, dummy);

    /*if (total_bursts==1) vbe *= 0x1;
    else*/

    if (total_bursts==4) vbe *= 0x1111;
    else if (total_bursts==8) vbe *= 0x11111111;
    
    /*** set request ***/
    vtid = set_req( (uchar)(op==0?LOAD_M:STORE_M), address, vbe, buffer, size*total_bursts, total_bursts, true,
                    false, false); // last parameters: blocking, postable. ARM blocks on bus access
    return vtid;
  }

void send_cell_hacked(uchar vtid,           // transaction identifier
		       TADDRESS start_addr,  // address cell
		       const TDATA& tcell,  // current data cell
		       TBE tbe,             // byte enable
		       unsigned char topc,  // temp opcode
		       bool posted,	    // post flag
		       bool last_pack)	    // end of packet
	{ 
	    bool firstwait;
	    Out.req.write(true);
	    Out.eop.write(last_pack);
	    Out.add.write(start_addr & add_mask);
	    Out.be.write(tbe);
	    Out.data.write(tcell);
	    Out.opc.write(topc);
	    Out.src.write(m_src);
	    
	    /*** set tid value ***/
	    vtid &= 0x0f;
	    if(m_message_count)
		vtid |= 0x10;
	    if(posted)
		vtid |= 0x20;
	    if(m_tid6)
		vtid |= 0x40;
	    Out.tid.write(vtid);
	    Out.pri.write(m_pri & 0x0f);
	    
	    /*** set lock ***/
	    if(m_lock_count)
		Out.lck.write(true);
	    
	    firstwait=true;
	    
	    do
	    {	
	     wait();
	       if(firstwait) 
	       {ready_to_wrapper.write(false);
	       } 
	     //printf("aspetto gnt\n");
	    }
	    while(!(Out.gnt));
	    
	    //printf("ricevuto gnt\n");
	    
	    if(last_pack)
	    {	
	      if( (m_message_count) && (m_lock_count == 0) )
		    m_message_count--;
		if(m_lock_count)
		    m_lock_count--;
		Out.req.write(false);
		Out.eop.write(false);
		Out.lck.write(false);
		Out.tid.write(vtid);
	    }
	}

#define READ(address, size, total_bursts, vtid)                 OPERATION((char)0, address, size, 0, 0,  total_bursts, vtid)
#define WRITE(address, size, data, nburst, total_bursts, vtid)  OPERATION((char)1, address, size, data, nburst, total_bursts, vtid)


  // Returns the number of clock cycles spent waiting (ssend_cell may have to wait)
  uint64_t OPERATION(char op, TADDRESS address, int size, TDATA data, int nburst, int total_bursts, uchar& vtid)
  {
    TBE tbe;         /*  temporary byte enable  */
    BETYPE vbe;      /*  byte enable  */
    uchar tmp_opc;   /*  temporary opcode  */
    uchar  nb = 0;   /*  num bytes = 2^n  */
    double t;

    ASSERT( (op == 0) || (op == 1) );   //  0 : read  -  1: write

    ASSERT( size <= 4 );                // 32-bit bus (4 bytes)
    ASSERT( (size == 4) || (size == 2) || (size == 1) );
    ASSERT( bus_width == 4 );           // 32-bit bus (4 bytes)

    TRACEX(STBUS_OP_TRACEX, 10, "SOURCE:%d - ", (int)SOURCE);
    TRACEX(STBUS_OP_TRACEX, 10, 
           "ARM_STbus_interface::OPERATION(%d, 0x%x, %d, ..., %d, "
           "%d, 0x%x, ...)\n",
          (int)op, (int)address, size, nburst, total_bursts);

    if(trtable.full()) {
      cout << name() << " Warning: The request Table is full !" << endl;
      return (TOO_MANY_REQUESTS);
    }

    if (size==4) {
      vbe=0xF;
      nb = 2;
    } else if (size==2) {
      vbe=0x3;
      nb = 1;
    } else if (size==1) {
      vbe=0x1;
      nb = 0;
    }

    // nb = ln2(bytes to transfer)  ==>> nb += ln2(totalbursts)

    /*if (total_bursts==1) {
      nb+0;
      vbe *= 0x1;
    } else*/
    if (total_bursts==4) {
      nb+=2;
      vbe *= 0x1111;
    }
    else if (total_bursts==8) {
      nb+=3;
      vbe *= 0x11111111;
    }

    // Data realignment (if needed)
    realign(size, address, vbe, data);

    tmp_opc = (nb << 4) | (op==0?LOAD_M:STORE_M);

    //ASSERT(sizeof(data)==sizeof(TDATA))
    tbe = vbe & be_mask;

    t = sc_simulation_time();

    if(op==1 && nburst<total_bursts-1 && total_bursts>=4)
     {
      //if(SOURCE==1) printf("Send cell hacked\n");
      send_cell_hacked(vtid, address, data, tbe, tmp_opc, false, (op==0?true:nburst==total_bursts-1) );
     }
    else 
     {
      //if(SOURCE==1) printf("Send cell\n");
      send_cell(vtid, address, data, tbe, tmp_opc, false, (op==0?true:nburst==total_bursts-1) );
     }                                         /* postable */

    return (uint64_t)(sc_simulation_time() - t);
  }








  void realign(int size, TADDRESS& address, BETYPE& vbe, TDATA& data)
  {
    // Data realignment (if needed)
    // 32-bit interconnect ==> align addresses to 32-bit boundaries
    if (size==2) {
      if (address%4 != 0) {
        address&=~3;
        data <<= 16;
        vbe <<= 2;
      }
    } else if (size==1) {
      switch (address%4) {
        case 1:
          data <<= 8;
          vbe <<= 1;
          break;
        case 2:
          data <<= 16;
          vbe <<= 2;
          break;
        case 3:
          data <<= 24;
          vbe <<= 3;
          break;
        default:
          break;
      }
      address &= ~3;
    }
  }






  void initiator_behav()
  {
    TDATA dout;
    TADDRESS address;

    int size = 0;
    int bursts = 0;
    bool rw;
    uint64_t t;

    PINOUT mast_pinout;

    TRACEX(STBUS_OP_TRACEX, 10, 
           "SOURCE:%d - ARM_STbus_interface::initiator_behav()\n", 
           (int)SOURCE);

    ready_to_wrapper.write(false);

    while(1)
    {
      while (request_from_wrapper.delayed() != true)
        wait();

      if (STATS)
        statobject->requestsAccess(SOURCE);

      mast_pinout = pinout.read();
      //ready_to_wrapper.write(false);
      address = mast_pinout.address;

      TRACE_CLK_CNT(8);
      TRACEX(STBUS_OP_TRACEX, 8, "SOURCE:%d - ", (int)SOURCE);
      TRACEX(STBUS_OP_TRACEX, 8, 
             "ARM_STbus_interface::initiator_behav() - address: 0x%x\n", 
             (int)address);
      
      address = addresser->Logical2Physical(address, SOURCE);

      dout = mast_pinout.data;

      switch (mast_pinout.bw)
      {
        case 0 :
          size = 0x4;  /* word transfer */
          break;
        case 1 :
          size = 0x1;  /* byte transfer */
          break;
        case 2 :
          size = 0x2;  /* halfword transfer */
          break;
      }

      // Maybe other values should be valid too?
      ASSERT( (mast_pinout.burst == 1) || (mast_pinout.burst == 4) || (mast_pinout.burst == 8) || (mast_pinout.burst == 16) );
      bursts = mast_pinout.burst;

      TRACE_CLK_CNT(8);
      TRACEX(STBUS_OP_TRACEX, 8, "SOURCE:%d - ", (int)SOURCE);
      TRACEX(STBUS_OP_TRACEX, 8, 
             "ARM_STbus_interface::initiator_behav() - "
             "size: 0x%x - bursts: 0x%x\n", 
             size, bursts);

      rw = mast_pinout.rw;
      
      // WRITING
      
      if (rw == 1)
      {
        uchar vtid;

        TRACE_CLK_CNT(8);
        TRACEX(STBUS_OP_TRACEX, 8, 
               "SOURCE:%d - ARM_STbus_interface::initiator_behav() - writing\n",
               (int)SOURCE);

        vtid = PREPARE_OP((char)1, address, size, bursts, (uchar*)&dout);
        
        if (SOURCE==1)
          TRACEX(STBUS_OP_TRACEX, 6, 
                 "INITIATOR:%d start_write "
                 "addr:%x,size:%d,tot_burst:%d,vtid:%d\n",
	         SOURCE, address,size,bursts,vtid);	
	
        if(bursts==1)
	{ 
	 t = WRITE(address, size, (TDATA)dout, 0, bursts, vtid);
	 
	 if (STATS)
            statobject->beginsAccess(1, !rw, bursts, SOURCE);          
	    
	 while ( !Out.r_req || ((Out.r_tid)&0x0f)!=vtid  )
            wait();
	}
        else
	{
         for (int i = 0; i < bursts; i++)
         {
          if(i<bursts-1 && i!=0) 
	   {ready_to_wrapper.write(true);
	    //if (SOURCE==1) printf("Setto ready\n");
	   }
	  t = WRITE(address, size, (TDATA)dout, i, bursts, vtid);

          if (STATS && (i == 0))
            statobject->beginsAccess(1, !rw, bursts, SOURCE);

          // cell sent
          if (i < bursts - 1)
          {
           mast_pinout = pinout.read();
           dout = mast_pinout.data;
           //if (SOURCE==1) printf("Letto %x dal dma\n",dout);
	  }
         }
	 //if (SOURCE==1) printf("Aspetto il req\n");
	 while ( !Out.r_req || ((Out.r_tid)&0x0f)!=vtid  )
          wait();
        }
      }
      
      // READING
      
      else
      {
        uchar vtid;
        uchar buf[4*16];   /* max 16 bursts, 4 bytes each */

        TRACE_CLK_CNT(8);
        TRACEX(STBUS_OP_TRACEX, 8, 
               "SOURCE:%d - ARM_STbus_interface::initiator_behav() - reading\n",
               (int)SOURCE);

        vtid = PREPARE_OP((char)0, address, size, bursts, buf);
        if(SOURCE==1)
          TRACEX(STBUS_OP_TRACEX, 6,
                 "INITIATOR:%d start_read "
                 "addr:%x,size:%d,burst:%d,vtid:%d\n",
                 SOURCE, address,size,bursts,vtid);
	t = READ(address, size, bursts, vtid);

        for (int i = 0; i < bursts; i++)
        {
          TRACE_CLK_CNT(8);
          TRACEX(STBUS_OP_TRACEX, 8, "SOURCE:%d - ", (int)SOURCE);
          TRACEX(STBUS_OP_TRACEX, 8, 
                 "ARM_STbus_interface::initiator_behav() - burst read #%d\n", 
                 i);

          if (STATS && (i == 0))
            statobject->beginsAccess(1, !rw, bursts, SOURCE);
          
	  //if(SOURCE==1) printf("Initiator read:%d\n",mast_pinout.data);
	  
          while ( !(Out.r_req) || ((Out.r_tid)&0x0f)!=vtid )  /*  KLUDGE!! */
            wait();

          // cell received
          mast_pinout.data=*((TDATA*)(buf+size*i));
          mast_pinout.address=address+size*i;
          TRACE_CLK_CNT(8);
          TRACEX(STBUS_OP_TRACEX, 8, "SOURCE:%d - ", (int)SOURCE);
          TRACEX(STBUS_OP_TRACEX, 8, 
                 "ARM_STbus_interface::initiator_behav() - burst: %d = %x\n", 
                 i, (int)mast_pinout.data);
          pinout.write(mast_pinout);
	  
	  //if(SOURCE == 1)printf("Initiator:%d, READ addr:%x,data:%d,burst:%d\n",SOURCE,address,mast_pinout.data,bursts);

          if (i < bursts - 1)
          {
            ready_to_wrapper.write(true);
            wait();   // Leave the ready signal high for just one cycle, so the wrapper can read it
            ready_to_wrapper.write(false);
          }
        }
      }

      if (STATS)
        statobject->endsAccess(!rw, bursts, SOURCE);

      ready_to_wrapper.write(true);
      wait();   // Leave the ready signal high for just one cycle, so the wrapper can read it
      ready_to_wrapper.write(false);

      if (STATS)
         statobject->busFreed(SOURCE);

      wait();
    } // end while(1)
  }



};

#endif // __ST_INITIATOR_H__

