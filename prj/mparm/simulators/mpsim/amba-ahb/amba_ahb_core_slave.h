///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_ahb_core_slave.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         AMBA AHB coreslave devices
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __AMBA_AHB_CORE_SLAVE_H__
#define __AMBA_AHB_CORE_SLAVE_H__

#include <systemc.h>
#include "globals.h"
#include "address.h"
#include "amba_ahb_slave.h"
#include "stats.h"
#include "mem_class.h"
#include "scratch_mem.h"

///////////////////////////////////////////////////////////////////////////////
// amba_ahb_core_slave - This class inherits an AMBA AHB slave and provides
//                       the functionality of a core-associated slave, with
//                       access to the internal SPM of the core.
class amba_ahb_core_slave : public amba_ahb_slave
{ 
  private:
    // ID of the target scratch memory
    uint16_t ID1;
    Mem_class *scra;
    Scratch_queue_mem *queue;
    uint8_t dummy;
  
    inline void Write(uint32_t addr, uint32_t data, uint8_t bw, bool *must_wait)
    {   
      if(addresser->PhysicalInQueueSpace(ID1, addresser->Logical2Physical(addr, ID1)))
      {
        TRACEX(WHICH_TRACEX, 9, "%s %d: QUEUE Addr = 0x%08x Write Data = 0x%08x Size = %u\n",
          type, ID, (unsigned int)addr, data, bw);
        queue->Write(addr, data, bw);
      }
      else
      {
        if(addresser->PhysicalInScratchSpace(ID1, addresser->Logical2Physical(addr, ID1), &dummy))
        {      
          TRACEX(WHICH_TRACEX, 9, "%s %d: SCRATCH Addr = 0x%08x Write Data = 0x%08x Size = %u\n",
            type, ID, (unsigned int)addr, data, bw);

          scra->Write(addr, data, bw);
     
          if (STATS)
            statobject->inspectextSCRATCHAccess(addr, false, ID);
        } 
        else
        {
          printf("%s %d, %d: Wrong write address 0x%08x, cannot map it to either scratch or queue!\n", 
            type, ID, ID1, addr);
          exit(1);
        }
      }
      *must_wait = false;
    };

    inline uint32_t Read(uint32_t addr, bool *must_wait)
    {
      uint32_t data=0;
   
      if(addresser->PhysicalInQueueSpace(ID1, addresser->Logical2Physical(addr, ID1)))
      {
        data = queue->Read(addr);

        TRACEX(WHICH_TRACEX, 9, "%s %d: QUEUE Addr = 0x%08x Read  Data = 0x%08x\n",
          type, ID, (unsigned int)addr, data);
          
        *must_wait = false;
        
        return data;
      }
      else
      {
        if(addresser->PhysicalInScratchSpace(ID1, addresser->Logical2Physical(addr, ID1), &dummy))
        {
          if (STATS)
            statobject->inspectextSCRATCHAccess(addr, true, ID);
      
          data = scra->Read(addr);
      
          TRACEX(WHICH_TRACEX, 9, "%s %d: SCRATCH Addr = 0x%08x Read  Data = 0x%08x\n",
            type, ID, (unsigned int)addr, data);
          
          *must_wait = false;
      
          return data; 
        }
        else
        {
          printf("%s %d, %d: Wrong read address 0x%08x, cannot map it to either scratch or queue!\n", 
            type, ID, ID1, addr);
          exit(1);
          return 0; // Dummy return to avoid warnings - control should never get here
        }
      }
    };   
  
  public:
    amba_ahb_core_slave (sc_module_name nm,
                         uint8_t ID,
                         uint8_t ID1,
                         uint32_t start,
                         uint32_t size,
                         uint mem_in_ws,
                         uint mem_bb_ws,
                         Mem_class *scra,
                         Scratch_queue_mem *queue)
                      : amba_ahb_slave(nm, ID, start, size, mem_in_ws, mem_bb_ws),
                        ID1(ID1),
                        scra(scra),
                        queue(queue)
                        
    {
      type = "amba_ahb_core_slave";
      WHICH_TRACEX = CORESLAVE_TRACEX;
   
      TRACEX(WHICH_TRACEX, 7, "%s %d: ID1 = %d Start = 0x%08x Size = 0x%08x Wait states = %u\n",
        type, ID, ID1, START_ADDRESS, TARGET_MEM_SIZE, mem_in_ws); 
  }
};

#endif // __AMBA_AHB_CORE_SLAVE_H__
