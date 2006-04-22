///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         dual_clock_fifo.h
// author       DEIS - Universita' di Bologna
//              Martino Ruggiero - mruggiero@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// info         Implements the FIFO of a dual-clock sync module
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DUAL_CLOCK_FIFO_H__
#define __DUAL_CLOCK_FIFO_H__

#include <systemc.h>
#include "core_signal.h"
#include "debug.h"

#define MAX_COUNT_FIFO 4 
#define MAX_ADDR_FIFO  MAX_COUNT_FIFO-1

struct RAMWORD
{

  PINOUT pinout;
  bool empty;
  int ID;
  
  inline bool operator == (const RAMWORD& rhs) const
      {
        return (rhs.pinout == pinout &&
                 rhs.empty == empty  &&
                 rhs.ID == ID
                 );
      }
};

///////////////////////////////////////
//PUSH INTERFACE
////////////////////////////////

SC_MODULE(push_interface)
{
RAMWORD* memdata;
uint32_t push_addr;

public:
//Push Interface
sc_in<bool> push_req_n;
sc_in<bool> clk_push;
sc_in<bool> reset;
sc_out<bool> push_error;
sc_in<PINOUT> data_in;

sc_out<sc_uint<32> > wr_addr;
sc_out<sc_uint<32> > gray_addr;

void push();

SC_HAS_PROCESS(push_interface);
push_interface(sc_module_name nm, RAMWORD* mem)
: sc_module (nm)
,push_error (false)
,wr_addr    (0)
,gray_addr (0)
 {
push_addr = 0;
memdata=mem;

SC_CTHREAD(push, clk_push.pos()); 

 }
};

///////////////////////////////////////
//POP INTERFACE
////////////////////////////////
SC_MODULE(pop_interface)
{
RAMWORD* memdata;
uint32_t pop_addr;

public:
//Pop Interface
sc_in<bool> pop_req_n;
sc_in<bool> clk_pop;
sc_in<bool> reset;

sc_out<bool> pop_error;
sc_out<sc_uint<32> > rd_addr;
sc_out<sc_uint<32> > gray_addr;
sc_out<PINOUT> data_out;

void pop();

SC_HAS_PROCESS(pop_interface);
pop_interface(sc_module_name nm, RAMWORD* mem)
: sc_module (nm)
,pop_error (false)
,rd_addr    (0)
,gray_addr (0)
 {
pop_addr = 0;
memdata=mem;

SC_CTHREAD(pop, clk_pop.pos()); 
 
 } 
};
///////////////////////////////////////
//CONTROL MODULE
////////////////////////////////
SC_MODULE(control_module)
{
RAMWORD* memdata;
int push_af_lvl;
int pop_ae_lvl;

uint32_t pop_addr;
uint32_t push_addr;
int count;

public:
sc_in<bool> reset;

sc_out<bool> push_full;
sc_out<bool> push_af;

sc_out<bool> pop_empty;
sc_out<bool> pop_ae;

sc_in<sc_uint<32> > sync_push_addr;
sc_in<sc_uint<32> > sync_pop_addr;

void control();

SC_HAS_PROCESS(control_module);
control_module(sc_module_name nm, RAMWORD* mem)
: sc_module (nm)
,push_full  (false)
,push_af    (false)
 {
pop_empty.initialize(true);
pop_ae.initialize(true);

push_af_lvl = INTERNAL_DIM_BURST - 1;
pop_ae_lvl  = INTERNAL_DIM_BURST - 1;

pop_addr = 0;
push_addr = 0;
count = 0;
 
memdata=mem;
 
SC_THREAD(control)
sensitive << sync_push_addr;
sensitive << sync_pop_addr;
 }
}; 
 
 
///////////////////////////////////////
//FIFO CONTROLLER
////////////////////////////////

SC_MODULE(fifo_controller)
{
public:
uint depth;
RAMWORD memdata[64];


public:
pop_interface* pop_if;
push_interface* push_if;
control_module* control;

public:
sc_signal<sc_uint<32> > *wr_gray;
sc_signal<sc_uint<32> > *rd_gray;
sc_signal<sc_uint<32> > *push_addr;
sc_signal<sc_uint<32> > *pop_addr;
sc_signal<bool > *pop_e;
sc_signal<bool > *push_f;

public:
SC_HAS_PROCESS(fifo_controller);
fifo_controller(sc_module_name nm)
: sc_module(nm)
{
depth = 8;

for(int i=0 ; i<64 ; i++){
memdata[i].empty = true ;
}

wr_gray = new sc_signal<sc_uint<32> >;
rd_gray = new sc_signal<sc_uint<32> >;
push_addr = new sc_signal<sc_uint<32> >;
pop_addr = new sc_signal<sc_uint<32> >;
pop_e = new sc_signal<bool >;
push_f = new sc_signal<bool >;

push_if = new push_interface("push_if",memdata);
pop_if  = new pop_interface("pop_if",memdata);
control = new control_module("control",memdata);
 
pop_if->gray_addr(*rd_gray);
control->sync_pop_addr(*rd_gray);

push_if->gray_addr(*wr_gray);
control->sync_push_addr(*wr_gray);

control->pop_empty(*pop_e);
control->push_full(*push_f);

push_if->wr_addr(*push_addr);

pop_if->rd_addr(*pop_addr);

push_if->reset(ResetGen_1);
pop_if->reset(ResetGen_1);
control->reset(ResetGen_1);

}

};

#endif // __DUAL_CLOCK_FIFO_H__
