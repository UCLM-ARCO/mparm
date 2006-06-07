#include "sdram.h"

// WARNING: address variable is always in 32 bits words!
// I have to multiply them by 4 when i use Read and Write methods
// of mem_class
// DRAM_SIZE is in byte 
// TRACEX levels:
// 8: prints command read from Memory Controller
// 10: prints basic variables
// 12: prints times

void sdram::sdram_logic()
{
  if(reset_SDRAM.read() == true)
  {
    active_to_command_done = false;
    active_to_precharge_done = false;
    precharge_to_active = false;
    read_done = false;
    data_to_read = 0;
    data_to_write = 0;
    truncate_read = DIMBURST;
    first_read = true;
    trp_value = 0;
    read_on = false;
    write_on = false;
    j = 0;

    for(int z=0;z < VECTOR_SIZE;z++)
    {
      s[z] = false;
    }
  }
  else
  {

// printf("SDRAM tRCD value:%d  TIME:%u: command:%d:\n",trcd_value,
// (uint)(sc_simulation_time()),command.read());
  TRACEX(WHICH_TRACEX, 10,"command:%d:\n",command.read());
  TRACEX(WHICH_TRACEX, 12," at Time %u\n",(uint)(sc_simulation_time()));

switch(command.read())
{
// SDRAM main module: command switch
/////////////////////////////////////////////////////////////////////////////

  case NOP_COMMAND:
  
//printf("NOP Time %u\n",(uint)(sc_simulation_time()));
TRACEX(WHICH_TRACEX, 8,"NOP COMMAND\n");
TRACEX(WHICH_TRACEX, 12," at Time %u\n",(uint)(sc_simulation_time()));

    if(active_to_command_done == true) trcd_value--;
    if(active_to_precharge_done == true && tras_value>0) tras_value--;
    if(precharge_to_active == true) trp_value--;
    if(read_done == true && tcas_value > 0) tcas_value--;
    if(read_done == true && truncate_read > 0) truncate_read--;

  break;

/////////////////////////////////////////////////////////////////////////////
  
  case ACTIVE_COMMAND:
  
//printf("ACTIVE Time %u\n",(uint)(sc_simulation_time()));
TRACEX(WHICH_TRACEX, 8,"ACTIVE COMMAND\n");
TRACEX(WHICH_TRACEX, 12," at Time %u\n",(uint)(sc_simulation_time()));


    trcd_value = TRCD_SDRAM;
    trcd_value--;
    tras_value = TRAS_SDRAM;
    tras_value--;
    active_to_command_done = true;
    active_to_precharge_done = true;
    row = AB.read();
    bank = BA.read();

    if(data_to_read == 0) 
    {
      read_on = false;
    }
    else
    {
      read_on = true; 
    }

    if(data_to_write == 0) 
    {
      write_on = false;
    }
    else
    {
      write_on = true; 
    }

    if(trp_value > 0)
    {
      printf("tRP time fail,trp_value=%d\n",trp_value);
      exit(1);
    }

    precharge_to_active = false;

TRACEX(WHICH_TRACEX, 10,"row:%d bank:%d read_on:%d write_on:%d\n",
row,bank,read_on,write_on);
TRACEX(WHICH_TRACEX, 12," at Time %u\n",(uint)(sc_simulation_time()));

    break;

/////////////////////////////////////////////////////////////////////////////

  case READ_COMMAND:

//printf(" READ COMMAND at TIME:%u \n",(uint)(sc_simulation_time()));
TRACEX(WHICH_TRACEX, 8,"READ COMMAND\n");
TRACEX(WHICH_TRACEX, 12," at Time %u\n",(uint)(sc_simulation_time()));


  read_on = true;
  if(active_to_precharge_done == true && tras_value>0) tras_value--;
  if(trcd_value > 0)
  {
    printf("tRCD time fail:%d\n",trcd_value);
    exit(1);
  }
  active_to_command_done = false;
  column = AB.read();

  if(SDRAM_SIZE == 64 || SDRAM_SIZE == 128)
  {
// 8 column bits,2 bank bits and other are row bits:
// I use a mask to extract values
    address_read = column + (bank << 8) + (row << 10);
  }

  if(SDRAM_SIZE == 256 || SDRAM_SIZE == 512)
  {
// 9 column bits,2 bank bits and other are row bits:
// I use a mask to extract values
    address_read = column + (bank << 9) + (row << 11);
  }
//printf("ADDRESS:0x%x:\n",address_read);
TRACEX(WHICH_TRACEX, 10,"ADDRESS in READ:0x%x:\n",address_read);


  if(DIMBURST == 2)
  {
    if( (j + TCAS_SDRAM) < VECTOR_SIZE )
    {
      s[j + TCAS_SDRAM] = true;
    }
    else
    {
      s[j + TCAS_SDRAM - VECTOR_SIZE] = true;
    }
	
    if( (j + TCAS_SDRAM + 1) < VECTOR_SIZE )
    {
      s[j + TCAS_SDRAM + 1] = true;
    }
    else
    {
      s[j + TCAS_SDRAM + 1 - VECTOR_SIZE] = true;
    }
  }

  if(DIMBURST == 4)
  {
    if( (j + TCAS_SDRAM) < VECTOR_SIZE )
    {
      s[j + TCAS_SDRAM] = true;
    }
    else
    {
      s[j + TCAS_SDRAM - VECTOR_SIZE] = true;
    }
	
    if( (j + TCAS_SDRAM + 1) < VECTOR_SIZE )
    {
      s[j + TCAS_SDRAM + 1] = true;
    }
    else
    {
      s[j + TCAS_SDRAM + 1 - VECTOR_SIZE] = true;
    }	
	
    if( (j + TCAS_SDRAM + 2) < VECTOR_SIZE )
    {
      s[j + TCAS_SDRAM + 2] = true;
    }
    else
    {
      s[j + TCAS_SDRAM + 2 - VECTOR_SIZE] = true;
    }
	
    if( (j + TCAS_SDRAM + 3) < VECTOR_SIZE )
    {
      s[j + TCAS_SDRAM + 3] = true;
    }
    else
    {
      s[j + TCAS_SDRAM + 3 - VECTOR_SIZE] = true;
    }
  }

  if(DIMBURST == 8)
  {
    if( (j + TCAS_SDRAM) < VECTOR_SIZE )
    {
      s[j + TCAS_SDRAM] = true;
    }
    else
    {
      s[j + TCAS_SDRAM - VECTOR_SIZE] = true;
    }
	
    if( (j + TCAS_SDRAM + 1) < VECTOR_SIZE )
    {
      s[j + TCAS_SDRAM + 1] = true;
    }
    else
    {
      s[j + TCAS_SDRAM + 1 - VECTOR_SIZE] = true;
    }	
	
    if( (j + TCAS_SDRAM + 2) < VECTOR_SIZE )
    {
      s[j + TCAS_SDRAM + 2] = true;
    }
    else
    {
      s[j + TCAS_SDRAM + 2 - VECTOR_SIZE] = true;
    }
	
    if( (j + TCAS_SDRAM + 3) < VECTOR_SIZE )
    {
      s[j + TCAS_SDRAM + 3] = true;
    }
    else
    {
      s[j + TCAS_SDRAM + 3 - VECTOR_SIZE] = true;
    }
	
    if( (j + TCAS_SDRAM + 4) < VECTOR_SIZE )
    {
      s[j + TCAS_SDRAM + 4] = true;
    }
    else
    {
      s[j + TCAS_SDRAM + 4 - VECTOR_SIZE] = true;
    }
	
    if( (j + TCAS_SDRAM + 5) < VECTOR_SIZE )
    {
      s[j + TCAS_SDRAM + 5] = true;
    }
    else
    {
      s[j + TCAS_SDRAM + 5 - VECTOR_SIZE] = true;
    }	
	
    if( (j + TCAS_SDRAM + 6) < VECTOR_SIZE )
    {
      s[j + TCAS_SDRAM + 6] = true;
    }
    else
    {
      s[j + TCAS_SDRAM + 6 - VECTOR_SIZE] = true;
    }
	
    if( (j + TCAS_SDRAM + 7) < VECTOR_SIZE )
    {
      s[j + TCAS_SDRAM + 7] = true;
    }
    else
    {
      s[j + TCAS_SDRAM + 7 - VECTOR_SIZE] = true;
    }
  }


  if(first_read == true)
  {
    tcas_value = TCAS_SDRAM;
    address = address_read;
    if(address > DRAM_SIZE/4)
    {
      printf("Address out of size!!:%x DRAM_SIZE:%x \n",address,DRAM_SIZE);
      exit(1);
    }
  }
  else
  {	
    if(tcas_value > 0) tcas_value--;
  }
  read_done = true;

  if(truncate_read == 0 || first_read == true)
  {
    data_to_read = data_to_read + DIMBURST;
  }
  else
  {
    data_to_read = data_to_read + DIMBURST - truncate_read;
  }

  if(truncate_read > 0) truncate_read--;
  first_read = false;

  TRACEX(WHICH_TRACEX, 10,"data to read:0x%x:\n",data_to_read);

  break;

/////////////////////////////////////////////////////////////////////////////

  case WRITE_COMMAND:

  write_on = true;

//printf("WRITE:Time %u\n",(uint)(sc_simulation_time()));
TRACEX(WHICH_TRACEX, 8,"WRITE COMMAND\n");
TRACEX(WHICH_TRACEX, 12," at Time %u\n",(uint)(sc_simulation_time()));


  if(active_to_precharge_done == true && tras_value>0) tras_value--;
  if(trcd_value > 0)
  {
    printf("tRCD time fail, tRCD_value:%d\n",trcd_value);
    exit(1);
  }
  active_to_command_done = false;
  if(active_to_precharge_done == true && tras_value > 0) tras_value--;
  column = AB.read();

  if(SDRAM_SIZE == 64 || SDRAM_SIZE == 128)
  {
// 8 column bits,2 bank bits and other are row bits:
// I use a mask to extract values
    address_write = column + (bank << 8) + (row << 10);
  }

  if(SDRAM_SIZE == 256 || SDRAM_SIZE == 512)
  {
// 9 column bits,2 bank bits and other are row bits:
// I use a mask to extract values
    address_write = column + (bank << 9) + (row << 11);
  }
//printf("ADDRESS:0x%x:\n",address_write);
TRACEX(WHICH_TRACEX, 10,"ADDRESS in WRITE:0x%x:\n",address_write);

  address = address_write;

  if(address > DRAM_SIZE/4)
  {
    printf("Address out of size!!");
    exit(1);
  }

  data_to_write = DIMBURST;

  break;

/////////////////////////////////////////////////////////////////////////////

  case PRECHARGE_COMMAND:

TRACEX(WHICH_TRACEX, 8,"PRECHARGE COMMAND\n");
TRACEX(WHICH_TRACEX, 12," at Time %u\n",(uint)(sc_simulation_time()));


  if(tras_value > 0)
  {
    printf("tRAS time fail,tras_value:%d\n",tras_value);
    exit(1);
  }

  active_to_precharge_done = false;
  trp_value = TRP_SDRAM;
  trp_value--;
  precharge_to_active = true;

  if(read_done == true && tcas_value > 0) tcas_value--;

  first_read = true;

  break;

}//end switch

/////////////////////////////////////////////////////////////////////////////

if(read_on == true)
{
// READ case
// Check if CAS time has passed, I have data to read and
// it's the exact time to read
  if(tcas_value <= 0 && data_to_read > 0 && s[j] == true) 
  {
    s[j] = false;
    data = sdram_memory->Read((address * 4), 0);
    DATABUS.write(data);
//printf(" DATA OUT SDRAM:%d from address 0x%x at TIME:%u \n",
//data,address,(uint)(sc_simulation_time()));
TRACEX(WHICH_TRACEX, 10," DATA OUT SDRAM:%d from address 0x%x\n",data,address);
TRACEX(WHICH_TRACEX, 12," at TIME:%u\n",(uint)(sc_simulation_time()));

    data_to_read--;
    if(data_to_read == 0) first_read = true;
    address++;
    if(address > DRAM_SIZE/4)
    {
      printf("Address out of size!!");
      exit(1);
    }
  }
  else
  {
// Used to clean the activation vector
    if(tcas_value > 0 && s[j] == true) s[j] = false; 
  }
}
else
{
}

if(write_on == true)
{
// WRITE case
  if(data_to_write > 0 && DQM_signal.read() == true)
  {
    data = DATABUS.read();
    sdram_memory->Write((address * 4),data, 0);
//printf(" DATA IN SDRAM:%d at address 0x%x TIME:%u\n",
//data, address,(uint)(sc_simulation_time()));
TRACEX(WHICH_TRACEX, 10,"DATA IN SDRAM:%d at address 0x%x\n",data,address);
TRACEX(WHICH_TRACEX, 12," at TIME:%u\n",(uint)(sc_simulation_time()));

    data_to_write--;
    address++;
    
    if(address > DRAM_SIZE/4)
    {
      printf("Address out of size!!");
      exit(1);
    }
  }
  else
  {
  }
}
else
{
}
// Circular vector increment and restart
j++; 
if(j == VECTOR_SIZE) j = 0;

}//end else reset

}//end method
