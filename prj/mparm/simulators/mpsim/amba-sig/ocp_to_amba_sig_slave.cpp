///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         ocp_to_amba_sig_slave.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Links an OCP (slave) interface to an AMBA (signal model) bus
//
///////////////////////////////////////////////////////////////////////////////

#include "ocp_to_amba_sig_slave.h"
#include "stats.h"
#include "globals.h"
#include "address.h"

///////////////////////////////////////////////////////////////////////////////
// working - Performs master operation. Listens to request coming from
//           an OCP slave interface and communicates with the AMBA
//           bus. Supports burst transfers.
void ocpambasigslave::working()
{
  request varstruct;
  uint32_t din, dout, addr, prevaddr, size;
  uint burst=0, ocpburst, i;
  bool firstwait, wr;

  // This line causes the decoder to notice a change in address lines and thus to initialize
  // Address value can be anything, provided it is in the space of the slave number 0
  address.write(1);
  // Initialization of OCP signals
  SResp.write(OCPSRESNULL);
  SCmdAccept.write(false);
  SDataAccept.write(false);
  SData.write(0);

#ifdef PRINTDEBUG
  printf("Master %hu starting\n", my);
#endif

//FIXME debug
//#define EXTRAMPRINTDEBUG

  while(1)
  {
      // Wait until someone requests something
      do
        wait();
      while (MCmd.read() == OCPCMDIDLE);
      // Word, half word or byte?
      switch (MByteEn.read())
      {
        case OCPMBYEWORD:  varstruct.hsize = AMBA_SIG_HSIZE_WORD;
		           size = 0x4;
		           break;
        case OCPMBYEBYTE:  varstruct.hsize = AMBA_SIG_HSIZE_BYTE;
		           size = 0x1;
		           break;
        case OCPMBYEHWRD:  varstruct.hsize = AMBA_SIG_HSIZE_HALFWORD;
		           size = 0x2;
		           break;
	default :          printf("Fatal error: Master %hu detected a malformed data size at time %10.1f\n",
		             my, sc_simulation_time());
		           exit(1);
      }

      ocpburst = (uint)MBurst.read();
      //FIXME wr has multiple possibilities
      wr = (MCmd.read() == OCPCMDWRITE) ? true : false;
      addr = addresser->Logical2Physical(MAddr.read(), my);
      dout = MData.read();

      //FIXME: is this reliable? (and no support for INCR16 etc)
      if (ocpburst == OCPBURLAST)
      {
        burst = 1;
        varstruct.hburst = AMBA_SIG_HBURST_SINGLE;
      }
      if (ocpburst == OCPBURINC4)
      {
        burst = 4;
        varstruct.hburst = AMBA_SIG_HBURST_INCR4;
      }
      if (ocpburst == OCPBURINC8)
      {
        burst = 8;
        varstruct.hburst = AMBA_SIG_HBURST_INCR8;
      }
      
      if (wr)
        varstruct.hwrite = true;
      else
        varstruct.hwrite = false;

      varstruct.htrans = AMBA_SIG_HTRANS_NONSEQ;
      
      if (STATS)
        statobject->requestsAccess(my);

      hreq.write(true);
      address.write (addr);
      mast.write (varstruct);
      
      // If it is a write, put data on the bus
      if (wr)
        hwdata.write (dout);
      
      // Waiting for bus ownership
      if(wr && burst!=1) 
      {
        SResp.write(OCPSRESDVA);
        firstwait=true;
        do
        {
          wait();
          if (firstwait)
          {
            firstwait=false;
            SResp.write(OCPSRESNULL);
          }
        }
        while(!(hgrant.read() && hready.read()));
      }
      else
         wait_until (hgrant.delayed() == true && hready.delayed() == true); 

      // I really got the bus
      hreq.write (false);
 
      // If it is a burst cycle, increment the address
      if(burst!=1)
      {
       prevaddr = addr;
       addr+=size;
#ifdef PRINTDEBUG
       printf("Master %hu: address %x, size %x", my, addr, size);
#endif
       address.write (addr);
       if (wr) 
       {
        dout = MData.read();
        hwdata.write (dout);
	SResp.write(OCPSRESDVA);
       }
      }
      
      if (STATS)
        statobject->beginsAccess(2, !wr, burst, my);

      if (varstruct.hburst == AMBA_SIG_HBURST_SINGLE)   //------------------------Single transfer
      {
	  // Next iteration, transaction will be already finished
	  varstruct.htrans = AMBA_SIG_HTRANS_IDLE;
	  mast.write(varstruct);
	  wait_until (hready.delayed () == true);
	  
	  if (!wr)  // single read
	  {
	    din = hrdata.read();
	    SData.write(din);
#ifdef EXTRAMPRINTDEBUG
	    printf("Master %hu: single read  %8x at address 0x%08x\n\n",my,(uint)din,(uint)addr);
#endif
	  }
#ifdef EXTRAMPRINTDEBUG
	  else      // single write
	    printf("Master %hu: single write %8x at address 0x%08x\n\n", my,(uint)dout,(uint)addr);
#endif
	  SResp.write(OCPSRESDVA);

          if (STATS)
	    statobject->endsAccess(!wr, burst, my);

	  wait();
	  SResp.write(OCPSRESNULL);
      }
      else                              //------------------------Burst transfer
      {
	  if (wr) // write burst
	  {
	      // Next iteration will be the second, so switch to SEQ
	      varstruct.htrans = AMBA_SIG_HTRANS_SEQ;
	      mast.write (varstruct);
	      for (i = 0; i < burst; i++)
	      {firstwait=true;
	       do
	       {wait();
	        if (firstwait)
		{firstwait=false;
	         SResp.write(OCPSRESNULL);
	        }
	       }while(!hready.read());

	       if(i>=burst-2)
	       {
	        varstruct.htrans = AMBA_SIG_HTRANS_IDLE;
	        mast.write (varstruct);
	       }
	       else
		{// Burst cycle, increment the address
		 addr+=size;
                 address.write (addr);
                 dout = MData.read();
                 hwdata.write (dout);
		 SResp.write(OCPSRESDVA);
		}
	      } // end for

              if (STATS)
	        statobject->endsAccess(!wr, burst, my);

	      wait();
	      SResp.write(OCPSRESNULL);
#ifdef EXTRAMPRINTDEBUG
	      printf("\n");
#endif
	  }
	  else  // read burst
	  {
	      // Next iteration will be the second, so switch to SEQ
	      varstruct.htrans = AMBA_SIG_HTRANS_SEQ;
	      mast.write (varstruct);

	      for (i = 0; i < burst; i++)
	      {
		  wait_until (hready.delayed () == true);
		  din = hrdata.read ();
		  SData.write(din);
		  
#ifdef EXTRAMPRINTDEBUG
	          printf("Master %hu: burst  read  %8x at address 0x%08x\n", my, uint(din), (uint)prevaddr);
		  if(i==burst-1)
		    printf("\n");
		  prevaddr = addr;
#endif
	          // Next iteration will be the last, so switch to IDLE
		  if(i==burst-2)
		  {
		   varstruct.htrans = AMBA_SIG_HTRANS_IDLE;
	           mast.write (varstruct);
		  }
		  else
		  {// burst cycle I need to increment the address
		   addr+=size;
                   address.write (addr);
		  }
		  SResp.write(OCPSRESDVA);

		  if (STATS)
		    if (i==burst-1)
		      statobject->endsAccess(!wr, burst, my);

		  wait();
		  SResp.write(OCPSRESNULL);
	      } // end for
	    } // end read burst
      } // end burst

      if (STATS)
        statobject->busFreed(my);

   } // end while
}

