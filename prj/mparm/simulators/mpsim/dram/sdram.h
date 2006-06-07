#ifndef __SDRAM_H__
#define __SDRAM_H__

#include "systemc.h"
#include "sdram_car.h"
#include "mem_class.h"

// Defining SDRAM command
#define NOP_COMMAND 0
#define ACTIVE_COMMAND 1
#define READ_COMMAND 2
#define WRITE_COMMAND 3
#define PRECHARGE_COMMAND 4

// Defining the size of CAS vector to pipelining commands
#define VECTOR_SIZE 16

// This implement a "normal" external memory of a core
class Sdram_mem :public Mem_class
{
 public:
  Sdram_mem(uint16_t id,uint32_t size) : 
    Mem_class(id,size)
  {
   type="Offchip_Sdram"; 
   //addresser->pMemoryDebug[ID]=myMemory;
  
   //Poletti needed for LEAD
   if(load_program("TargetMem.bin")) 
   exit(1);
  }
 
};

SC_MODULE(sdram)
{
	sc_in_clk clock;
	sc_in<bool> reset_SDRAM;
	sc_in<uint> command;
	sc_in<uint> AB;
	sc_in<uint> BA;
	sc_inout<uint> DATABUS;
	sc_in<bool> DQM_signal;
	
public:

int row,bank,column,address_read,address,address_write;

int trcd_value;
int tcas_value;
int tras_value;
int trp_value;

int data_to_read; // Number of words to read
int data_to_write; // Number of words to write
int truncate_read; // To understand if a read needs to be truncated

int j;

uint data;

bool active_to_command_done, active_to_precharge_done, precharge_to_active, read_done;
bool first_read;
bool write_on,read_on;

 char* type;
 uint32_t WHICH_TRACEX;

Sdram_mem *sdram_memory;
bool *s; // activation vector
void sdram_logic();

SC_CTOR(sdram)
{        
         //It can be 8MB 16MB 32MB 64MB 
         if(DRAM_SIZE!=0x00800000 && DRAM_SIZE!=0x01000000 && DRAM_SIZE!=0x02000000 && DRAM_SIZE!=0x04000000)
         {
	  printf("Wrong DRAM_SIZE:%x It can be 8MB 16MB 32MB 64MB\n",DRAM_SIZE);
	  exit(1);
	 }
	 sdram_memory = new Sdram_mem(0,DRAM_SIZE);
	 s = (bool *) malloc(sizeof(bool)*VECTOR_SIZE);
 	 SC_METHOD(sdram_logic);
	 sensitive_pos << clock ;
	 
         WHICH_TRACEX=SDRAM_TRACEX;
         type="SDRAM";

	 
};
 


};


#endif // __SDRAM_H__

