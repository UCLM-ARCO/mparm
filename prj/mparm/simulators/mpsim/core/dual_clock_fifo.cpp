///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         dual_clock_fifo.cpp
// author       DEIS - Universita' di Bologna
//              Martino Ruggiero - mruggiero@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// info         Implements the FIFO of a dual-clock sync module
//
///////////////////////////////////////////////////////////////////////////////

#include "dual_clock_fifo.h"

void control_module::control(){
while(1){
if(!reset.read()){
TRACEX(DCFIFO_TRACEX,11,"%s : control_module::control\n",name());

pop_addr = sync_pop_addr.read();
push_addr = sync_push_addr.read();

if(push_addr > pop_addr)
  count = abs((int) push_addr - (int) pop_addr);
if(push_addr < pop_addr)
  count = abs((int) push_addr - (int) pop_addr + MAX_COUNT_FIFO );
if(push_addr == pop_addr)
  if(memdata[(int)push_addr].empty == false){
    count=MAX_COUNT_FIFO;
    
    pop_ae.write(false);
    pop_empty.write(false);
    
    push_af.write(true);
    push_full.write(true);

    }
  else{
    count=0;
    
    pop_empty.write(true);
    pop_ae.write(true);
    
    push_af.write(false);
    push_full.write(false);
    }

if(count > 0 && count < MAX_COUNT_FIFO){
        push_full.write(false);
        pop_empty.write(false);
        }

if(count < push_af_lvl)
        push_af.write(false);

if(count > pop_ae_lvl) 
        pop_ae.write(false);
      

TRACEX(DCFIFO_TRACEX,11,"%s : push_interface::control, count = %d\n",name(),count);
wait();
}
else
wait();
}
}


void push_interface::push(){
PINOUT pinout;

while(true){

 if(push_req_n.read() == true){
   TRACEX(DCFIFO_TRACEX,11,"%s : push_req_n.read() == true\n",name());
        if(memdata[(int)push_addr].empty == true){
        TRACEX(DCFIFO_TRACEX,11,"%s : push_req_n.read() == true  push_full_flag == false\n",name());
                pinout = data_in.read();
                memdata[(int)push_addr].pinout = pinout;
                memdata[(int)push_addr].empty = false;

   
        if((int)push_addr == MAX_ADDR_FIFO)
                push_addr=0;
        else
                push_addr++;

TRACEX(DCFIFO_TRACEX,11,"%s : push_count++ at %10.1f ns\n",name(), sc_simulation_time()); 

        gray_addr.write(push_addr); //AGGIUNGERE CODIFICA
        wr_addr.write(push_addr);   // FIXME
        ASSERT((int)push_addr <= MAX_ADDR_FIFO);
        
TRACEX(DCFIFO_TRACEX,11,"%s : gray_addr.write(push_addr);\n",name());
        wait();
        }
        else{
        push_error.write(true);
        wait();
        }

        }
        else{
        wait();
        }
}

}


void pop_interface::pop() {
PINOUT pinout;

while(true){

 if(pop_req_n.read() == true){
        if(memdata[(int)pop_addr].empty == false){
        pinout = memdata[(int)pop_addr].pinout;
        memdata[(int)pop_addr].empty=true;
        data_out.write(pinout);
       
                if((int)pop_addr == MAX_ADDR_FIFO)
                pop_addr=0;
                else
                pop_addr++;

                rd_addr.write(pop_addr);
                gray_addr.write(pop_addr);

TRACEX(DCFIFO_TRACEX,11,"%s : pop_count-- at %10.1f ns\n",name(), sc_simulation_time()); 
                wait();

                }
        else{
        pop_error.write(true);
        wait();
        }
        }
        else{
        wait();
        }

 }

}




