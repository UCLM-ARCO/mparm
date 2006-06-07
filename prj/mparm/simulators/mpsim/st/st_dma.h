///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         st_dma.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Implements a complete DMA (bus side)
//
///////////////////////////////////////////////////////////////////////////////
#ifndef __ST_DMA_CLASS_H__
#define __ST_DMA_CLASS_H__

#include <systemc.h>
#include "config.h"
#include "globals.h"
#include "dmatransfer.h"
#include <list>

class St_Dma_transfer :public dmatransfer
{  
 protected:
 bool Write(uint32_t addr, uint32_t* data, uint32_t nburst);
 bool Read(uint32_t addr, uint32_t* data, uint32_t nburst);
 //I need to rewrite the basic transfer function for problems with the four word alignement
 //needed in the st-bus burst
 void execute_transf();
 bool local1,local2;
 
 virtual uint32_t read_local(uint32_t addr)=0;
 virtual void write_local(uint32_t addr,uint32_t data)=0;
 
 std::list< uint32_t >* buffer1;
 
 public:

 //signals for the master
 sc_inout<PINOUT> pinout;
 sc_in<bool> ready_from_master;
 sc_out<bool> request_to_master;
 
 St_Dma_transfer(sc_module_name nm, uint16_t id, uint32_t dimburst, uint32_t obj, 
  			uint32_t nproc) :
  dmatransfer(nm,id,dimburst,obj,nproc)
 {
   buffer1= new std::list<uint32_t>();
   local1=local2=false;
 };
   
};

#endif 
//__ST_DMA_CLASS_H__
