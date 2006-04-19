#ifndef __STATEMACHINE3_H__
#define __STATEMACHINE3_H__

#include "systemc.h"
#include "sdram_car.h"
#include "globals.h"

#include "debug.h"

#define FIFO_LENGHT 4096
#define TCAS_VECTOR_DIMENSION 16
//FIXME Right now 16 is the maximum working size
#define BUS_MAX_BURST_LENGHT 16


#define NOP_COMMAND 0
#define ACTIVE_COMMAND 1
#define READ_COMMAND 2
#define WRITE_COMMAND 3
#define PRECHARGE_COMMAND 4

//State

#define DRAMDMA_RESET_0 0
#define DRAMDMA_IDLE_0 1
#define DRAMDMA_ACTIVE_STATE 2
#define DRAMDMA_READ_STATE 5
#define DRAMDMA_WRITE_STATE 7
#define DRAMDMA_PRECHARGE 9

//You can enable this macro if you want to profile the performance of this device
//#define DEBUG_PROFILE_DRAMDMA

SC_MODULE(statemachine3)
{
  sc_in_clk clock;
  sc_in<bool> reset_FSM;
	
  sc_out<bool> ready;
  sc_in<bool> request;

// Transfer Engine Side
  sc_inout<uint> data_processor[BUS_MAX_BURST_LENGHT];
  sc_in<bool> fpop_processor;		  
  sc_in<bool> fpush_processor;
  sc_out<bool> data_out_proc;
  sc_in<uint> data_dimension;
  sc_in<uint> AddressBus;
  sc_in<uint> SizeBus;
  sc_in<bool> RD;
  sc_out<uint> fcount;
	
// SDRAM side

  sc_out<uint> AB;
  sc_out<uint> BA;
  sc_out<uint> command;
  sc_out<bool> DQM_signal;
  sc_inout<uint> data_SDRAM;

 SC_CTOR(statemachine3)
 {
   fifomemory = (uint *) calloc(FIFO_LENGHT,sizeof(uint));
   tCAS_vector = (int *) calloc(TCAS_VECTOR_DIMENSION,sizeof(int));
   tCAS_vector_activation = (int *) calloc(TCAS_VECTOR_DIMENSION,sizeof(int));
   burst_size_read = (int *) calloc(TCAS_VECTOR_DIMENSION,sizeof(int));
   SC_METHOD(FSM_logic);
   sensitive_pos << clock;
 
   WHICH_TRACEX=DRAMDMASTATEMACHINE_TRACEX;
    
   type="STATEMACHINE";

   state = 0;
 };

protected:

int address;
int size;
int burst_size;
int state, next_state;
bool write_done,read_done;
bool is_read;
int number_of_command,command_counter;
int alpha;
int number;
bool start_burst;
int tRCD,tRAS,tWR,tRP;
int row,bank,column;
int objects_in_fifo,empty_spaces;
bool cb;
bool fpop_SDRAM,fpush_SDRAM;
uint index;
int j;
bool break_for;

int number_dec;

int total;
int total_pre;
uint data_dim;

uint* fifomemory;
int* tCAS_vector;
int* tCAS_vector_activation;
int* burst_size_read;

 char* type;
 uint32_t WHICH_TRACEX;

// Functions

void FSM_logic();
int column_gen(int address);
int row_gen(int address);
int bank_gen(int address);
bool change_bank(int address);


};



#endif // __STATEMACHINE3_H__
