///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_ahb_dma.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Implements a complete DMA (bus side)
//
///////////////////////////////////////////////////////////////////////////////

//This Class implement a DMA to be connected to the amba-ahb bus

#include "amba_ahb_dma.h"
#include "stats.h"

#define AMBA_AHB_MIN_DIM_BURST 0x4

bool amba_ahb_dma_transfer :: Write(uint32_t addr, uint32_t* data, uint32_t nburst)
{
 uint i,burstcounter;
 PINOUT pinout_mast;
 
 TRACEX(WHICH_TRACEX, 12,"%s:%d write addr:%x,nburst:%d\n",
         type,ID,addr,nburst);
 
 if(local(addr))
 {
  for(i=0;i<nburst;i++)
  {
   if (STATS)
     statobject->inspectDMAAccess(addr, false, ID);
     
   //scra->Write(addr,data[i],0);
   write_local(addr,data[i]);
   wait();
   addr+=4;
  } 
 }//end of scratch write
 else
 {
  //setup the bus for the master
  pinout_mast.address = addr;
  pinout_mast.rw = 1; //write
  pinout_mast.bw = 0; //dimension of 32 bit
  pinout_mast.benable = 1;
  
  if((nburst < (uint32_t)AMBA_AHB_MIN_DIM_BURST))
  {
   pinout_mast.burst=1;
   for(i=0;i<nburst;i++)
   {      
    request_to_master.write(true);
    pinout_mast.data=data[i];
    pinout.write(pinout_mast);

    if (STATS)
     statobject->inspectDMAAccess(pinout_mast.address, false, ID);

    wait_until(ready_from_master.delayed() == true);
    request_to_master.write(false);
    pinout_mast.address+=4;
   }  
  }
  else
  {
   //////////////
   //start burst_write
   pinout_mast.burst = nburst;
   pinout_mast.data = buffer[0];
   //write the bus for the master
   pinout.write(pinout_mast);
   //request the master
   request_to_master.write(true);
   burstcounter = 0;
	    
   while(burstcounter<nburst)
   {

    TRACEX(WHICH_TRACEX, 12,"%s:%d waiting for the bus\n",type,ID);
    
    if (STATS)
    statobject->inspectDMAAccess(pinout_mast.address+4*burstcounter, false, ID);

    wait_until(ready_from_master.delayed() == true);

    TRACEX(WHICH_TRACEX, 12,"%s:%d I got the the bus\n",type,ID);
 
    burstcounter++;
    if (burstcounter<nburst)
    {//write the data
     pinout_mast.data = data[burstcounter];
     pinout.write(pinout_mast);
    }
    
    if (burstcounter<nburst-1) request_to_master.write(false);
   }    
    pinout_mast.data = data[0];
    pinout_mast.address+=4*nburst;

  }//end of the burst write
 }//end of master write
 return true;
};

bool amba_ahb_dma_transfer::Read(uint32_t addr, uint32_t* data, uint32_t nburst)
{
 uint i,burstcounter;
 PINOUT pinout_mast; 

 TRACEX(WHICH_TRACEX, 12,"%s:%d read addr:%x,nburst:%d\n",
         type,ID,addr,nburst);
 
 if(local(addr))
 {
  for(i=0;i<nburst;i++)
  {
   if (STATS)
     statobject->inspectDMAAccess(addr, false, ID);
     
   //data[i]=scra->Read(addr);
   data[i]= read_local(addr);
   wait();
   addr+=4;
   
  }
 }//end of scratch read
 else
 {
  //set the bus for the master
  pinout_mast.address = addr;
  pinout_mast.rw = 0; //read
  pinout_mast.bw = 0;
  pinout_mast.benable = 1;
  
  if((nburst < (uint32_t)AMBA_AHB_MIN_DIM_BURST))
  {
   pinout_mast.burst=1;
   for(i=0;i<nburst;i++)
   { 
    //write the bus for the master
    pinout.write(pinout_mast);

    request_to_master.write(true);
    pinout.write(pinout_mast);

    if (STATS)
    statobject->inspectDMAAccess(pinout_mast.address, true, ID);
   
    wait_until(ready_from_master.delayed() == true);
	      
    pinout_mast = pinout.read();
    data[i]=pinout_mast.data; 
    request_to_master.write(false);
    pinout_mast.address+=4;   
   }
  }//end of single read
  else
  {
   pinout_mast.burst = nburst;
   request_to_master.write(true);
   pinout.write(pinout_mast);
   burstcounter = 0;
   while(burstcounter<nburst)
	     {
              TRACEX(WHICH_TRACEX, 12,"%s:%d waiting for the bus\n",type,ID);
	      
	      if (STATS)
		statobject->inspectDMAAccess(pinout_mast.address+4*burstcounter, true, ID);

	      wait_until(ready_from_master.delayed() == true);

              TRACEX(WHICH_TRACEX, 12,"%s:%d I got the the bus\n",type,ID);

	      //read the data
	      pinout_mast = pinout.read();
	      data[burstcounter]=pinout_mast.data;
	      ++burstcounter;
	     }

   pinout_mast.address+=4*nburst;
   request_to_master.write(false);
  }//end of burst read
   
 }//end of master read
 return true;
};
