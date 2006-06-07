#ifndef STBUS_TARGET_T3_BCA_H_HACKED_1
#define STBUS_TARGET_T3_BCA_H_HACKED_1

#include "power.h"
#include "mem_class.h"

template <class TDATA, class TADDRESS, class TBE, uint STBUS_SIZE, uint FIFO_IN_SZ, uint FIFO_OUT_SZ, uint ANSWER_DELAY>
SC_MODULE(TARGET_T3_hacked1)
{
	sc_in_clk clk;
	sc_in<bool> rst;

	uint ID; /* needed for the power estimation functions */
	char *type;
	uint32_t WHICH_TRACEX;
	
	stbus_T2T3_port< TDATA, TADDRESS, TBE > In;

	/************ internal signals, data and structures *************/

	/*** memory array ***/
	//uchar target_table[TARGET_MEM_SIZE];
        Mem_class* target_table;

	/*** input fifo ***/
	FIFO<struct targ_pipe_cell<TDATA, TADDRESS, TBE>, FIFO_IN_SZ> in_target_pipe;

	/*** output fifo ***/
	FIFO<struct targ_response_pipe_cell<TDATA>, FIFO_OUT_SZ> out_target_pipe;

	/*** execute FSM state ***/
	uchar exec_status;
	
	/*** sender FSM state ***/
	uchar sender_status;
	
	/*** temporary exeute FSM variables ***/
	uchar opc;
	TADDRESS temp_addr, unaligned_start_address;
	uint numcell, count_cell;
	uint nbytes, opc_numbytes;
	TDATA data_cell;
	TBE be_bit_mask;
	bool res, unaligned_address;
	uint add_offset, remaining_cells;
	unsigned long long int m_clock_counter;
	bool rst_flag;
                                                   
	int delay_counter; // number of clk delay cycles beetween gnt=1 and r_req=1
        //Martino
        bool r_opc ;

        TADDRESS START_ADDRESS;
        TADDRESS TARGET_MEM_SIZE;

	int mem_ws;
        uint mem_in_ws; // number of delay cycles for the first access to the device
        uint mem_bb_ws; // number of delay cycles for the second access to the device

	/******************************************************************** Constructor *************************************************************************/

        int member_cycle_count;
        int first_transfer;

        SC_HAS_PROCESS(TARGET_T3_hacked1);
	
	TARGET_T3_hacked1(sc_module_name nm, uint8_t id, TADDRESS start, TADDRESS size,
	 uint mem_in_ws1, uint mem_bb_ws1)
	{


	  cout << name() << ": instantiated !" << endl;
 	
 	  SC_METHOD(target_thread);
	  sensitive_pos << clk;
		
	  nbytes = STBUS_SIZE >> 3;
	  rst_flag = false;
	  
	  ID=id;
	  START_ADDRESS = start;
	  TARGET_MEM_SIZE = size;
          mem_in_ws = mem_in_ws1;
          mem_bb_ws = mem_bb_ws1;
	  WHICH_TRACEX = TARGET_TRACEX;
	}

	virtual void inspect_power(int op,TADDRESS temp_addr)
	{
	 double pwr;
	 pwr = powerRAM(ID, TARGET_MEM_SIZE, STBUS_SIZE, op);
         statobject->inspectMemoryAccess(temp_addr, (op==(int)READop)?1:0, pwr, ID);
	}
	
	virtual TADDRESS addressing(TADDRESS addr)
	{
         return addr-START_ADDRESS;
	}
	
	void target_thread()
	{
	    if(rst)
	    {
		/*** reset variables ***/
		/*** reset fsm states ***/
		exec_status = 0;
		sender_status = 1;
		be_bit_mask = 1 << (nbytes-1);
		delay_counter = 0;
		in_target_pipe.reset();
		out_target_pipe.reset();
		m_clock_counter = 0;
		rst_flag = false;
		member_cycle_count = first_transfer = -1;

	    }
	    else
	    if(!rst_flag)
	    {
		/*** set default signals ***/
		In.gnt.write(true);
		rst_flag = true;
	    }
	    else
	    {
	        /*** inc clock counter ***/
	        m_clock_counter++;

		/*** receiver ***/
		Target_receiver();

		/*** exec ***/
		Target_exec();

		Target_sender();
	    }
	}

	virtual void Target_exec()
	{
	  switch(exec_status)
	  {
	      case 0:
		  if(!in_target_pipe.empty()&&!out_target_pipe.full())
		  {
                      //Martino
		      r_opc = false;
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
		  else{
		       #ifdef EN_LEAKAGE
                       //LEAKAGE POWER
		       if (POWERSTATS) inspect_power(NOop,0);					  
		       #endif
		       break;
		     }

	      case 1:
		  if(!in_target_pipe.empty()&&!out_target_pipe.full()&&((m_clock_counter-in_target_pipe.first().time_stamp)>=ANSWER_DELAY))
		  {
                    //double pwr;
	            ASSERT( (member_cycle_count>=0) && (first_transfer>=0) );
                    if (first_transfer)
                      mem_ws=mem_in_ws;
                    else
                      mem_ws=mem_bb_ws;
                    if (member_cycle_count==mem_ws) {
	              int op;
                      if (first_transfer)
                        first_transfer=0;
                      member_cycle_count=0;
                      /* INSERISCI CHIAMATA A statobject->inspectMemoryAccess(... la memoria sta fornendo un dato ...) */
		      opc = in_target_pipe.first().opc & 0x0f;
		      if((opc == LOAD_M)||(opc == LOADG_M)||((opc == RMW_M)&&in_target_pipe.first().lck))
		        op=READop;
		      else
		        op=WRITEop;
                      if (POWERSTATS)
		        inspect_power(op,temp_addr);	
                    } else {
                      member_cycle_count++;
                      /* INSERISCI CHIAMATA A statobject->inspectMemoryAccess(... la memoria non sta fornendo un dato ...) */
                      if (POWERSTATS)
		        inspect_power(STALLop,temp_addr);
                      break;
                    }

		      opc = in_target_pipe.first().opc & 0x0f;
		      if((opc == LOAD_M)||(opc == LOADG_M)||((opc == RMW_M)&&in_target_pipe.first().lck))
		      {
			  res = read_mem(data_cell, temp_addr, nbytes);
                          //Martino
			  r_opc = r_opc || res;
                          //Martino
			  push_rcell(data_cell, r_opc, in_target_pipe.first().src, in_target_pipe.first().tid, in_target_pipe.first().pri, in_target_pipe.first().lck,
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
                          //Martino
			  r_opc = r_opc || res;
			  if(in_target_pipe.first().eop)
			  {
                              //Martino
			      push_rcell(0, r_opc, in_target_pipe.first().src, in_target_pipe.first().tid, in_target_pipe.first().pri, in_target_pipe.first().lck,
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
                          //Martino
			  r_opc = r_opc || res;
                          //Martino
			  push_rcell(data_cell, r_opc, in_target_pipe.first().src, in_target_pipe.first().tid, in_target_pipe.first().pri, in_target_pipe.first().lck,
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
		  }
		  #ifdef EN_LEAKAGE
                  else {
		  //LEAKAGE POWER
		  if (POWERSTATS) inspect_power(NOop,0);					  
		  }
		  #endif
                  break;
	  }
	}
	
	

	void Target_receiver()
	{
	    if(In.req.read()&&In.gnt)
	    {
		struct targ_pipe_cell<TDATA, TADDRESS, TBE> & pipe_cell = in_target_pipe.in_put();
		
		/*** read data fields ***/
		pipe_cell.add = In.add.read();
		pipe_cell.data = In.data.read();
		pipe_cell.opc = In.opc.read();
		pipe_cell.be = In.be.read();
		pipe_cell.eop = In.eop.read();
		pipe_cell.lck = In.lck.read();
		pipe_cell.tid = In.tid.read();
		pipe_cell.src = In.src.read();
		pipe_cell.pri = In.pri.read();
		pipe_cell.time_stamp = m_clock_counter;
	    }

	    if(in_target_pipe.full())
		In.gnt.write(false);
	    else
		In.gnt.write(true);
	}


        virtual bool read_mem(TDATA& vcell, TADDRESS target_addr, uint8_t nbytes)	
	{   
	  if(!((target_addr >= START_ADDRESS)&&(target_addr <= START_ADDRESS+TARGET_MEM_SIZE)))
	    {
		//cout << name() << ":  Out of mem range access !" << endl;
		printf("%s:%d Read out of memory range address:%x-START:%x STOP:%x\n", 
		         type, ID, target_addr, (uint)START_ADDRESS, (uint)START_ADDRESS+TARGET_MEM_SIZE);
		return true;
	    }

	    //target_addr = (target_addr - START_ADDRESS);	    
	    target_addr=addressing(target_addr);
	    
	    for(uint i=0; i < nbytes; i++)
	    {
		vcell = vcell << 8;
		    vcell = vcell | target_table->Read((target_addr+ nbytes - i -1), 1);
	    }
	 
	   TRACEX(WHICH_TRACEX, 8,
                  "%s:%d Addr:%x Read:%x\n", type, ID, target_addr, vcell);
	    
	    return false;// r_opc = false -> transaction ok !
	}	

	virtual bool write_mem(TDATA val, TADDRESS target_addr, TBE temp_be, uint8_t nbytes)	
        {
	    INTER_DATATYPE<8> temp_byte;
	    if(!((target_addr >= START_ADDRESS)&&(target_addr <= START_ADDRESS+TARGET_MEM_SIZE)))
	    {
		//cout << name() << ":  Out of mem range access !" << endl;
		printf("%s:%d Write out of memory range address:%x-START:%x STOP:%x\n", 
		         type, ID, target_addr, (uint)START_ADDRESS, (uint)START_ADDRESS+TARGET_MEM_SIZE);
		
		return true;
	    }

	    //target_addr = (target_addr - START_ADDRESS);
	    target_addr=addressing(target_addr);
	    
	    TRACEX(WHICH_TRACEX, 8,
                     "%s:%d Addr:%x write:%x\n", type, ID, target_addr, val);

	    
	    for(uint i=0; i < nbytes; i++)
	    {
		if(temp_be & 1)
		{
		    temp_byte = val & 0x0ff;
		    target_table->Write((target_addr+i), temp_byte, 1);
		}
		temp_be >>= 1;
		val = val >> 8;
	    }
   
	    return false;    // the returned value is to put into r_opc signal (false = correct transaction)
	}

	void push_rcell(TDATA temp_data, bool temp_opc, uchar vsrc, uchar vtid, uchar vpri, bool vlck, bool last_cell)
	{
	    struct targ_response_pipe_cell<TDATA> & tcell = out_target_pipe.in_put();
	    tcell.r_eop = last_cell;
	    tcell.r_opc = temp_opc;
	    tcell.r_src = vsrc;
	    tcell.r_tid = vtid;
	    tcell.r_pri = vpri;
	    tcell.r_lck = vlck;
	    tcell.r_data = temp_data;
	}
	
	void PutRespOut()
	{
	    In.r_eop.write(out_target_pipe.first().r_eop);
	    In.r_lck.write(out_target_pipe.first().r_lck);
	    In.r_opc.write(out_target_pipe.first().r_opc);
	    In.r_data.write(out_target_pipe.first().r_data);
	    In.r_src.write(out_target_pipe.first().r_src);
	    In.r_tid.write(out_target_pipe.first().r_tid);
	    In.r_pri.write(out_target_pipe.first().r_pri);
	    In.r_req = true;
	    out_target_pipe.get();
	}


	void Target_sender()
	{
	    switch(sender_status)
	    {
	      case 0:
		  if(In.r_gnt.read() == false)
		     break;
		  else
		  {
//	              out_target_pipe.get();
		      if(out_target_pipe.empty())
		      {
			  In.r_req = false;
			  In.r_eop = false;
			  In.r_data = 0;
		      }
		      sender_status = 1;
		  }
		
	      case 1:
		  if(!out_target_pipe.empty())
		  {
		      PutRespOut();
		      sender_status = 0;
		  }
		break;
	    }
	}
};

#endif
			
