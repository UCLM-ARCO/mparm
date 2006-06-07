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

#ifndef __SLAVE2_H__
#define __SLAVE2_H__

#include <systemc.h>
#include "globals.h"
#include "mem_class.h"
#include "address.h"
#include "amba_sig_signal.h"
#include "amba_sig_target.h"
#include "scratch_mem.h"

class slave2 : public amba_sig_slave
{ 
 private:
  
  Mem_class* scra;
  Scratch_queue_mem* queue;
  uint8_t   dummy;
  uint16_t ID1;
  
  inline void Write(uint32_t addr, uint32_t data, uint8_t bw)
  {   
   if(addresser->PhysicalInQueueSpace(ID1, addresser->Logical2Physical(addr, ID1)))
     {
      TRACEX(WHICH_TRACEX, 9,"%s:%d QUEUE Addr:%x write:%u bw:%u\n",
       type, ID, (unsigned int)addr, data, bw);
       
      queue->Write(addr, data, bw);
     }
   else
    if(addresser->PhysicalInScratchSpace(ID1, addresser->Logical2Physical(addr, ID1), &dummy))
    {      
     TRACEX(WHICH_TRACEX, 9,"%s:%d SCRATCH Addr:%x write:%u bw:%u\n",
       type, ID, (unsigned int)addr, data, bw);

     scra->Write(addr, data, bw);
     
     if (STATS)
      statobject->inspectextSCRATCHAccess(addr, false, ID);
    } 
   else
    {
     printf("%s %d,%d wrong write: I cannot map it in scratch or queue:0x%x\n", 
     type, ID, ID1, addr);
     exit(1);
    }
  };

  inline uint32_t Read(uint32_t addr)
  {
   uint32_t data=0;
   
   if(addresser->PhysicalInQueueSpace(ID1, addresser->Logical2Physical(addr, ID1)))
     {
      data=queue->Read(addr);
      
      TRACEX(WHICH_TRACEX, 9,"%s:%d QUEUE Addr:%x read:%u\n",
       type, ID, (unsigned int)addr, data);      
      
      return data; 
     }
   else
    if(addresser->PhysicalInScratchSpace(ID1, addresser->Logical2Physical(addr, ID1), &dummy))
     {
      if (STATS)
       statobject->inspectextSCRATCHAccess(addr, true, ID);
      
      data=scra->Read(addr);
      
      TRACEX(WHICH_TRACEX, 9,"%s:%d SCRATCH Addr:%x read:%u\n",
       type, ID, (unsigned int)addr, data);
      
      return data; 
     }
   else
    {
     printf("%s %d,%d wrong read: I cannot map it in scratch or queue:0x%x\n", 
     type, ID, ID1, addr);
     exit(1);
     return 0; // dummy return, just to prevent compiler warninga
    } 
  };   
  
 public:
  
  SC_HAS_PROCESS(slave2);
  
  slave2(sc_module_name nm, uint8_t id, uint8_t id1, uint32_t start, uint32_t size, 
   uint mem_in_ws1, Mem_class *builderscra, Scratch_queue_mem *builderqueue) : 
    amba_sig_slave(nm,id,start,size,mem_in_ws1)
  {
   type = "CORE_TARGET";
    
   ID1=id1;   
   
   WHICH_TRACEX=CORESLAVE_TRACEX;
   
   TRACEX(WHICH_TRACEX, 7,"%s:%d ID1:%d start:%x size:%x wait:%u\n",
       type, ID, ID1, START_ADDRESS,TARGET_MEM_SIZE,mem_in_ws); 
   
   //printf("%s %d,%d address:0x%x\n", type, ID, ID1, start);
   //They are initialized in the builder
   scra=builderscra;
   queue=builderqueue;      
  }
};

#endif // __SLAVE2_H__
