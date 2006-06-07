///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         slave0.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         AMBA bus memory slave
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __SLAVE3_H__
#define __SLAVE3_H__

#include <systemc.h>
#include "globals.h"
#include "smartmem.h"
#include "address.h"
#include "amba_sig_signal.h"
#include "amba_sig_target.h"

class slave3 : public amba_sig_slave
{ 
 private:

  PINOUT pinout;
  //ID of the master port needed for the dma
  uint8_t ID1;

  inline void Write(uint32_t addr, uint32_t data, uint8_t bw)
  {
   double pwr=0;
      
   if ((addr-START_ADDRESS) < SMARTMEM_DMA_SIZE)
   {
    // create the pinout-> translation to a DMA port
    pinout.address = addr;
    pinout.bw=bw;
    //write
    pinout.rw = 1;
    pinout.benable = 1;

    pinout.data = data; 
    
#ifdef PRINTDEBUG
    printf("%s %d receive data on a write %x \n", type, ID, pinout.data);
    printf("%s %d Receive a SMARTMEM DMA call\n", type, ID);
#endif

    // Pass slave pinout to DMA and assert request
    pinoutdma.write(pinout);
	
    requestdma.write(true);

    wait_until(readydma.delayed());

    requestdma.write(false);   
    if (POWERSTATS)
    {
     pwr = powerDMA(ID1-(DMA+1)*N_CORES,SMARTMEM_DMA_SIZE, 32, WRITEop);
      statobject->inspectDMAprogramAccess(pinout.address,(pinout.rw==0)?true:false,
      pwr,ID1-(DMA+1)*N_CORES);
    }
   }
   else
    {
#ifdef PRINTDEBUG
     printf("%s %d writing to smartmem: addr:%x, data:%d\n",type,ID,addr,data);
#endif
     if (POWERSTATS)
     {
       pwr = powerRAM(ID, TARGET_MEM_SIZE, 32, READop);
       statobject->inspectSMARTMEMAccess(addr, true, pwr, ID);
     }
     else
      if(STATS)
        statobject->inspectSMARTMEMAccess(addr, true, pwr, ID);
	
     target_mem->Write(addr, data, bw);
    }
  }

  inline uint32_t Read(uint32_t addr)
  {
   double pwr=0;
   if ((addr-START_ADDRESS) < SMARTMEM_DMA_SIZE)
  	{
  	 // create the pinout-> translation to a DMA port
	 pinout.address = addr;
	 pinout.bw = 0; //dimension of 32 bit
	 //read
	 pinout.rw = 0;
	 pinout.benable = 1;
	 pinout.data    =  0;
	
#ifdef PRINTDEBUG
	 printf("%s %d Receive a DMA call\n",type, ID);
#endif
	 // Pass slave pinout to DMA and assert request
	 pinoutdma.write(pinout);
	 requestdma.write(true);
	 wait_until(readydma.delayed());
	
	 // get the data from the dma
	 pinout = pinoutdma.read();
	 requestdma.write(false);
	
#ifdef PRINTDEBUG
	 printf("%s %d Send data back to bus %d \n",type, ID, pinout.data);
#endif    
	 
         if (POWERSTATS)
	 {
	   pwr = powerDMA(ID1-(DMA+1)*N_CORES,SMARTMEM_DMA_SIZE, 32, READop);
           statobject->inspectDMAprogramAccess(pinout.address,(pinout.rw==0)?true:false,
	   pwr,ID1-(DMA+1)*N_CORES);
	 }
	 return pinout.data;
	 
	}
   else
   	{
#ifdef PRINTDEBUG
         printf("%s %d reading from smartmem: addr:%x\n",type,ID,addr);
#endif
         if (POWERSTATS)
	 {
           pwr = powerRAM(ID, TARGET_MEM_SIZE, 32, WRITEop);
           statobject->inspectSMARTMEMAccess(addr, false, pwr, ID);
	 }
	 if(STATS)
          statobject->inspectSMARTMEMAccess(addr, false, pwr, ID); 
	 return target_mem->Read(addr);
	}
  }   
  
 public:
 
  SC_HAS_PROCESS(slave3);
  
  slave3(sc_module_name nm,uint8_t id, uint8_t id1, uint32_t start, 
         uint32_t size, uint mem_in_ws1, Scratch_mem *mem) : 
    amba_sig_slave(nm,id,start,size,mem_in_ws1)
  {
   type = "SMART_TARGET";
   target_mem = mem;
   ID1=id1;
   WHICH_TRACEX=SMARTMEM_TRACEX;
   
     TRACEX(WHICH_TRACEX, 7,"%s:%d ID1:%d start:%x size:%x wait:%u\n",
       type, ID, ID1, START_ADDRESS,TARGET_MEM_SIZE,mem_in_ws); 
   
   //printf("%s %d address:0x%x\n", type, ID, START_ADDRESS);

  }
  // port to the DMA
  sc_inout<PINOUT> pinoutdma;
  sc_in<bool>  readydma;
  sc_out<bool> requestdma;

};

#endif // __SLAVE3_H__
