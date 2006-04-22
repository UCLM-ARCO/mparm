#include "statemachine3.h"

void statemachine3::FSM_logic()
{

//printf("State:%d burst_size:%d ",state,burst_size);
//printf("  Time %u\n",(uint)(sc_simulation_time()));
//printf("reset:%d\n",reset_FSM.read());
 #if 0
 TRACEX(WHICH_TRACEX, 8,"%s reset:%d\n",type,reset_FSM.read());
 TRACEX(WHICH_TRACEX, 10,"%s State:%d burst_size:%d ",type,state,burst_size);
 TRACEX(WHICH_TRACEX, 12," %s at Time %u\n",type,(uint)(sc_simulation_time()));
 #endif
 
  if(reset_FSM.read() == true)
  {
    size = 0;
    address = 0;
    next_state = DRAMDMA_IDLE_0;
    write_done = false;
    read_done = false;
    index = 0;
    number = 0;
    burst_size = 0;
    cb = false;
    tWR = TWR_SDRAM;
    command_counter = 0;
    number_dec = 1;
    total = 0;
    total_pre = 0;
    
    TRACEX(WHICH_TRACEX, 10,"%s State:%d, next_state:%d \n",type,state,next_state);
    TRACEX(WHICH_TRACEX, 12," %s at Time %u\n",type,(uint)(sc_simulation_time()));
  }
  else
  {

state = next_state;
switch(state)
{
/////////////////////////////////////////////////////////////////////////////
// STATE MACHINE PART
/////////////////////////////////////////////////////////////////////////////
  case DRAMDMA_IDLE_0:
  
  // printf("request:%d ready:%d\n",request.read(),ready.read());
  #if 0
   TRACEX(WHICH_TRACEX, 8,"%s IDLE_0 state in state machine\n",type);
   TRACEX(WHICH_TRACEX, 10,"%s request:%d ready:%d\n",type,request.read(),ready.read());
   TRACEX(WHICH_TRACEX, 12," %s at Time %u\n",type,(uint)(sc_simulation_time()));	
  #endif
    
    if(request.read() == true)
    {   

// Riscaling address to SDRAM start address
      address = AddressBus.read() - DRAM_BASE;
// This address is in BYTE!
// Poletti Riscaling address from BYTES to WORD
      address = address>>2;
// From now address is in WORD: statemachine3 calculates all in WORD of 32 bits
// size contain the total size of programmed transfer      
      size = SizeBus.read();
// is_read contain the information about direction of transfer
// is_read = true L2->L1 transfer
// is_read = false L2->L1 transfer      
      is_read = RD.read();
      
      TRACEX(WHICH_TRACEX, 8,"%s IDLE_0 state in state machine\n",type);
      TRACEX(WHICH_TRACEX, 10,"%s request:%d ready:%d\n",type,request.read(),ready.read());
      TRACEX(WHICH_TRACEX, 10,"%s address:%d size:%d is_read:%d",type,address,size,is_read);
      TRACEX(WHICH_TRACEX, 12," %s at Time %u\n",type,(uint)(sc_simulation_time()));	
      
// number of command calculates the total number of command to use for this transfer
// to mantain a constant flow of data
      number_of_command = (int) ceil((double)((double)(SizeBus.read()) / ((double)DIMBURST)));
      alpha = (int) ceil((double)((double)(size) / ((double)DIMBURST)));
      if( ((DIMBURST == 2) && ((int)(size + (address & 0x00000001)) > alpha * DIMBURST) ) ||
	  ((DIMBURST == 4) && ((int)(size + (address & 0x00000003)) > alpha * DIMBURST) ) ||
	  ((DIMBURST == 8) && ((int)(size + (address & 0x00000007)) > alpha * DIMBURST  )) )
      {
        number_of_command = (int) ceil((double)((double)(size) / ((double)DIMBURST))) + 1;
      }
			
      number = 0;
			
// Initialize structure for read pipelining      
      if(is_read == true)
      {
	for(int i=0;i<TCAS_VECTOR_DIMENSION;i++)
	{
          tCAS_vector[i] = TCAS_SDRAM + 3;
	}	
				
	for(int i=0;i<TCAS_VECTOR_DIMENSION;i++)
	{
	  tCAS_vector_activation[i] = false;
	}
	
	for(int i=0;i<TCAS_VECTOR_DIMENSION;i++)
	{
	  burst_size_read[i] = 0;
	}
      }

      ready.write(false);

      start_burst = true;			
			
    }
    else
    {	
//There are no request: we can take another operation
      start_burst = false;
      ready.write(true);
    }

    if(start_burst == true)
    {
      next_state = DRAMDMA_ACTIVE_STATE;
      tRCD = TRCD_SDRAM;
      tRCD--;
      tRAS = TRAS_SDRAM;
      command.write(ACTIVE_COMMAND);
      
#ifdef DEBUG_PROFILE_DRAMDMA
  if(is_read == false)
  {
    printf("  START WRITE to SDRAM - MEMORY CONTROLLER SECTION:%d\n",(int)(sc_simulation_time()));
  }
  else
  {
    printf("  START READ from SDRAM - MEMORY CONTROLLER SECTION:%d\n",(int)(sc_simulation_time()));
  }
#endif      
       
      row = row_gen(address);
      AB.write(row);
      bank = bank_gen(address);
      BA.write(bank);
    }
    else
    {
      next_state = DRAMDMA_IDLE_0;
     command.write(NOP_COMMAND);	
    }

  break;

/////////////////////////////////////////////////////////////////////////////

  case DRAMDMA_ACTIVE_STATE:

//printf("ACTIVE state in state machine at Time %u\n",(uint)(sc_simulation_time()));
  TRACEX(WHICH_TRACEX, 8,"%s ACTIVE state in state machine\n",type);

  if(tRAS > 0) tRAS--;
  write_done = false;
  read_done = false;
  objects_in_fifo = index;
  empty_spaces = FIFO_LENGHT - objects_in_fifo;

  TRACEX(WHICH_TRACEX, 10," %s empty_spaces:%d, objects_in_fifo %d in ACTIVE state\n"
  	,type,empty_spaces,objects_in_fifo);	
  TRACEX(WHICH_TRACEX, 12," %s at Time %u\n",type,(uint)(sc_simulation_time()));	
  
  if(DIMBURST == 2) command_counter = DIMBURST - (address & 0x00000001);
  if(DIMBURST == 4) command_counter = DIMBURST - (address & 0x00000003);
  if(DIMBURST == 8) command_counter = DIMBURST - (address & 0x00000007);

  if(is_read == false) // L1->L2
  {
  
    if( ((DIMBURST == 2) && ((address & 0x00000001) !=0)) ||
        ((DIMBURST == 4) && ((address & 0x00000003) !=0)) ||
        ((DIMBURST == 8) && ((address & 0x00000007) !=0))  )
    {
// Here i have a 32 bit word-misaligned burst: i have to prevent wrap around of sdram
// so i have to terminate burst if its size is less than an sdram block or i have
// to send 2 commands in order to start second command exactly when i have to change block
// A block is a portion of SDRAM memory of size in words equal of the size of DIMBURST.
// If I send 2 commands, i have to check fifo size: i need the space necessary to the size
// of 2 commands, because i don't want to interrupt the command sequence.

      if( ((DIMBURST == 2) && (((address & 0x00000001) + size) <= DIMBURST)) ||
          ((DIMBURST == 4) && (((address & 0x00000003) + size) <= DIMBURST)) ||
          ((DIMBURST == 8) && (((address & 0x00000007) + size) <= DIMBURST)) )  
      {
// Here the size is less than block size, from start address to end: no wrap around is 
// possible here with this size, address and DIMBURST   
		
        if( (objects_in_fifo >= size) && tRCD == 0  )
        {
          next_state = DRAMDMA_WRITE_STATE;
          command.write(WRITE_COMMAND);
          write_done = true;
          cb = change_bank(address);
          column = column_gen(address);
          AB.write(column);
          bank = bank_gen(address);
          BA.write(bank);
          number_of_command--;
        }
        else
        {
          next_state = DRAMDMA_ACTIVE_STATE;
          command.write(NOP_COMMAND);
          if(tRCD > 0) tRCD--;
        }
      }
      else
      {
// Here the size is more than the block size: i have to check if i have enough room in fifo      
	if( ((DIMBURST == 2) && (objects_in_fifo >=((DIMBURST - (address & 0x00000001)) + DIMBURST))) ||
	    ((DIMBURST == 4) && (objects_in_fifo >=((DIMBURST - (address & 0x00000003)) + DIMBURST))) ||
    	    ((DIMBURST == 8) && (objects_in_fifo >=((DIMBURST - (address & 0x00000007)) + DIMBURST))) ||
	     (objects_in_fifo >= size))
	{
          if(tRCD == 0)
          {
	    next_state = DRAMDMA_WRITE_STATE;
	    command.write(WRITE_COMMAND);
	    write_done = true;
	    cb = change_bank(address);
	    column = column_gen(address);
	    AB.write(column);
	    bank_gen(address);
	    BA.write(bank);
	    number_of_command--;
	  }
	  else
	  {
	    next_state = DRAMDMA_ACTIVE_STATE;
            command.write(NOP_COMMAND);
	    if(tRCD > 0) tRCD--;
	  }
	}
	else
	{
	  next_state = DRAMDMA_ACTIVE_STATE;
	  command.write(NOP_COMMAND);
	  if(tRCD > 0) tRCD--;
	}
      }
    } //end of misaligned part
    else
    {
// Aligned part
      if( (objects_in_fifo >= DIMBURST || objects_in_fifo >= size) && tRCD == 0  ) 
      {
	next_state = DRAMDMA_WRITE_STATE;
	command.write(WRITE_COMMAND);
	write_done = true;
	cb = change_bank(address);
	column = column_gen(address);
	AB.write(column);
	bank = bank_gen(address);
	BA.write(bank);
	number_of_command--;
      }
      else
      {
	next_state = DRAMDMA_ACTIVE_STATE;
	command.write(NOP_COMMAND);
	if(tRCD > 0) tRCD--;
      }
    }
  } //end of write part
  else
  { // READ part

    if( ((DIMBURST == 2) && ((address & 0x00000001) !=0)) ||
        ((DIMBURST == 4) && ((address & 0x00000003) !=0)) ||
        ((DIMBURST == 8) && ((address & 0x00000007) !=0))  )
    {
// Misaligned part
      if( ((DIMBURST == 2) && (((address & 0x00000001) + size) <= DIMBURST)) ||
	  ((DIMBURST == 4) && (((address & 0x00000003) + size) <= DIMBURST)) ||
	  ((DIMBURST == 8) && (((address & 0x00000007) + size) <= DIMBURST)) )  
      {
        if( (empty_spaces >= size) && tRCD == 0  )
        {
          next_state = DRAMDMA_READ_STATE;
	  command.write(READ_COMMAND);
	  read_done = true;
	  cb = change_bank(address);
	  column = column_gen(address);
	  AB.write(column);
	  bank = bank_gen(address);
	  BA.write(bank);
	  tCAS_vector_activation[number] = true;
	  number_of_command--;
	  number++;
	  if(number == TCAS_VECTOR_DIMENSION) number = 0;
        }
        else
        {
          next_state = DRAMDMA_ACTIVE_STATE;
          command.write(NOP_COMMAND);
          if(tRCD > 0) tRCD--;
        }
      }
      else
      {
        if( ((DIMBURST == 2) && (empty_spaces >=((DIMBURST - (address & 0x00000001)) + DIMBURST))) ||
	    ((DIMBURST == 4) && (empty_spaces >=((DIMBURST - (address & 0x00000003)) + DIMBURST))) ||
    	    ((DIMBURST == 8) && (empty_spaces >=((DIMBURST - (address & 0x00000007)) + DIMBURST))) ||
	    (empty_spaces >= size) ) 
        {
          if(tRCD == 0)
	  {
	    next_state = DRAMDMA_READ_STATE;
	    command.write(READ_COMMAND);
	    read_done = true;
	    cb = change_bank(address);
	    column = column_gen(address);
	    AB.write(column);
	    bank = bank_gen(address);
	    BA.write(bank);
	    tCAS_vector_activation[number] = true;
	    number_of_command--;
	    number++;
	    if(number == TCAS_VECTOR_DIMENSION) number = 0;
	  }
	  else
	  {
	    next_state = DRAMDMA_ACTIVE_STATE;
	    command.write(NOP_COMMAND);
	    if(tRCD > 0) tRCD--;
	  }
        }
        else
        {	
	  next_state = DRAMDMA_ACTIVE_STATE;
	  command.write(NOP_COMMAND);
	  if(tRCD > 0) tRCD--;
        }
      }
    } // end of misaligned part
    else
    {
//Aligned part
      if( (empty_spaces >= DIMBURST || empty_spaces >= size) && tRCD == 0  ) //analogo alla write
      {
        next_state = DRAMDMA_READ_STATE;
        command.write(READ_COMMAND);
        read_done = true;
        cb = change_bank(address);
        column = column_gen(address);
        AB.write(column);
        bank = bank_gen(address);
        BA.write(bank);
        tCAS_vector_activation[number] = true;
        number_of_command--;
        number++;
        if(number == TCAS_VECTOR_DIMENSION)
        {
          number = 0;
        }
      }
      else
      {
        next_state = DRAMDMA_ACTIVE_STATE;
        command.write(NOP_COMMAND);
        if(tRCD > 0) tRCD--;
      }
    }// end of read
  }
  break;

///////////////////////////////////////////////////////////////////////////////

  case DRAMDMA_READ_STATE:
  
//printf("READ state in state machine at Time %u\n",(uint)(sc_simulation_time()));
 TRACEX(WHICH_TRACEX, 8,"%s READ state in state machine\n",type);

  objects_in_fifo = index;
  empty_spaces = FIFO_LENGHT - objects_in_fifo;

  TRACEX(WHICH_TRACEX, 10,"%s empty_spaces:%d, objects_in_fifo %d in READ state\n",type,
         empty_spaces,objects_in_fifo);	
  TRACEX(WHICH_TRACEX, 12," %s at Time %u\n",type,(uint)(sc_simulation_time()));	

  read_done = false;

  if(tRAS > 0) tRAS--;

  if(number_of_command > 0 && cb == false) 
  {
    if(command_counter > 0) command_counter--;
    
    total = 0;
	
    for(uint c=0;c<TCAS_VECTOR_DIMENSION;c++)
    {
      total = total + burst_size_read[c];
    }

    if(command_counter == 0 && empty_spaces - total >= DIMBURST)
    {
      next_state = DRAMDMA_READ_STATE;
      command.write(READ_COMMAND);
      read_done = true;
      column = column_gen(address);
      AB.write(column);
      bank = bank_gen(address);
      cb = change_bank(address);
      BA.write(bank);
      tCAS_vector_activation[number] = true;
      number_of_command--;
      number++;
      if(number == TCAS_VECTOR_DIMENSION) number = 0;
      command_counter = DIMBURST;
    }
    else
    {
      next_state = DRAMDMA_READ_STATE;
      command.write(NOP_COMMAND);
    }
  }
  else
  {
    if( (number - 1) < 0 )
    {
      number_dec = TCAS_VECTOR_DIMENSION - 1;
    }
    else
    {
      number_dec = number - 1;
    }
    
    total_pre = 0;
    
    for(int y=0;y<TCAS_VECTOR_DIMENSION;y++)
    {
      total_pre = total_pre + burst_size_read[y];
    }
    
    if(tRAS == 0 && (total_pre <= (TCAS_SDRAM )))
    {
      next_state = DRAMDMA_PRECHARGE;
      command.write(PRECHARGE_COMMAND);
      AB.write(0x0300);
      tRP = TRP_SDRAM;
    }
    else
    {
      next_state = DRAMDMA_READ_STATE;
      command.write(NOP_COMMAND);
    }
  }
  break;

///////////////////////////////////////////////////////////////////////////////

  case DRAMDMA_WRITE_STATE:

//printf("WRITE state in state machine at Time %u\n",(uint)(sc_simulation_time()));
 TRACEX(WHICH_TRACEX, 8,"%s WRITE state in state machine\n",type);
  objects_in_fifo = index;
  empty_spaces = FIFO_LENGHT - objects_in_fifo;

  TRACEX(WHICH_TRACEX, 10,"%s empty_spaces:%d, objects_in_fifo %d in WRITE state\n",type,
         empty_spaces,objects_in_fifo);	
  TRACEX(WHICH_TRACEX, 12," %s at Time %u\n",type,(uint)(sc_simulation_time()));	


  if(tRAS > 0) tRAS--;

  write_done = false;

  if(number_of_command > 0 && cb == false)
  {
    tWR = TWR_SDRAM; 

    if(command_counter > 0) command_counter--;

    if(command_counter == 0 && ( (objects_in_fifo >= DIMBURST) || (objects_in_fifo >= size) ) )
    {
      next_state = DRAMDMA_WRITE_STATE;
      command.write(WRITE_COMMAND);
      write_done = true;
      column = column_gen(address);
      AB.write(column);
      bank = bank_gen(address);
      cb = change_bank(address);
      BA.write(bank);
      number_of_command--;
      command_counter = DIMBURST;
    }
    else
    {
      next_state = DRAMDMA_WRITE_STATE;
      command.write(NOP_COMMAND);
    }
  }
  else
  {
    if(tWR > 0 && burst_size == 0) tWR--;
    if(tRAS == 0 && tWR == 0 && burst_size == 0)
    {
      next_state = DRAMDMA_PRECHARGE;
      command.write(PRECHARGE_COMMAND);
      AB.write(0x0300);
      tRP = TRP_SDRAM;
    }
    else
    {
      next_state = DRAMDMA_WRITE_STATE;
      command.write(NOP_COMMAND);
    }
  }

  break;

///////////////////////////////////////////////////////////////////////////////

  case DRAMDMA_PRECHARGE:

//printf("PRECHARGE state in state machine at Time %u\n",(uint)(sc_simulation_time()));
 TRACEX(WHICH_TRACEX, 8,"%s PRECHARGE state in state machine \n",type);
 TRACEX(WHICH_TRACEX, 12," %s at Time %u\n",type,(uint)(sc_simulation_time()));	
  if(tRP > 0) tRP--;

  if(tRP == 0)
  {
    if(cb == true && size != 0)
    {	    
      next_state = DRAMDMA_ACTIVE_STATE;
      command.write(ACTIVE_COMMAND);
      tRCD = TRCD_SDRAM;
      tRCD--;
      tRAS = TRAS_SDRAM;
      row = row_gen(address);
      AB.write(row);
      bank = bank_gen(address);
      BA.write(bank);
    }
    else
    {
      next_state = DRAMDMA_IDLE_0;
      
#ifdef DEBUG_PROFILE_DRAMDMA
  printf("    STOP of MEMORY CONTROLLER transfer on SDRAM:%d\n",(int)(sc_simulation_time()));
#endif      
      
      
      command.write(NOP_COMMAND);
    }
  }
  else
  {
    next_state = DRAMDMA_PRECHARGE;
    command.write(NOP_COMMAND);
  }

  break;

} // end switch

/////////////////////////////////////////////////////////////////////////////
// DATA FLOW PART
/////////////////////////////////////////////////////////////////////////////

if(is_read == false)
{ 
// WRITE PART

  if(write_done == true)
  {
//Burst size calculation
    if( ((DIMBURST == 2) && ((address & 0x00000001) !=0)) ||
  	((DIMBURST == 4) && ((address & 0x00000003) !=0)) ||
  	((DIMBURST == 8) && ((address & 0x00000007) !=0))  )
    { 
// Misaligned part
      if(DIMBURST == 2) burst_size = DIMBURST - (address & 0x00000001);
      if(DIMBURST == 4) burst_size = DIMBURST - (address & 0x00000003);
      if(DIMBURST == 8) burst_size = DIMBURST - (address & 0x00000007);
      if(size <= burst_size) burst_size = size;
    }
    else
    { 
// Aligned part
      if(size <= DIMBURST)
      {
        burst_size = size;
      }
      else
      {
        burst_size = DIMBURST;
      }
    }

//Size and address calculation

    address = address + burst_size;
    size = size - burst_size;

// printf(" WRITE Address:%d Size:%d Burst size:%d Time: %u \n",address, size, burst_size, (uint)(sc_simulation_time()));  
 TRACEX(WHICH_TRACEX, 10, " %s DATA ZONE WRITE Address:%d Size:%d Burst size:%d\n",type,address, size, burst_size);   
 TRACEX(WHICH_TRACEX, 12," %s at Time %u\n",type,(uint)(sc_simulation_time()));	
  }
  
// FIFO commands generation  
  
  if(burst_size > 0)
  {
    fpop_SDRAM = true;
    burst_size--;
  }
  else
  {
    fpop_SDRAM = false;
    if(tWR > 0 && burst_size == 0) tWR--;
  }
  DQM_signal.write(fpop_SDRAM);
}
else
{
// READ Part

// Operation on READ pipeline vector

  for(int i=0;i<TCAS_VECTOR_DIMENSION;i++)
  {
    if(tCAS_vector_activation[i]==true && (tCAS_vector[i] > 0)) tCAS_vector[i]--;
  }

  if( (number - 1) < 0 )
  {
    number_dec = TCAS_VECTOR_DIMENSION - 1;
  }
  else
  {
    number_dec = number - 1;
  }
	
  if(tCAS_vector_activation[number_dec] == true && read_done == true)
  {
    if( ((DIMBURST == 2) && ((address & 0x00000001) !=0)) ||
    	((DIMBURST == 4) && ((address & 0x00000003) !=0)) ||
    	((DIMBURST == 8) && ((address & 0x00000007) !=0))  )
    {
// Misaligned part
      if(DIMBURST == 2) burst_size_read[number_dec] = DIMBURST - (address & 0x00000001);
      if(DIMBURST == 4) burst_size_read[number_dec] = DIMBURST - (address & 0x00000003);
      if(DIMBURST == 8) burst_size_read[number_dec] = DIMBURST - (address & 0x00000007);
  
      if(size <= burst_size_read[number_dec]) burst_size_read[number_dec] = size;
    }
    else
    {
// Aligned part
      if(size <= DIMBURST)
      {
        burst_size_read[number_dec] = size;
      }
      else
      {
  	burst_size_read[number_dec] = DIMBURST;
      }
    }
    address = address + burst_size_read[number_dec];
    size = size - burst_size_read[number_dec];
  }

    j = number_dec;
    
// j is the current element processed    
if(size!= 0 || burst_size_read[j]!=0)
{
 TRACEX(WHICH_TRACEX, 10, "%s DATA ZONE READ Address:%d Size:%d Burst size:%d\n",type,address, size, burst_size_read[j]);   
 TRACEX(WHICH_TRACEX, 12," %s at Time %u\n",type,(uint)(sc_simulation_time()));	
}
    
    for(int k=0;k<TCAS_VECTOR_DIMENSION;k++)
    {
      if(tCAS_vector_activation[j] == true && tCAS_vector[j]==0)
      {
    	if(burst_size_read[j] > 0)
    	{
    	  fpush_SDRAM = true;
    	  burst_size_read[j]--;
    	  if(burst_size_read[j]==0) 
    	  {
    	    tCAS_vector_activation[j] = false;
    	    tCAS_vector[j] = TCAS_SDRAM + 3;
    	  }
    	  break_for = true;
    	}	
      }
      else
      {
    	fpush_SDRAM = false;
      	break_for =false;
      }
      j++;
      if(j==TCAS_VECTOR_DIMENSION) j = 0;

      if(break_for == true) break;
    }
  }


/////////////////////////////////////////////////////////////////////////////
// INTERNAL FIFO PART
/////////////////////////////////////////////////////////////////////////////

//printf("FIFO_ZONE:fcount:%d fpop_processor:%d fpush_processor:%d fpop_SDRAM:%d fpush_SDRAM:%d data_processor:%d"
//,fcount.read(),fpop_processor.read(),fpush_processor.read(),fpop_SDRAM,fpush_SDRAM,data_processor.read());
//printf("  Time %u\n",(uint)(sc_simulation_time()));
#if 0
TRACEX(WHICH_TRACEX, 10,"%s FIFO_ZONE:fcount:%d fpop_processor:%d fpush_processor:%d fpop_SDRAM:%d fpush_SDRAM:%d",
type,fcount.read(),fpop_processor.read(),fpush_processor.read(),fpop_SDRAM,fpush_SDRAM);
TRACEX(WHICH_TRACEX, 12," %s Time %u\n",type,(uint)(sc_simulation_time()));
#endif

  data_out_proc.write(false);

  if(fpop_processor.read())
  {
    data_dim = data_dimension.read();
    if(index < data_dim)
    {
      printf("FIFO empty!!\n");
      exit(1);
    }

    for(uint m=0;m<data_dim;m++)
    {
      data_processor[m].write(fifomemory[m]);
      //printf("  FIFO_PROCESSOR_OUT:%d \n",fifomemory[m]);	      
      //printf("  Time %u\n",(uint)(sc_simulation_time()));
      TRACEX(WHICH_TRACEX, 10," %s FIFO_PROCESSOR_OUT:%d \n",type,fifomemory[m]);
      TRACEX(WHICH_TRACEX, 12," %s  Time %u\n",type,(uint)(sc_simulation_time()));
    }
    
    data_out_proc.write(true);

    for(uint m=0;m<index-1;m++)
    {
      fifomemory[m]=fifomemory[m+data_dim];
    }
    		    
    index = index - data_dim;
  }

  if(fpop_SDRAM == true)
  {

    if(index == 0)
    {
      printf("FIFO empty!!\n");
      exit(1);
    }
	
      data_SDRAM.write(fifomemory[0]);
      //printf("  FIFO_SDRAM_OUT:%d \n",fifomemory[0]); 			      
      //printf("  Time %u\n",(uint)(sc_simulation_time()));
      TRACEX(WHICH_TRACEX, 10," %s FIFO_SDRAM_OUT:%d \n",type,fifomemory[0]); 			      
      TRACEX(WHICH_TRACEX, 12," %s Time %u\n",type,(uint)(sc_simulation_time()));
      for(uint m=0;m<index-1;m++)
      {
        fifomemory[m]=fifomemory[m+1];
      }
        	      
      index--;
  }

  if(fpush_processor.read())
  {
    data_dim = data_dimension.read();
    if(index + data_dim > FIFO_LENGHT)
    {
      printf("FIFO full!!\n");
      exit(1);
    }

    for(uint m=0;m<data_dim;m++)
    {
      fifomemory[index + m]=data_processor[m].read();
      //printf("DATA %d in FIFO from processor:%d\n",m,data_processor[m].read());
      //printf("  Time %u\n",(uint)(sc_simulation_time()));
      TRACEX(WHICH_TRACEX, 10,"%s DATA %d in FIFO from processor:%d\n",type,m,data_processor[m].read());
      TRACEX(WHICH_TRACEX, 12," %s Time %u\n",type,(uint)(sc_simulation_time()));
    }

    index=index+data_dim;
  }

  if(fpush_SDRAM == true)
  {
    if(index == FIFO_LENGHT)
    {
      printf("FIFO full!!\n");
      exit(1);
    }
    fifomemory[index]=data_SDRAM.read();
//printf("DATA in FIFO from SDRAMprocessor:%d\n",data_SDRAM.read());
//printf("  Time %u\n",(uint)(sc_simulation_time()));
    TRACEX(WHICH_TRACEX, 10,"%s DATA in FIFO from SDRAMprocessor:%d\n",type,data_SDRAM.read());
    TRACEX(WHICH_TRACEX, 12," %s Time %u\n",type,(uint)(sc_simulation_time()));
    index++;
  }

  fcount.write(index);

// FIFO tracing

//printf("FIFO:|");
TRACEX(WHICH_TRACEX, 14,"%s FIFO:|",type);
for(int k=0;k < FIFO_LENGHT;k++)
{
	//printf("%d|",fifomemory[k]);
	TRACEX(WHICH_TRACEX, 14,"%d|",fifomemory[k]);
}
//printf("elements:%d Time:%u|\n",fcount.read(),(uint)(sc_simulation_time()));
TRACEX(WHICH_TRACEX, 14,"elements:%d Time:%u|\n",fcount.read(),(uint)(sc_simulation_time()));


}// end else reset FSM

}//end method



///////////////////////////////////////////////////////////////////////////////
// COLUMN SDRAM ADDRESS GENERATION - COMBINATORIAL
///////////////////////////////////////////////////////////////////////////////

int statemachine3::column_gen(int address)
{
  int column_value=0;

  if(SDRAM_SIZE == 64 || SDRAM_SIZE == 128)
  {
    column_value = address & 0x000000FF;	
  }

  if(SDRAM_SIZE == 256 || SDRAM_SIZE == 512)
  {
    column_value = address & 0x000001FF;	
  }
  
  return column_value;
}



///////////////////////////////////////////////////////////////////////////////
// ROW SDRAM ADDRESS GENERATION - COMBINATORIAL
///////////////////////////////////////////////////////////////////////////////

int statemachine3::row_gen(int address)
{
  int row_value=0;

  if(SDRAM_SIZE == 64 || SDRAM_SIZE == 128)
  {
    row_value = (address & 0xFFFFFC00) >> 10;
  }

  if(SDRAM_SIZE == 256 || SDRAM_SIZE == 512)
  {
    row_value = (address & 0xFFFFF800) >> 11;
  }

  return row_value;
}



///////////////////////////////////////////////////////////////////////////////
// BANK SDRAM ADDRESS GENERATION - COMBINATORIAL
///////////////////////////////////////////////////////////////////////////////

int statemachine3::bank_gen(int address)
{
  int bank_value=0;
  
  if(SDRAM_SIZE == 64 || SDRAM_SIZE == 128)
  {
    bank_value = (address & 0x00000300) >> 8;
  }

  if(SDRAM_SIZE == 256 || SDRAM_SIZE == 512)
  {
    bank_value = (address & 0x00000700) >> 9;
  }

  return bank_value;
}



///////////////////////////////////////////////////////////////////////////////
// CHANGE BANK CALCULATION  - COMBINATORIAL
///////////////////////////////////////////////////////////////////////////////

bool statemachine3::change_bank(int address)
{

  int finaladdress;
  int bank_old;
  int bank_new;

  if(DIMBURST == 2) finaladdress = address + (DIMBURST - (address & 0x00000001));
  if(DIMBURST == 4) finaladdress = address + (DIMBURST - (address & 0x00000003));
  if(DIMBURST == 8) finaladdress = address + (DIMBURST - (address & 0x00000007));

  bank_old = bank_gen(address);
  bank_new = bank_gen(finaladdress);

  if(bank_old == bank_new)
  {
    return false;
  }
  else
  {
    return true;
  }

}




