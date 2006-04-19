

#ifndef __ST_PROG_FREQ_REG_included_
#define __ST_PROG_FREQ_REG_included_

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "SC_TARGET_T3_hacked_1.h"

#include "config.h"
#include "address.h"
#include "freq_register.h"

//################
//ST_PROG_FREQ_REG
//################

template <class TDATA, class TADDRESS, class TBE, uint STBUS_SIZE, 
          uint FIFO_IN_SZ, uint FIFO_OUT_SZ, uint ANSWER_DELAY>
class  ST_PROG_FREQ_REG : public 
                    TARGET_T3_hacked1
		    <TDATA, TADDRESS, TBE, STBUS_SIZE,
                     FIFO_IN_SZ, FIFO_OUT_SZ, ANSWER_DELAY>

{
 public:
 
  sc_out<sc_uint<8> > *p_feed;
  unsigned short int outputs;
  int i;
 
  feeder *p_register;
 
     ST_PROG_FREQ_REG(sc_module_name nm, uint8_t id, TADDRESS start, TADDRESS size,
	 uint mem_in_ws1, uint mem_bb_ws1, unsigned short int outputs) :
     TARGET_T3_hacked1 <TDATA, TADDRESS, TBE, STBUS_SIZE, 
                       FIFO_IN_SZ, FIFO_OUT_SZ, ANSWER_DELAY>
  (nm,id,start,size,mem_in_ws1,mem_bb_ws1), outputs(outputs)
     {
      p_feed = new sc_out<sc_uint<8> > [outputs];
      type="ST_PROG_FREQ_REG";
      WHICH_TRACEX = FREQDEVICE_TRACEX;
      
      // Programmable register for the frequency divider
      p_register = new feeder("p_register", outputs);
      p_register->clock(clk);
     
      for (i = 0; i < outputs; i ++)
        p_register->feed[i](p_feed[i]);

      TRACEX(WHICH_TRACEX, 7,"%s:%d start:%x size:%x wait:%u, wait1:%u\n",
        type, ID, START_ADDRESS,TARGET_MEM_SIZE,mem_in_ws,mem_bb_ws);
     };
     
       
     virtual bool read_mem(TDATA& vcell, TADDRESS target_addr, uint8_t nbytes)	
     {
       TRACEX(WHICH_TRACEX, 8,"%s: read_mem @ %x at %10.1f ns\n", type,target_addr, sc_simulation_time());
	  if(!((target_addr >= START_ADDRESS)&&(target_addr <= START_ADDRESS+TARGET_MEM_SIZE)))
	    {
		//cout << name() << ":  Out of mem range access !" << endl;
		printf("%s:%d Read out of memory range address:%x-START:%x STOP:%x\n", 
		         type, ID, target_addr, (uint)START_ADDRESS, (uint)START_ADDRESS+TARGET_MEM_SIZE);
		return true;
	    }
    
	  target_addr = ((target_addr - START_ADDRESS)/4);
	    
	  vcell = p_register->get_value(target_addr);
	   TRACEX(WHICH_TRACEX, 8,
                  "%s:%d Addr:%x Read:%ud\n", type, ID, target_addr, vcell);
	    
	    return false;// r_opc = false -> transaction ok !
     }	

     virtual bool write_mem(TDATA val, TADDRESS target_addr, TBE temp_be, uint8_t nbytes)	
     {         
      TRACEX(WHICH_TRACEX, 8,"%s: write_mem %d @ %x at %10.1f ns\n", type,val,target_addr, sc_simulation_time());            
	    if(!((target_addr >= START_ADDRESS)&&(target_addr <= START_ADDRESS+TARGET_MEM_SIZE)))
	    {
		printf("%s:%d Write out of memory range address:%x-START:%x STOP:%x\n", 
		         type, ID, target_addr, (uint)START_ADDRESS, (uint)START_ADDRESS+TARGET_MEM_SIZE);
		
		return true;
	    }

            target_addr = (addressing(target_addr)/4);


            p_register->update(target_addr, val);

	    return false;    // the returned value is to put into r_opc signal (false = correct transaction)
    }
	
};
#endif
