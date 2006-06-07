///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         SMART_TARGET_T3.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         STBus memory device coupled with a DMA
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __SMART_T3_included
#define __SMART_T3_included


//////////////
//NEW DEFINITION OF SMARTMEM
//////////////

#include "SC_TARGET_T3_hacked_1.h"
#include "ext_mem.h"

#include "config.h"
#include "address.h"

template <class TDATA, class TADDRESS, class TBE, uint STBUS_SIZE, 
          uint FIFO_IN_SZ, uint FIFO_OUT_SZ, uint ANSWER_DELAY>
class  EXT_SMART_TARGET : public 
                    TARGET_T3_hacked1
		    <TDATA, TADDRESS, TBE, STBUS_SIZE,
                     FIFO_IN_SZ, FIFO_OUT_SZ, ANSWER_DELAY>

{
 public:
     EXT_SMART_TARGET(sc_module_name nm, uint8_t id, uint8_t id1, TADDRESS start, 
         TADDRESS size, uint mem_in_ws1, uint mem_bb_ws1, Scratch_mem *mem) :
     TARGET_T3_hacked1 <TDATA, TADDRESS, TBE, STBUS_SIZE, 
                       FIFO_IN_SZ, FIFO_OUT_SZ, ANSWER_DELAY>
  (nm,id,start,size,mem_in_ws1,mem_bb_ws1)
     {
      type="SMART_TARGET";
      WHICH_TRACEX=SMARTMEM_TRACEX;
    
      ID1=id1; //this is the id of the master port
      target_table = mem;      
      
      TRACEX(WHICH_TRACEX, 7,"%s:%d ID1:%d start:%x size:%x wait:%u,wait1:%u\n",
       type, ID, ID1, START_ADDRESS,TARGET_MEM_SIZE,mem_in_ws,mem_bb_ws);
     };

   //Poletti
   PINOUT pinout;
   // port to the DMA
   sc_inout<PINOUT> pinoutdma;
   sc_in<bool>  readydma;
   sc_out<bool> requestdma;
   //indicate the state of the program to the dma
   uint dma_state;   
   
   //ID of the master port needed for the dma
   uint ID1;
   
 protected:

        inline TADDRESS addressing(TADDRESS addr)
	{
         return addr;
	}
	
//redefined adding one new state variable:"dma_state" in order to be able to program the
// dma_controller       
	void Target_exec()
	{
	  switch(exec_status)
	  {
	      case 0:
		  if(!in_target_pipe.empty()&&!out_target_pipe.full())
		  {
		      opc = in_target_pipe.first().opc & 0x0f;
		      temp_addr = in_target_pipe.first().add;
		      unaligned_address = false;
		      opc_numbytes = (1<<((in_target_pipe.first().opc&0x70)>>4));
		      add_offset = temp_addr%opc_numbytes;
		      if(add_offset != 0)
		      {
			  unaligned_address = true;
			  unaligned_start_address = temp_addr - (temp_addr%opc_numbytes);
			  remaining_cells = add_offset/nbytes;
		      }
//		      if((opc == LOAD_M)||(opc == LOADG_M)||((opc == RMW_M)&&in_target_pipe.first().lck))
//		      {
			  numcell = opc_numbytes/nbytes;
			  if(numcell == 0)
			      numcell = 1;
			  count_cell = 0;
//		      }
		      res = false;
		      exec_status = 1;
                      member_cycle_count=0;
                      first_transfer=1;
		  }
		  else
		      break;

	      case 1:
		 if(!in_target_pipe.empty()&&!out_target_pipe.full()&&((m_clock_counter-in_target_pipe.first().time_stamp)>=ANSWER_DELAY))
		 {
                    double pwr=0;
	            ASSERT( (member_cycle_count>=0) && (first_transfer>=0) );
                    if (first_transfer)
                      mem_ws=mem_in_ws;
                    else
                      mem_ws=mem_bb_ws;
                    
		  //START_ADDRESS = indirizzo di partenza del del target +SMARTMEM_DMA_SIZE 
		  if(temp_addr-START_ADDRESS < SMARTMEM_DMA_SIZE )
		  { 
		   uint op;
		   switch(dma_state)
		   {case 0:
		     opc = in_target_pipe.first().opc & 0x0f;
		      if((opc == LOAD_M)||(opc == LOADG_M)||((opc == RMW_M)&&in_target_pipe.first().lck))
		        {op=READop;
		         //read
			 pinout.rw = 0;
			 pinout.data = 0;
		        }
		      else
		        {op=WRITEop;
			 //write
			 pinout.rw = 1;
			 pinout.data=(uint32_t)in_target_pipe.first().data;
			}
		       TRACEX(WHICH_TRACEX, 8,
                  "%s:%d,%d Addr:%x, r/w:%d\n", type, ID, ID1, temp_addr,pinout.rw);
		       
		       // create the pinout-> translation to a DMA port
    	               pinout.address = (uint32_t) temp_addr;
    	               pinout.bw=0;
                       pinout.benable = 1;
	
		       // Pass slave pinout to DMA and assert request
			pinoutdma.write(pinout);
			requestdma.write(true);
			dma_state=1;

		     break;
		   
		   case 1:
		     if(readydma.read())
		     {
		      requestdma.write(false);
		      if(pinout.rw == 0)
		      {pinout=pinoutdma.read();
		       res=false;
		       push_rcell((TDATA)pinout.data, res, in_target_pipe.first().src, in_target_pipe.first().tid, in_target_pipe.first().pri, in_target_pipe.first().lck,
		       (count_cell >= numcell-1));
		       
		       temp_addr += nbytes;
		       count_cell++;
		       
		       if(unaligned_address)
			  {
			      if(count_cell == numcell-remaining_cells)
				  temp_addr = unaligned_start_address;
			  }
		       if(count_cell >= numcell)
			  {
			      in_target_pipe.get();
			  }
		      }
		      else
		      {res=false;
		       push_rcell(0, res, in_target_pipe.first().src, in_target_pipe.first().tid, in_target_pipe.first().pri, in_target_pipe.first().lck,
		       in_target_pipe.first().eop);
		       in_target_pipe.get();
		      }
       if (POWERSTATS)
       {
        pwr = powerDMA(ID1-(DMA+1)*N_CORES,SMARTMEM_DMA_SIZE, 32, (pinout.rw==0)?READop:WRITEop);
        statobject->inspectDMAprogramAccess
	(pinout.address,(pinout.rw==0)?true:false,pwr,ID1-(DMA+1)*N_CORES);
       }		      
		      dma_state=0;
		      exec_status = 0;
		     }
		     break;
		   }  
		  }//end of dma read/write
		  else
		  {
		    if (member_cycle_count==mem_ws) {		   
	              int op;
                      if (first_transfer)
                        first_transfer=0;
                      member_cycle_count=0;
		      opc = in_target_pipe.first().opc & 0x0f;
		      if((opc == LOAD_M)||(opc == LOADG_M)||((opc == RMW_M)&&in_target_pipe.first().lck))
		        op=READop;
		      else
		        op=WRITEop;
		      if (POWERSTATS)
		        pwr = powerRAM(ID, TARGET_MEM_SIZE, STBUS_SIZE, op);
		      statobject->inspectSMARTMEMAccess(temp_addr, (op==(int)READop)?1:0, pwr, ID);
                    		                   
		   TRACEX(WHICH_TRACEX, 9,
                    "%s:%d:%d mem_acc Addr:%x R/W:%d\n", type, ID, ID1, (op==(int)READop)?1:0);
		    
		    } else {
                      member_cycle_count++;
                      break;
                    }
		      opc = in_target_pipe.first().opc & 0x0f;
		      if((opc == LOAD_M)||(opc == LOADG_M)||((opc == RMW_M)&&in_target_pipe.first().lck))
		      {
			  res = read_mem(data_cell, temp_addr, nbytes);
			  push_rcell(data_cell, res, in_target_pipe.first().src, in_target_pipe.first().tid, in_target_pipe.first().pri, in_target_pipe.first().lck,
				     (count_cell >= numcell-1));
			  temp_addr += nbytes;
			  count_cell++;

			  if(unaligned_address)
			  {
			      if(count_cell == numcell-remaining_cells)
				  temp_addr = unaligned_start_address;
			  }
			  if(count_cell >= numcell)
			  {
			      in_target_pipe.get();
			      exec_status = 0;
			  }
		      }
		      else
		      if((opc == STORE_M)||(opc == STOREG_M)||((opc == RMW_M)&&(in_target_pipe.first().lck == 0)))
		      {
			  res |= write_mem(in_target_pipe.first().data, temp_addr, 
			                   in_target_pipe.first().be, nbytes);
			  if(in_target_pipe.first().eop)
			  {
			      push_rcell(0, res, in_target_pipe.first().src, in_target_pipe.first().tid, in_target_pipe.first().pri, in_target_pipe.first().lck,
					 in_target_pipe.first().eop);
			      exec_status = 0;
			  }

			  temp_addr += nbytes;

			  if(unaligned_address)
			  {
			      count_cell++;
			      if(count_cell == numcell-remaining_cells)
				  temp_addr = unaligned_start_address;
			  }
			  
			  in_target_pipe.get();
		      }
		      else
		      if(opc == SWAP_M)
		      {
			  res = read_mem(data_cell, temp_addr, nbytes);
			  res |= write_mem(in_target_pipe.first().data, temp_addr, 
			                   in_target_pipe.first().be, nbytes);
			  push_rcell(data_cell, res, in_target_pipe.first().src, in_target_pipe.first().tid, in_target_pipe.first().pri, in_target_pipe.first().lck,
				     in_target_pipe.first().eop);
			  if(in_target_pipe.first().eop)
			      exec_status = 0;
			  if(unaligned_address)
			  {
			      count_cell++;
			      if(count_cell == numcell-remaining_cells)
				  temp_addr = unaligned_start_address;
			  }
			  else
			      temp_addr += nbytes;
			  in_target_pipe.get();			  
		      }
		   }//end of read/write smartmem memory
		  }
		  break;
	  }
	}		
};


#endif

			
