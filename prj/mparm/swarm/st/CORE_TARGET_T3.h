///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         SEMAPHORE_T3.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         STBus semaphore device
//
///////////////////////////////////////////////////////////////////////////////




#ifndef __CORESLAVE_T3_included_
#define __CORESLAVE_T3_included_

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "address.h"

//////////////
//NEW DEFINITION OF CORESLAVE
//////////////

template <class TDATA, class TADDRESS, class TBE, uint STBUS_SIZE, 
          uint FIFO_IN_SZ, uint FIFO_OUT_SZ, uint ANSWER_DELAY>
class  CORESLAVE_T3 : public 
                    TARGET_T3_hacked1
		    <TDATA, TADDRESS, TBE, STBUS_SIZE,
                     FIFO_IN_SZ, FIFO_OUT_SZ, ANSWER_DELAY>

{
 public:
     CORESLAVE_T3(sc_module_name nm, uint8_t id, uint8_t id1, TADDRESS start, 
         TADDRESS size, uint mem_in_ws1, uint mem_bb_ws1, 
	 Mem_class *builderscra, Scratch_queue_mem *builderqueue) :
     TARGET_T3_hacked1 <TDATA, TADDRESS, TBE, STBUS_SIZE, 
                       FIFO_IN_SZ, FIFO_OUT_SZ, ANSWER_DELAY>
  (nm,id,start,size,mem_in_ws1,mem_bb_ws1)
     {
      type="CORE_TARGET";
      WHICH_TRACEX=CORESLAVE_TRACEX;
      
      ID1=id1;
         
      TRACEX(WHICH_TRACEX, 7,"%s:%d ID1:%d start:%x size:%x wait:%u,wait1:%u\n",
       type, ID, ID1, START_ADDRESS,TARGET_MEM_SIZE,mem_in_ws,mem_bb_ws);
      
      //printf("%s %d,%d address:0x%x\n", type, ID, ID1, START_ADDRESS);
      //They are initialized in the builder
      scra=builderscra;
      queue=builderqueue;
     };
 
    inline void inspect_power(int op,TADDRESS temp_addr)
    {
     //is the scratch itself that trace the power consumption...
    }
 
 private:
  Mem_class* scra;
  Scratch_queue_mem* queue;  
  uint8_t   dummy;
  uint16_t ID1; //this is the id of the core associated to this device
  
        virtual bool read_mem(TDATA& vcell, TADDRESS target_addr, uint8_t nbytes)	
	{   
	  if(!((target_addr >= START_ADDRESS)&&(target_addr <= START_ADDRESS+TARGET_MEM_SIZE)))
	    {
		cout << name() << ":  Out of mem range access !" << endl;
		return true;
	    }
	    
	    if(addresser->PhysicalInQueueSpace
	       (ID1, addresser->Logical2Physical(target_addr, ID1)))	    
	    {
	     ASSERT(nbytes==4);
	     vcell=0;
	     vcell = queue->Read((target_addr), 0);
	    }
	    else            
	     if(addresser->PhysicalInScratchSpace
		           (ID1, addresser->Logical2Physical(target_addr, ID1), &dummy)) 
	    {
	     for(uint i=0; i < nbytes; i++)
	     {
		vcell = vcell << 8;

                vcell = vcell | scra->Read((target_addr+ nbytes - i -1), 1);
	     }
	     if (STATS)
               statobject->inspectextSCRATCHAccess(target_addr, true, ID);
	     
	     
	    
	    }
	    else
	     {
	      printf("%s:%d wrong address:%x",type,ID,target_addr);
	      exit(1);
	     }
		
	   TRACEX(WHICH_TRACEX, 8,
                  "%s:%d Addr:%x Read:%ud\n", type, ID, target_addr, vcell); 
	    
	    return false;// r_opc = false -> transaction ok !
	}	

	virtual bool write_mem(TDATA val, TADDRESS target_addr, TBE temp_be, uint8_t nbytes)	
        {
            TRACEX(WHICH_TRACEX, 8,
                     "%s:%d Addr:%x write:%ud\n", type, ID, target_addr, val);

	    INTER_DATATYPE<8> temp_byte;
	    if(!((target_addr >= START_ADDRESS)&&(target_addr <= START_ADDRESS+TARGET_MEM_SIZE)))
	    {
		cout << name() << ":  Out of mem range access !" << endl;
		return true;
	    }

	    if(addresser->PhysicalInQueueSpace
	                  (ID1, addresser->Logical2Physical(target_addr, ID1)))	    
	    {
	     ASSERT(nbytes==4);
	     ASSERT(temp_be==0xF);
	     queue->Write((target_addr), val, 0);
	    }
	    else            
	     if(addresser->PhysicalInScratchSpace
		           (ID1, addresser->Logical2Physical(target_addr, ID1), &dummy)) 
	    {
	     for(uint i=0; i < nbytes; i++)
	     {
		if(temp_be & 1)
		{
		 temp_byte = val & 0x0ff;
                 scra->Write((target_addr+i), temp_byte, 1);
                }
		temp_be >>= 1;
		val = val >> 8;
	     }
	     
             if (STATS)
                statobject->inspectextSCRATCHAccess(target_addr, false, ID);
	    }
	     else
	     {
	      printf("%s:%d wrong address:%x",type,ID,target_addr);
	      exit(1);
	     }
	       
	    return false;    // the returned value is to put into r_opc signal (false = correct transaction)
	}
};

#endif  /*  __CORESLAVE_T3_included_  */


			
