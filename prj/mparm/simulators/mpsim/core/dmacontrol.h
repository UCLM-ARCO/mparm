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


#ifndef __DMACONTROL_H__
#define __DMACONTROL_H__

#include <systemc.h>
#include "globals.h"
#include "core_signal.h"
#include "debug.h"
#include "address.h"

SC_MODULE(dmacontrol)
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
  
  SC_HAS_PROCESS(dmacontrol);
  
  dmacontrol(sc_module_name nm, uint16_t id, uint32_t obj, uint32_t nproc) :
    sc_module(nm)
  {
    ID = id;
    //number of possible processor that can program the dma
    if(!(0<nproc<=N_CORES)) 
    {
     printf("DMA_CONTROL Wrong number of processor:%d\n",nproc);
     exit(1);
    }
    numproc= nproc;
    maxobj = obj;
    size = ((maxobj)*4*7+4)*numproc;
    reg = (DMA_CONT_REG *) malloc(maxobj*sizeof(DMA_CONT_REG)*numproc);
        
    //Now the queue of the work to do has the same dimension of the maximun object that you
    //can add to the dma
    work = (uint32_t*) malloc(maxobj*sizeof(uint32_t)*numproc);
    
    for (uint i=0;i<maxobj*numproc;i++)
      work[i]=0xFFFFFFFF; 
  
    counter=(uint32_t*)malloc(numproc*sizeof(uint32_t));
    free=(bool**)malloc(numproc*sizeof(bool*));
    for (uint j=0;j<numproc;j++)
    {
     free[j]=(bool*)malloc(sizeof(bool)*maxobj);
     for (uint i=0;i<maxobj;i++)
      free[j][i]=true;
     counter[j]=0;
    }  
    nwork=0;
    freepos=0;
    working=0;
    numberofchange=0;
    request=false;
    changestate=false;
    endtransfer=false;
    
    WHICH_TRACEX=IDMA_CONT_TRACEX;
    
    SC_CTHREAD(simuldmacontrol, clock.pos());
  }

  protected:

  DMA_CONT_REG* reg;
  DMA_CONT_REG1 now; // the object actually on transfer
  bool** free;   //free positions in the reg table : one for cores
  uint32_t* counter;  //number of free positions in the reg array : one for cores
  uint32_t nwork,    //number of works to do in the queue
	   working,freepos,  //on going transfer, first work to do in the queue
	   size; //size of the dma related to the dimension the maxobj parameter
  uint32_t* work; //queue of the works to do 
  
  PINOUT membus;
  uint i,numberofobject,numberofregister,numberofchange,maxobj,numproc,whichproc,int_object;
  bool request,changestate,endtransfer;
  char* type;
  uint16_t ID;
  uint32_t WHICH_TRACEX;
  
  virtual uint32_t addressing(uint32_t addr) = 0;
  virtual bool control(uint32_t addr)
   { 
    uint8_t dummy=0;
    return
     (!addresser->PhysicalInSmartmem(addr) 
     && !addresser->PhysicalInPrivateSpace(addr)
     && !addresser->PhysicalInSharedSpace(addr)
     && !addresser->PhysicalInCoreSlaveSpace(addr)  
     && !addresser->PhysicalInScratchSpace((uint8_t)ID, 
                     addresser->Logical2Physical(membus.data,ID), &dummy));
   };
   
  void simuldmacontrol();
};

#endif // __DMACONTROL_H__
