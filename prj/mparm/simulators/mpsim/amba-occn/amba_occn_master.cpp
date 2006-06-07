///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_occn_master.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         AMBA (OCCN) bus master
//
///////////////////////////////////////////////////////////////////////////////

#include "amba_occn_master.h"

///////////////////////////////////////////////////////////////////////////////
// working - Performs master operation. Listens to request coming from
//           processors (via wrappers) or DMAs and communicates with the AMBA
//           bus. Supports burst transfers.
void amba_occn_master::working()
{
  uint32_t din, dout, addr, size;
  uint burst=0, i;
  bool wr;
  PINOUT mast_pinout;

  Pdu<AhbMasterCtrl,N_int32> *tx_pdu = new Pdu<AhbMasterCtrl,N_int32>;
  Pdu<AhbSlaveCtrl,N_int32>  *rx_pdu = new Pdu<AhbSlaveCtrl,N_int32>;
  
  ready_to_wrapper.write(false);

#ifdef PRINTDEBUG
  printf("Master %hu starting\n", my);
#endif
  
  
  // AST-Grenoble: Initialize master request
  occn_hdr(*tx_pdu,hbusreq)=false;
  occn_hdr(*tx_pdu,hlock)=false;
  port.asend(tx_pdu);

  while(1)
  {
      //AST-Grenoble:  Wait until Processor requests something                 
      do
        wait(clock.posedge_event());
      while (request_from_wrapper.read() == false);

      //AST_Grenoble: getting controls from wrapper
      mast_pinout = pinout.read();

      
      //AST_Grenoble:  data size setting
      switch (mast_pinout.bw)
      {
        case 0 :  occn_hdr(*tx_pdu,hsize)=4; 
		  size = 0x4;
		  break;
        case 1 :  occn_hdr(*tx_pdu,hsize)=1;
		  size = 0x1;
                  break;
        case 2 :  occn_hdr(*tx_pdu,hsize)=2;
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

      //AST_Grenoble: Burst setting
      if (burst == 1)
        occn_hdr(*tx_pdu,hburst)= AhbArb::SINGLE;
      if (burst == 4)
        occn_hdr(*tx_pdu,hburst)= AhbArb::INCR4;
      if (burst == 8)
        occn_hdr(*tx_pdu,hburst)= AhbArb::INCR8;
      if (burst == 16)
        occn_hdr(*tx_pdu,hburst)= AhbArb::INCR16;

       //AST_Grenoble: opcode setting
      if (wr)
        occn_hdr(*tx_pdu,hwrite)= 1;
      else
        occn_hdr(*tx_pdu,hwrite)= 0;

      occn_hdr(*tx_pdu,htrans)= AhbArb::NONSEQ;
      
      if (STATS)
        statobject->requestsAccess(my);
      
      occn_hdr(*tx_pdu,haddr)= addr;

      if (burst==1)
      {           
          occn_hdr(*tx_pdu,hbusreq)=true;   // AST-Grenoble: requesting bus for single transfer
          occn_hdr(*tx_pdu,hlock)=false;          	            
      }
      else
      {
          occn_hdr(*tx_pdu,hbusreq)=true;
	  occn_hdr(*tx_pdu,hlock)=true;   // AST-Grenoble: atomic burst must be locked
      } 
      
     

      
      //AST_Grenoble: Burst write, set ready_to_wrapper to get second data
      if ((burst!=1)&&(wr))
        rdy_ev.notify();
                           
     
      port.send(tx_pdu); //AST_Grenoble: Blocking send waiting for bus ownership      

                                 
       // AST_Grenoble: address incrementing managed automatically by the OCCN AHB model
      
      // If it is a burst cycle, increment the address
      // if(burst!=1)
      //{        
	//prevaddr = addr;
	//addr+=size;
      //#ifdef PRINTDEBUG
      //      printf("Master %hu: address %x, size %x", my, addr, size);
      //#endif          
      //   }
  
      if (STATS)
        statobject->beginsAccess(2, !wr, burst, my);

      if (occn_hdr(*tx_pdu,hburst) == AhbArb::SINGLE)   //AST_Grenoble: Single transfer
      {
	  // AST-Grenoble:  Next iteration, transaction will be already finished

          // AST-Grenoble: Bus leaving
	  occn_hdr(*tx_pdu,hbusreq)= false;
          occn_hdr(*tx_pdu,hlock)= false; 
          occn_hdr(*tx_pdu,htrans) = AhbArb::IDLE;         

          // AST_Grenoble: second stage in pipeline,master drives data on the bus
          if (wr)
           *tx_pdu = dout;

	  port.asend(tx_pdu); // AST-Grenoble:  non blocking send to copy in the MsgBox hbusreq=false
	           
	  if (!wr)  //AST-Grenoble: Single Read
	  {
            *rx_pdu = *port.receive();//AST-Grenoble: blocking receive waiting for end of data transfer and data read  (HREADY='1') 
	    din = *rx_pdu; 
            port.reply();   
            mast_pinout.data = din;

#ifdef EXTRAMPRINTDEBUG
	    printf("Master %hu: single read  %8x at address 0x%08x\n\n",my,(uint)din,(uint)addr);
#endif
	  }
          else  //AST-Grenoble: Single Write
	  {
            *rx_pdu = *port.receive(); //AST-Grenoble: blocking receive waiting for end of data transfer (HREADY='1') 
            port.reply();   

#ifdef EXTRAMPRINTDEBUG	        
	    printf("Master %hu: single write %8x at address 0x%08x\n\n", my,(uint)dout,(uint)addr);        
#endif

          }
          //AST-Grenoble: Master drives data to ARM processor
          pinout.write(mast_pinout);
          rdy_ev.notify();
          
          if (STATS)
	    statobject->endsAccess(!wr, burst, my);
                                                         
          wait(clock.posedge_event());

          
      }
      else   //------------------------Burst transfer
      {
          if (wr) //AST_Grenoble: Burst Write
	  {
              //AST-Grenoble:  Next iteration will be the second, so switch to SEQ
	      occn_hdr(*tx_pdu,htrans) = AhbArb::SEQ;
	      
             
	      for (i = 0; i < burst; i++)
	      {
                 //AST-Grenoble: drives data on data bus (data pipeline stage)
                  *tx_pdu = dout;
                  
                  mast_pinout = pinout.read();
                  dout = mast_pinout.data;
                 
                  if (i < (burst-1))
                    rdy_ev.notify();
                  
                  //AST_Grenoble: If Burst end,master does not wait hready and hgrant signals (asend() if i==(burst-1)) 
		  if (i==burst-1)
              	    port.asend(tx_pdu);                    
                  else
		    port.send(tx_pdu);                  
                
                  port.receive();  //AST-Grenoble: blocking receive waiting for end of data transfer (HREADY='1') 
                  port.reply();

                  //#ifdef EXTRAMPRINTDEBUG
                  //printf("Master %hu: burst  read  %8x at address 0x%08x\n", my, uint(din), (uint)prevaddr);
                  //if(i==burst-1)
                  //printf("\n");
                  //prevaddr = addr;
                  //#endif
                  
                  if(i==burst-3)
		  {		   
                   occn_hdr(*tx_pdu,hbusreq)=false;
                   occn_hdr(*tx_pdu,hlock)=false; // AST-Grenoble: release lock
		  }
	          
                  //AST-Grenoble: Next iteration will be the last, so switch to IDLE
		  if(i==burst-2)
		  {
		   occn_hdr(*tx_pdu,htrans) = AhbArb::IDLE;
                   occn_hdr(*tx_pdu,hbusreq)=false;
                   occn_hdr(*tx_pdu,hlock)=false; // AST-Grenoble: release lock                   
		  }
		  
		 
		  if (STATS)
		    if (i==burst-1)
		      statobject->endsAccess(!wr, burst, my);

	      } // end for             
              
              
              wait(clock.posedge_event());
	    
	  }  // end write burst		
	 else  //AST-Grenoble:  read burst          
	  {
	      //AST-Grenoble:  Next iteration will be the second, so switch to SEQ
	      occn_hdr(*tx_pdu,htrans) = AhbArb::SEQ;
	      
            
	      for (i = 0; i < burst; i++)
	      {
                  //AST_Grenoble: Burst end,master does not wait hready and hgrant signals (asend() if i==(burst-1))
		  if (i==burst-1)
              	    port.asend(tx_pdu);
                  else
		    port.send(tx_pdu);

                  *rx_pdu = *port.receive(); //AST-Grenoble: blocking receive waiting for end of data transfer and data read  (HREADY='1') 
	    	  din = *rx_pdu;
            	  port.reply();
		  mast_pinout.data=din;

                 
		  
                  //#ifdef EXTRAMPRINTDEBUG
                  //printf("Master %hu: burst  read  %8x at address 0x%08x\n", my, uint(din), (uint)prevaddr);
                  //if(i==burst-1)
                  //printf("\n");
                  //prevaddr = addr;
                  //#endif

                  if(i==burst-3)
		  {		   
                   occn_hdr(*tx_pdu,hbusreq)=false;
                   occn_hdr(*tx_pdu,hlock)=false; // AST-Grenoble: release lock
		  }
	          // AST-Grenoble:  Next iteration will be the last, so switch to IDLE
		  if(i==burst-2)
		  {
		   occn_hdr(*tx_pdu,htrans) = AhbArb::IDLE;
                   occn_hdr(*tx_pdu,hbusreq)=false;
                   occn_hdr(*tx_pdu,hlock)=false; // AST-Grenoble: release lock
                   
		  }
		  //AST-Grenoble: Master  drives burst data to ARM processor
		  pinout.write(mast_pinout);
		  rdy_ev.notify();
                 
		  if (STATS)
		    if (i==burst-1)
		      statobject->endsAccess(!wr, burst, my);

	      } // end for   
          
              

              wait(clock.posedge_event());
	    } // end read burst
      } // end burst
    
      if (STATS)
        statobject->busFreed(my);
      
   } // end while
}





void amba_occn_master::pulse_ready_generator()
{
do
{
  ready_to_wrapper.write(false);
  wait(rdy_ev);
  ready_to_wrapper.write(true);
  wait(clock.posedge_event());
}
while(1);
}



