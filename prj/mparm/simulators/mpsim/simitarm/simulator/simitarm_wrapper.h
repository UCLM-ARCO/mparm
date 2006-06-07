///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         simitarm_wrapper.h
// author       DEIS - Universita' di Bologna
//              Federico Ferrari
//              Stefano Ronconi
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Wraps a C++ ARM ISS in a SystemC envelope
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __SIMITARM_WRAPPER_H__
#define __SIMITARM_WRAPPER_H__


#include "globals.h"
#include "armsim.hpp"
#include "misc.h"
#include <cstring>
#include <iostream>
#include <systemc.h>
#include "core_signal.h"
#include <sys/time.h>
#include <sys/resource.h>                   // Availability under Windows to be checked
#include <unistd.h>
#include "stats.h"


SC_MODULE(Wrapper)
{
	simulator::arm_simulator *sima;						// StrongArm simulator
	
	sc_in_clk clock;
	sc_inout<PINOUT> *pinout_ft_master;
	sc_in<bool> *ready_from_master;
	sc_out<bool> *request_to_master;
	sc_inout<bool> *extint;
	
	void tick();								// synchronizing StrongArm with the SystemC clock
	void access();								// accessing to the bus
	
	PINOUT pinout;
	unsigned int ID;
	sc_signal<bool>  go;
	bool verbose;
	word_t *data_burst;
	UInt32 data_size;
	struct timeval begin_u, end_u, begin_s, end_s;
	struct rusage usg;
	float user_time, sys_time;

    public:
	sc_signal<bool> bus_busy;
	void gotobus(word_t *data,target_addr_t addr,bool rw,UInt32 size,bool blocky);
	void LoadProgram(const char *filename, int argc, char *argv[], char *envp[]);	// program loading
	
	SC_HAS_PROCESS(Wrapper);
	
	Wrapper::Wrapper(sc_module_name nm, unsigned int id, uint8_t* start,
				uint32_t ICacheBlocks, uint32_t ICacheAssoc, uint32_t ITLBBlocks, uint32_t ITLBAssoc,
				uint32_t DCacheBlocks, uint32_t DCacheAssoc, uint32_t DTLBBlocks, uint32_t DTLBAssoc, uint32_t WBEntries) :
	  sc_module(nm)
	{
		bus_busy = false;
		go = false;
		verbose = true;
 		
		ID = id;
		
		if (DMA)
		{
		  pinout_ft_master = new sc_inout<PINOUT> [2];
		  ready_from_master = new sc_in<bool> [2];
		  request_to_master = new sc_out<bool> [2];
		}
		else
		{
		  pinout_ft_master = new sc_inout<PINOUT> [1];
		  ready_from_master = new sc_in<bool> [1];
		  request_to_master = new sc_out<bool> [1];
		}
    
		extint = new sc_inout<bool> [NUMBER_OF_EXT];
		
		sima = new simulator::arm_simulator(this,verbose,(unsigned int)start,ICacheBlocks,ICacheAssoc,ITLBBlocks,ITLBAssoc,
							DCacheBlocks,DCacheAssoc,DTLBBlocks,DTLBAssoc,WBEntries);
		
		SC_CTHREAD(tick, clock.pos());
		SC_CTHREAD(access, clock.pos());
	}
	
};

#endif // __SIMITARM_WRAPPER_H__
