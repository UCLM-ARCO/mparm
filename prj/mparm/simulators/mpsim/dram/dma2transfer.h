#ifndef __DMA2TRANSFER_H__
#define __DMA2TRANSFER_H__

#define BUS_MIN_DIM_BURST 1
#define BUS_MAX_DIM_BURST 8

#include <systemc.h>
#include "globals.h"
#include "core_signal.h"
#include "debug.h"
#include "ast_iss_if.h"
#include "statemachine3.h"



SC_MODULE(dma2transfer)
{
  sc_in_clk clock;
  
  sc_in<bool> reset_transfer;
  
  //port to the control

  sc_in<DMA_CONT_REG1> datadma;		
  sc_in<bool> requestdmatransfer;		
  sc_out<bool> finished;				
  sc_out<uint> finished_transfer_id; 
  sc_out<uint> finished_transfer_proc;
  sc_out<bool> transfer_ack;

  //port to memory controller
  
  sc_in<bool> ready;
  sc_out<bool> request;
  sc_out<uint> AddressBus;
  sc_out<uint> SizeBus;
  sc_out<bool> RD;
  
  //port to the FIFO
  
  sc_out<uint> data_dimension;

  sc_inout<uint> data_processor[BUS_MAX_BURST_LENGHT];
  sc_out<bool> fpop_processor;		
  sc_out<bool> fpush_processor;
  sc_in<bool> data_out_proc;
  sc_in<uint> fcount;

  sc_port<ASTISSIf> stbusport;    // stbus master I/O port

 	unsigned int set_burst(unsigned int address)
	{
	 unsigned int burst=0;
	 
	  switch((address>>2) & 0x0000000F)
	  {
	    case 0x0:
	    burst = 16;
	    break;

	    case 0x1:
	    burst = 1;
	    break;

	    case 0x2:
	    burst = 2;
	    break;

	    case 0x3:
	    burst = 1;
	    break;
	
	    case 0x4:
	    burst = 4;
	    break;

	    case 0x5:
	    burst = 1;
	    break;

	    case 0x6:
	    burst = 2;
	    break;

	    case 0x7:
	    burst = 1;
	    break;
	    
	    case 0x8:
	    burst = 8;
	    break;

	    case 0x9:
	    burst = 1;
	    break;

	    case 0xA:
	    burst = 2;
	    break;

	    case 0xB:
	    burst = 1;
	    break;
	
	    case 0xC:
	    burst = 4;
	    break;

	    case 0xD:
	    burst = 1;
	    break;

	    case 0xE:
	    burst = 2;
	    break;
	
	    case 0xF:
	    burst = 1;
	    break;
	    
	    /*
	    case 0x10:
	    burst = 16;
	    break;

	    case 0x11:
	    burst = 1;
	    break;

	    case 0x12:
	    burst = 2;
	    break;

	    case 0x13:
	    burst = 1;
	    break;
	
	    case 0x14:
	    burst = 4;
	    break;

	    case 0x15:
	    burst = 1;
	    break;

	    case 0x16:
	    burst = 2;
	    break;
	
	    case 0x17:
	    burst = 1;
	    break;
	    
	    case 0x18:
	    burst = 8;
	    break;

	    case 0x19:
	    burst = 1;
	    break;

	    case 0x1A:
	    burst = 2;
	    break;

	    case 0x1B:
	    burst = 1;
	    break;
	
	    case 0x1C:
	    burst = 4;
	    break;

	    case 0x1D:
	    burst = 1;
	    break;

	    case 0x1E:
	    burst = 2;
	    break;
	
	    case 0x1F:
	    burst = 1;
	    break;
	    */
	    
	  }
	 return burst;
	 };

 SC_CTOR(dma2transfer)
 {
   buffer = (UInt32*) malloc (sizeof(UInt32) * BUS_MAX_DIM_BURST );
   SC_METHOD(transfer_work);
   sensitive_pos << clock;
   read_state = 0;
   write_state = 0;
   
   WHICH_TRACEX=DRAMDMATRANSFER_TRACEX;
    
   type="DMA2_TRANSFER";

   
 }


 protected:
 
 DMA_CONT_REG1 work;
 int st_measure_time;
 bool prog_accepted;
 bool lmi_programmed;
 bool buffer_ok;
 bool read_done,write_done;
 bool data_out;
 bool end_write;
 uint i,k;
 uint read_state,write_state;
 UInt32* buffer;
 sc_uint<32> address;
 uint burst,prev_burst;
 sc_uint<32> actual_size;
 
 char* type;
 uint32_t WHICH_TRACEX;
 	  

 void transfer_work();
 bool read(uint address ,UInt32* buffer,uint burst, uint nproc);
 bool write(uint address ,UInt32* buffer,uint burst, uint nproc); 

};



#endif //__DMA2TRANSFER_H__
