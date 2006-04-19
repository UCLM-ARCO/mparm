///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         dual_clock_pinout_adapter.cpp
// author       DEIS - Universita' di Bologna
//              Martino Ruggiero - mruggiero@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// info         Implements a dual-clock sync module, with pinout interface
//
///////////////////////////////////////////////////////////////////////////////

#include "dual_clock_pinout_adapter.h"

dual_clock_pinout_adapter::dual_clock_pinout_adapter(sc_module_name nm)
: sc_module(nm)
{
  pinout_1 = new sc_signal<PINOUT>;
  pinout_2 = new sc_signal<PINOUT>;
  pinout_3 = new sc_signal<PINOUT>;
  pinout_4 = new sc_signal<PINOUT>;

  sif = new dual_clock_pinout_slave_if("sif");
  mif = new dual_clock_pinout_master_if("mif");

  wr_f = new fifo_controller("wr_f");
  rd_f = new fifo_controller("rd_f");

  sif->clock(clk_s);
  sif->reset(rst_s);
  sif->request_from_wrapper(request_from_wrapper);
  sif->ready_to_wrapper(ready_to_wrapper);
  sif->pinout(pinout_s);
  rd_f->pop_if->clk_pop(clk_s);
  wr_f->push_if->clk_push(clk_s);
  
  mif->clock(clk_m);
  rd_f->push_if->clk_push(clk_m);
  wr_f->pop_if->clk_pop(clk_m);
  mif->reset(rst_m);
  mif->request_to_master(request_to_master);
  mif->ready_from_master(ready_from_master);
  mif->pinout(pinout_m);
  
  //sif->Write FIFO
  RequestToPush_Sif = new sc_signal<bool>;
  sif->request_to_push(*RequestToPush_Sif);
  wr_f->push_if->push_req_n(*RequestToPush_Sif);
  
  //PushFull_Sif = new sc_signal<bool>;
  //sif->push_full(*PushFull_Sif);
  //wr_f->push_if->push_full(*PushFull_Sif);
  //wr_f->pop_if->push_full(*PushFull_Sif);
  sif->push_full(*wr_f->push_f);
  
  ErrorPush_Sif = new sc_signal<bool>;
  sif->error_from_push(*ErrorPush_Sif);
  wr_f->push_if->push_error(*ErrorPush_Sif);
  
  ORPush_Sif = new sc_signal<bool>;
  sif->write_overrun(*ORPush_Sif);
  wr_f->control->push_af(*ORPush_Sif);
  
  //wr_f->push_if->clk_push(ClockGen_1);
  
  //MIF->Write FIFO
  RequestToPop_Mif = new sc_signal<bool>;
  mif->request_to_pop(*RequestToPop_Mif);
  wr_f->pop_if->pop_req_n(*RequestToPop_Mif);
  
  //PopEmpty_Mif = new sc_signal<bool>;
  //mif->pop_empty(*PopEmpty_Mif);
  //wr_f->pop_if->pop_empty(*PopEmpty_Mif);
  mif->pop_empty(*wr_f->pop_e);
  
  ErrorPop_Mif = new sc_signal<bool>;
  mif->error_from_pop(*ErrorPop_Mif);
  wr_f->pop_if->pop_error(*ErrorPop_Mif);
  
  ORPop_Mif = new sc_signal<bool>;
  mif->read_overrun(*ORPop_Mif);
  wr_f->control->pop_ae(*ORPop_Mif);
  
  //wr_f->pop_if->clk_pop(ClockGen_1);
  
  
  sif->pinout_to_wrf(*pinout_1);
  wr_f->push_if->data_in(*pinout_1);
  mif->pinout_from_wrf(*pinout_2);
  wr_f->pop_if->data_out(*pinout_2);
  
  //MIF->->Read FIFO
  RequestToPush_Mif = new sc_signal<bool>;
  mif->request_to_push(*RequestToPush_Mif);
  rd_f->push_if->push_req_n(*RequestToPush_Mif);
  
  //PushFull_Mif = new sc_signal<bool>;
  //mif->push_full(*PushFull_Mif);
  //rd_f->push_if->push_full(*PushFull_Mif);
  mif->push_full(*rd_f->push_f);
  
  ErrorPush_Mif = new sc_signal<bool>;
  mif->error_from_push(*ErrorPush_Mif);
  rd_f->push_if->push_error(*ErrorPush_Mif);
  
  ORPush_Mif = new sc_signal<bool>;
  mif->write_overrun(*ORPush_Mif);
  rd_f->control->push_af(*ORPush_Mif);
  
  //rd_f->push_if->clk_push(ClockGen_1);
  
  
  //Sif->Read FIFO
  RequestToPop_Sif = new sc_signal<bool>;
  sif->request_to_pop(*RequestToPop_Sif);
  rd_f->pop_if->pop_req_n(*RequestToPop_Sif);
  
  //PopEmpty_Sif = new sc_signal<bool>;
  //sif->pop_empty(*PopEmpty_Sif);
  //rd_f->pop_if->pop_empty(*PopEmpty_Sif);
  sif->pop_empty(*rd_f->pop_e);
  
  ErrorPop_Sif = new sc_signal<bool>;
  sif->error_from_pop(*ErrorPop_Sif);
  rd_f->pop_if->pop_error(*ErrorPop_Sif);
  
  ORPop_Sif = new sc_signal<bool>;
  sif->read_overrun(*ORPop_Sif);
  rd_f->control->pop_ae(*ORPop_Sif);//FIXME
  
  //rd_f->pop_if->clk_pop(ClockGen_1);
  
  mif->pinout_to_rdf(*pinout_3);
  rd_f->push_if->data_in(*pinout_3);
  sif->pinout_from_rdf(*pinout_4);
  rd_f->pop_if->data_out(*pinout_4);
}

