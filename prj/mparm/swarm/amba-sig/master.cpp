///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         master.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         AMBA bus master
//
///////////////////////////////////////////////////////////////////////////////

#include "master.h"
#include "stats.h"
#include "config.h"
#include "globals.h"
#include "address.h"

///////////////////////////////////////////////////////////////////////////////
// working - Performs master operation. Listens to request coming from
//           processors (via wrappers) or DMAs and communicates with the AMBA
//           bus. Supports burst transfers.
void amba_sig_master::working()
{
  request varstruct;
  uint32_t din, dout, addr, prevaddr, size;
  uint burst=0, i;
  bool firstwait, wr;
  PINOUT mast_pinout;

  // This line causes the decoder to notice a change in address lines and thus to initialize
  // Address value can be anything, provided it is in the space of the slave number 0
  address.write(1);
  ready_to_wrapper.write(false);

#ifdef PRINTDEBUG
  printf("Master %hu starting\n", my);
#endif

  while(1)
  {
      // Wait until someone requests something
      wait_until(request_from_wrapper.delayed() == true);
      // What's up?
      mast_pinout = pinout.read();
      // Word, half word or byte?
      switch (mast_pinout.bw)
      {
        case 0 :  varstruct.hsize = AMBA_SIG_HSIZE_WORD;
		  size = 0x4;
		  break;
        case 1 :  varstruct.hsize = AMBA_SIG_HSIZE_BYTE;
		  size = 0x1;
                  break;
        case 2 :  varstruct.hsize = AMBA_SIG_HSIZE_HALFWORD;
		  size = 0x2;
		  break;
	default : printf("Fatal error: Master %hu detected a malformed data size at time %10.1f\n",
                           my, sc_simulation_time());
                  exit(1);
      }

      burst = mast_pinout.burst;
      wr = mast_pinout.rw;
      addr = addresser->Logical2Physical(mast_pinout.address, my);
      dout = mast_pinout.data;

      if (burst == 1)
        varstruct.hburst = AMBA_SIG_HBURST_SINGLE;
      if (burst == 4)
        varstruct.hburst = AMBA_SIG_HBURST_INCR4;
      if (burst == 8)
        varstruct.hburst = AMBA_SIG_HBURST_INCR8;
      if (burst == 16)
        varstruct.hburst = AMBA_SIG_HBURST_INCR16;

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
        ready_to_wrapper.write(true);
        firstwait=true;
        do
        {
          wait();
          if (firstwait)
          {
            firstwait=false;
            ready_to_wrapper.write(false);
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
        mast_pinout = pinout.read();
        dout = mast_pinout.data;
        hwdata.write (dout);
	ready_to_wrapper.write(true);
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
	    mast_pinout.data = din;
#ifdef EXTRAMPRINTDEBUG
	    printf("Master %hu: single read  %8x at address 0x%08x\n\n",my,(uint)din,(uint)addr);
#endif
	  }
#ifdef EXTRAMPRINTDEBUG
	  else      // single write
	    printf("Master %hu: single write %8x at address 0x%08x\n\n", my,(uint)dout,(uint)addr);
#endif
	  pinout.write(mast_pinout);
	  ready_to_wrapper.write(true);

          if (STATS)
	    statobject->endsAccess(!wr, burst, my);

	  wait();
	  ready_to_wrapper.write(false);
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
	         ready_to_wrapper.write(false);
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
		 mast_pinout = pinout.read();
                 dout = mast_pinout.data;
                 hwdata.write (dout);
		 ready_to_wrapper.write(true);
		}
	      } // end for

              if (STATS)
	        statobject->endsAccess(!wr, burst, my);

	      wait();
	      ready_to_wrapper.write(false);
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
		  mast_pinout.data=din;
		  
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
		  pinout.write(mast_pinout);
		  ready_to_wrapper.write(true);

		  if (STATS)
		    if (i==burst-1)
		      statobject->endsAccess(!wr, burst, my);

		  wait();
		  ready_to_wrapper.write(false);
	      } // end for
	    } // end read burst
      } // end burst

      if (STATS)
        statobject->busFreed(my);

   } // end while
}
