///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         dmacontrol.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Implements a DMA controller (processor side)
//
///////////////////////////////////////////////////////////////////////////////


#ifndef __DMA2CONTROL_H__
#define __DMA2CONTROL_H__


#include <systemc.h>
#include "globals.h"
#include "core_signal.h"
#include "debug.h"
#include "address.h"

#include "statemachine3.h" //to include the plot string

SC_MODULE(dma2control)
{
  sc_in_clk clock;
  
  //port to the wrapper
  sc_inout<PINOUT> dmapin;
  sc_out<bool> readydma;
  sc_in<bool> requestdma;
  
  //port to the dma transfer
  sc_out<DMA_CONT_REG1> datadma;		
  sc_out<bool> requestdmatransfer;		
  sc_in<bool> finished;				
  sc_in<uint> finished_transfer_id; 
  sc_in<uint> finished_transfer_proc;

  sc_in<bool> transfer_ack;

  SC_HAS_PROCESS(dma2control);
    
    dma2control(sc_module_name nm, uint32_t start, uint32_t obj, uint32_t nproc, uint16_t id) :
    sc_module(nm)
  {
    WHICH_TRACEX=DRAMDMACONTROL_TRACEX;
    
    type="DMA2_CONTROL";
    
    ID=id;

  
    //number of possible processor that can program the dma
    if(!(0<nproc<=N_CORES)) 
    {
      printf("DMA_CONTROL Wrong number of processor:%d\n, variable N_CORES = %d\n",nproc,N_CORES);
      exit(1);
    }
    numproc= nproc;
    maxobj = obj;				
    size = ((maxobj)*4*7+4)*numproc;
    start_address = start;
    
    //Now the queue of the work to do has the same dimension of the maximun object that you
    //can add to the dma
    
    // reg is a matrix of DMA_CONT_REG1
    
    reg=(DMA_CONT_REG1**)malloc(numproc*sizeof(DMA_CONT_REG1*));
    for (uint r=0;r<numproc;r++)
    {
      reg[r]=(DMA_CONT_REG1*)malloc(maxobj*sizeof(DMA_CONT_REG1));
    }
    
    counter=(uint32_t*)malloc(numproc*sizeof(uint32_t));
    
    //free is a table that indicates which work localized by couple processor - object is free
    free=(bool**)malloc(numproc*sizeof(bool*));
    for (uint j=0;j<numproc;j++)
    {
      free[j]=(bool*)malloc(sizeof(bool)*maxobj);
      for (uint i=0;i<maxobj;i++)
        free[j][i]=true;
      counter[j]=0;
    }
    
    
    //committ table initialization committ table knows whick work are sent to transfer engine
    
    committ=(bool**)malloc(numproc*sizeof(bool*));
    for (uint c=0;c<numproc;c++)
    {
      committ[c]=(bool*)malloc(sizeof(bool)*maxobj);
      for (uint g=0;g<maxobj;g++)
        committ[c][g]=false;
    }  
    
    nwork=0;
    freepos=0;
    working=0;
    numberofchange=0;
    request=false;
    changestate=false;
    endtransfer=false;
    
    transfer_id=0;
    processor_id=0;
    
    dma_transfer_ack=false;
    
    done=false;
  
    prog_done=false;
    
    read_done=false;
    
    first_prog=false;
		
TRACEX(WHICH_TRACEX, 7,"%s start:%x size:%x \n",type,start_address,size);
    
    SC_CTHREAD(simuldma2control, clock.pos());
  }

  protected:

  DMA_CONT_REG1** reg;
  
  bool dma_transfer_ack;
  
  bool** free;   //free positions in the reg table : one for cores
  
  //committ table 
  // committ = true means that the object is currently in work 
  // committ = false means that the object work is ended or work has to be sent depending from object state 
  bool** committ;  
  
  uint32_t* counter;  //number of free positions in the reg array : one for cores
  uint32_t nwork,    //number of works to do in the queue
  working,freepos,  //on going transfer, first work to do in the queue
  size, //size of the dma related to the dimension the maxobj parameter
  start_address;     //start address of the dmaslave
  uint32_t* work; //queue of the works to do 
  
  
  PINOUT membus;
  uint i,numberofobject,numberofregister,numberofchange,maxobj,numproc,whichproc,int_object;
  
  uint transfer_id,processor_id;
  
  uint c,g;
  
  uint r,t;
  
  bool done;
  
  bool prog_done;

  bool read_done;
  
  bool first_prog;
  
  
  
  bool request,changestate,endtransfer;
  char* type;
  uint16_t ID;
  uint32_t WHICH_TRACEX;
  
  uint32_t addressing(uint32_t addr) {return (addr-start_address);}
  
  
  void simuldma2control();
};

#endif // __DMA2CONTROL_H__
