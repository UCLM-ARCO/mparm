///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_ahb_bridge.cpp
// author       DEIS - Universita' di Bologna
//              Martino Ruggiero - mruggiero@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// info         Implements a unidirectional bridge between two AMBA AHB layers.
//
///////////////////////////////////////////////////////////////////////////////

#include "amba_ahb_bridge.h"

amba_ahb_bridge::amba_ahb_bridge(sc_module_name nm, uint id)
  : sc_module(nm),
    bridge_ID(id)
{
  pinout_bridge1 = new sc_signal<PINOUT>;
  pinout_bridge2 = new sc_signal<PINOUT>;
  pinout_bridge3 = new sc_signal<PINOUT>;
  pinout_bridge4 = new sc_signal<PINOUT>;
  
  bmif = new amba_ahb_bridge_mif("bmif", bridge_ID);
  bsif = new amba_ahb_bridge_sif("bsif", bridge_ID);
  
  wr_f = new fifo_controller("wr_f");
  rd_f = new fifo_controller("rd_f");
  
  // External bindings
  wr_f->push_if->clk_push(clk_slave);
  rd_f->pop_if->clk_pop(clk_slave);
  
  bmif->clock(clk_master);
  wr_f->pop_if->clk_pop(clk_master);
  rd_f->push_if->clk_push(clk_master);
  bmif->master_port(master_port);
  
  // SIF->Write FIFO
  RequestToPush_Sif = new sc_signal<bool>;
  bsif->request_to_push(*RequestToPush_Sif);
  wr_f->push_if->push_req_n(*RequestToPush_Sif);
  
  bsif->push_full(*wr_f->push_f);
  
  ErrorPush_Sif = new sc_signal<bool>;
  bsif->error_from_push(*ErrorPush_Sif);
  wr_f->push_if->push_error(*ErrorPush_Sif);
  
  ORPush_Sif = new sc_signal<bool>;
  bsif->write_overrun(*ORPush_Sif);
  wr_f->control->push_af(*ORPush_Sif);
  
  // MIF->Write FIFO
  RequestToPop_Mif = new sc_signal<bool>;
  bmif->request_to_pop(*RequestToPop_Mif);
  wr_f->pop_if->pop_req_n(*RequestToPop_Mif);
  
  bmif->pop_empty(*wr_f->pop_e);
  
  ErrorPop_Mif = new sc_signal<bool>;
  bmif->error_from_pop(*ErrorPop_Mif);
  wr_f->pop_if->pop_error(*ErrorPop_Mif);
  
  ORPop_Mif = new sc_signal<bool>;
  bmif->read_overrun(*ORPop_Mif);
  wr_f->control->pop_ae(*ORPop_Mif);
  
  bsif->pinout_to_wrf(*pinout_bridge1);
  wr_f->push_if->data_in(*pinout_bridge1);
  bmif->pinout_from_wrf(*pinout_bridge2);
  wr_f->pop_if->data_out(*pinout_bridge2);
  
  // MIF->Read FIFO
  RequestToPush_Mif = new sc_signal<bool>;
  bmif->request_to_push(*RequestToPush_Mif);
  rd_f->push_if->push_req_n(*RequestToPush_Mif);
  
  bmif->push_full(*rd_f->push_f);
  
  ErrorPush_Mif = new sc_signal<bool>;
  bmif->error_from_push(*ErrorPush_Mif);
  rd_f->push_if->push_error(*ErrorPush_Mif);
  
  ORPush_Mif = new sc_signal<bool>;
  bmif->write_overrun(*ORPush_Mif);
  rd_f->control->push_af(*ORPush_Mif);
  
  // SIF->Read FIFO
  RequestToPop_Sif = new sc_signal<bool>;
  bsif->request_to_pop(*RequestToPop_Sif);
  rd_f->pop_if->pop_req_n(*RequestToPop_Sif);
  
  bsif->pop_empty(*rd_f->pop_e);
  
  ErrorPop_Sif = new sc_signal<bool>;
  bsif->error_from_pop(*ErrorPop_Sif);
  rd_f->pop_if->pop_error(*ErrorPop_Sif);
  
  ORPop_Sif = new sc_signal<bool>;
  bsif->read_overrun(*ORPop_Sif);
  rd_f->control->pop_ae(*ORPop_Sif);//FIXME
  
  bmif->pinout_to_rdf(*pinout_bridge3);
  rd_f->push_if->data_in(*pinout_bridge3);
  bsif->pinout_from_rdf(*pinout_bridge4);
  rd_f->pop_if->data_out(*pinout_bridge4);
}
