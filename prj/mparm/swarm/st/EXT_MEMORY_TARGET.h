///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         EXT_MEMORY_TARGET.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         STBus memory device
//
///////////////////////////////////////////////////////////////////////////////


#ifndef __EXT_MEMORY_TARGET_included_
#define __EXT_MEMORY_TARGET_included_

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "SC_TARGET_T3_hacked_1.h"
#include "ext_mem.h"

#include "config.h"
#include "address.h"

//################
//EXT_MEMORY TARGET
//################

template <class TDATA, class TADDRESS, class TBE, uint STBUS_SIZE, 
          uint FIFO_IN_SZ, uint FIFO_OUT_SZ, uint ANSWER_DELAY>
class  EXT_MEMORY_TARGET : public 
                    TARGET_T3_hacked1
		    <TDATA, TADDRESS, TBE, STBUS_SIZE,
                     FIFO_IN_SZ, FIFO_OUT_SZ, ANSWER_DELAY>

{
 public:
     EXT_MEMORY_TARGET(sc_module_name nm, uint8_t id, TADDRESS start, TADDRESS size,
	 uint mem_in_ws1, uint mem_bb_ws1) :
     TARGET_T3_hacked1 <TDATA, TADDRESS, TBE, STBUS_SIZE, 
                       FIFO_IN_SZ, FIFO_OUT_SZ, ANSWER_DELAY>
  (nm,id,start,size,mem_in_ws1,mem_bb_ws1)
     {
      type="EXT_MEMORY_TARGET";
      target_table= new Ext_mem(id,size);
      
      addresser->pMem_classDebug[ID]=target_table;
      
      TRACEX(WHICH_TRACEX, 7,"%s:%d start:%x size:%x wait:%u, wait1:%u\n",
       type, ID, START_ADDRESS,TARGET_MEM_SIZE,mem_in_ws,mem_bb_ws);
     };

};


//################
//SHARED TARGET
//################

template <class TDATA, class TADDRESS, class TBE, uint STBUS_SIZE, 
          uint FIFO_IN_SZ, uint FIFO_OUT_SZ, uint ANSWER_DELAY>
class  EXT_SHARED_TARGET : public 
                    TARGET_T3_hacked1
		    <TDATA, TADDRESS, TBE, STBUS_SIZE,
                     FIFO_IN_SZ, FIFO_OUT_SZ, ANSWER_DELAY>

{
 public:
     EXT_SHARED_TARGET(sc_module_name nm, uint8_t id, TADDRESS start, TADDRESS size,
	 uint mem_in_ws1, uint mem_bb_ws1) :
     TARGET_T3_hacked1 <TDATA, TADDRESS, TBE, STBUS_SIZE, 
                       FIFO_IN_SZ, FIFO_OUT_SZ, ANSWER_DELAY>
  (nm,id,start,size,mem_in_ws1,mem_bb_ws1)
     {
      type="EXT_SHARED_TARGET";
      target_table= new Shared_mem(id,size);
     
      WHICH_TRACEX=SHARED_TRACEX;
     
      addresser->pMem_classDebug[ID]=target_table;
     
      TRACEX(WHICH_TRACEX, 7,"%s:%d start:%x size:%x wait:%u, wait1:%u\n",
       type, ID, START_ADDRESS,TARGET_MEM_SIZE,mem_in_ws,mem_bb_ws);
     };

};


//################
//LX MEM TARGET
//################

template <class TDATA, class TADDRESS, class TBE, uint STBUS_SIZE,
          uint FIFO_IN_SZ, uint FIFO_OUT_SZ, uint ANSWER_DELAY>
class EXT_MEMORY_LX_TARGET : public
      TARGET_T3_hacked1
		    <TDATA, TADDRESS, TBE, STBUS_SIZE,
                     FIFO_IN_SZ, FIFO_OUT_SZ, ANSWER_DELAY>
{		     
 public:
     EXT_MEMORY_LX_TARGET(sc_module_name nm, uint8_t id, TADDRESS start, TADDRESS size,
	 uint mem_in_ws1, uint mem_bb_ws1) :
     TARGET_T3_hacked1 <TDATA, TADDRESS, TBE, STBUS_SIZE, 
                       FIFO_IN_SZ, FIFO_OUT_SZ, ANSWER_DELAY>
  (nm,id,start,size,mem_in_ws1,mem_bb_ws1)		     
	{
      type="EXT_LX_TARGET";
      target_table= new Lx_mem(id,size);
      
      addresser->pMem_classDebug[ID]=target_table;
     
      TRACEX(WHICH_TRACEX, 7,"%s:%d start:%x size:%x wait:%u, wait1:%u\n",
       type, ID, START_ADDRESS,TARGET_MEM_SIZE,mem_in_ws,mem_bb_ws);
     };

};

//################
//SEMAPHORE TARGET
//################

template <class TDATA, class TADDRESS, class TBE, uint STBUS_SIZE, 
          uint FIFO_IN_SZ, uint FIFO_OUT_SZ, uint ANSWER_DELAY>
class  EXT_SEMAPHORE_TARGET : public 
                    TARGET_T3_hacked1
		    <TDATA, TADDRESS, TBE, STBUS_SIZE,
                     FIFO_IN_SZ, FIFO_OUT_SZ, ANSWER_DELAY>

{
 public:
     EXT_SEMAPHORE_TARGET(sc_module_name nm, uint8_t id, TADDRESS start, TADDRESS size,
	 uint mem_in_ws1, uint mem_bb_ws1) :
     TARGET_T3_hacked1 <TDATA, TADDRESS, TBE, STBUS_SIZE, 
                       FIFO_IN_SZ, FIFO_OUT_SZ, ANSWER_DELAY>
  (nm,id,start,size,mem_in_ws1,mem_bb_ws1)
     {
      type="EXT_SEMAPHORE_TARGET";
      target_table= new Semaphore_mem(id,size);
      WHICH_TRACEX=SEMAPHORE_TRACEX;      
      
      addresser->pMem_classDebug[ID]=target_table;
      
      TRACEX(WHICH_TRACEX, 7,"%s:%d start:%x size:%x wait:%u, wait1:%u\n",
       type, ID, START_ADDRESS,TARGET_MEM_SIZE,mem_in_ws,mem_bb_ws);
     };
       
     virtual bool read_mem(TDATA& vcell, TADDRESS target_addr, uint8_t nbytes)	
     {
          	  
	  ASSERT(nbytes==4);
	  
	  if(!((target_addr >= START_ADDRESS)&&(target_addr <= START_ADDRESS+TARGET_MEM_SIZE)))
	    {
		cout << name() << ":  Out of mem range access !" << endl;
		return true;
	    }
	    target_addr = (target_addr - START_ADDRESS);	    
	    
	    //vcell=0;
		
            vcell = target_table->Read((target_addr), 0);
	    
	    TRACEX(WHICH_TRACEX, 8,
                     "%s:%d Addr:%x Read:%ud\n", type, ID, target_addr, (uint)vcell);
	    
	    return false;// r_opc = false -> transaction ok !
     }	

     virtual bool write_mem(TDATA val, TADDRESS target_addr, TBE temp_be, uint8_t nbytes)	
     {

	    ASSERT(temp_be==0xF);
	    ASSERT(nbytes==4);
	    
	    INTER_DATATYPE<8> temp_byte;
	    if(!((target_addr >= START_ADDRESS)&&(target_addr <= START_ADDRESS+TARGET_MEM_SIZE)))
	    {
		cout << name() << ":  Out of mem range access !" << endl;
		return true;
	    }

	    target_addr = (target_addr - START_ADDRESS);

	    target_table->Write((target_addr), val, 0);
	    
	    TRACEX(WHICH_TRACEX, 8,
                     "%s:%d Addr:%x write:%d\n", type, ID, target_addr, val);   
   
	    return false;    // the returned value is to put into r_opc signal (false = correct transaction)
    }
};



//################
//INTERRUPT TARGET
//################

template <class TDATA, class TADDRESS, class TBE, uint STBUS_SIZE, 
          uint FIFO_IN_SZ, uint FIFO_OUT_SZ, uint ANSWER_DELAY>
class  EXT_INTERRUPT_TARGET : public 
                    TARGET_T3_hacked1
		    <TDATA, TADDRESS, TBE, STBUS_SIZE,
                     FIFO_IN_SZ, FIFO_OUT_SZ, ANSWER_DELAY>

{
 public:
     EXT_INTERRUPT_TARGET(sc_module_name nm, uint8_t id, TADDRESS start, TADDRESS size,
	 uint mem_in_ws1, uint mem_bb_ws1) :
     TARGET_T3_hacked1 <TDATA, TADDRESS, TBE, STBUS_SIZE, 
                       FIFO_IN_SZ, FIFO_OUT_SZ, ANSWER_DELAY>
  (nm,id,start,size,mem_in_ws1,mem_bb_ws1)
     {
      type="EXT_INTERRUPT_TARGET";
      WHICH_TRACEX=INTERRUPT_TRACEX;
      
      extint = new sc_inout<bool> [N_CORES];      
      
      TRACEX(WHICH_TRACEX, 7,"%s:%d start:%x size:%x wait:%u, wait1:%u\n",
       type, ID, START_ADDRESS,TARGET_MEM_SIZE,mem_in_ws,mem_bb_ws);
     };

    void rst_thread()
    {
      if(rst) {
        TRACEX(WHICH_TRACEX, 8, 
               "INTERRUPT_T3::rst_thread() - resetting  rst: %d  clk: %d\n",
               (int)rst, (int)clk);
        for(int i=0;i<N_CORES;i++) extint[i].write(false);
      }
    };

   sc_inout<bool> *extint;

 protected:       
     virtual bool read_mem(TDATA& vcell, TADDRESS target_addr, uint8_t nbytes)	
     {
      printf("Fatal error: Interrupt slave is a write only slave, received read request at time %10.1f\n",
        sc_simulation_time());
      exit(1);
      return false;// r_opc = false -> transaction ok !
     }	

     virtual bool write_mem(TDATA val, TADDRESS target_addr, TBE temp_be, uint8_t nbytes)	
     {
         int intno;

	 ASSERT(temp_be==0xF);
	 ASSERT(nbytes==4);

	 if(!((target_addr >= START_ADDRESS)&&(target_addr <= START_ADDRESS+TARGET_MEM_SIZE)))
	 {
	  cout << name() << ":  Out of mem range access !" << endl;
	  return true;
	 }

         // Calculate the interrupt number
         intno = ((target_addr - START_ADDRESS) / 4) - 1;
         if (intno < 0 || intno >= N_CORES)
         {
           printf("%s: Fatal Error: cannot send interrupt on wire %d!\n", name(), intno);
           exit(1);
         }

         if(val>0){
		extint[intno].write(true);
		TRACEX(WHICH_TRACEX, 8,
                     "%s:%d addr:%x send_int%d\n", type, ID, target_addr, intno);   
	 }
	 else{
	 	extint[intno].write(false);
		TRACEX(WHICH_TRACEX, 8,
                     "%s:%d addr:%x clear_int%d\n", type, ID, target_addr, intno);   
	}

	return false;    // the returned value is to put into r_opc signal (false = correct transaction)
    }
	
};



#endif
