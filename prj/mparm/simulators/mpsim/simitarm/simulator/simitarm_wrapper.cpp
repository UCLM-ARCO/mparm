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



#include "simitarm_wrapper.h"


void Wrapper::gotobus(word_t *data,target_addr_t addr,bool rw,UInt32 size,bool blocky)
{
	do
	{
		wait();
	} while (bus_busy);
		
	bus_busy = true;

	data_burst = data;
	data_size=size;				       
	pinout.rw = rw;
	pinout.address = addr;							// struct pinout
        go = true;
	if (blocky)
		do
		{
			wait();
		} while (go);
}

void Wrapper::tick()
{

	sima -> initialize();							// StrongArm initialization
 	
										
	pinout.fiq = 1;								// pinout initialization
	pinout.irq = 1;
	pinout.address = 0;
	pinout.rw = 0;
	pinout.burst = 1;

	getrusage(RUSAGE_SELF, &usg);
	begin_u = usg.ru_utime;
	begin_s = usg.ru_stime;
        
	statobject->startMeasuring(ID);
                
	while(sima -> is_running())
	{
		sima -> clock_tick();
		wait();
	}

	sima->dump_stats(stderr);
	getrusage(RUSAGE_SELF, &usg);
	end_u = usg.ru_utime;
	end_s = usg.ru_stime;
	user_time = (end_u.tv_sec+end_u.tv_usec/1000000.0)-
		(begin_u.tv_sec+begin_u.tv_usec/1000000.0);
	sys_time = (end_s.tv_sec+end_s.tv_usec/1000000.0)-
		(begin_s.tv_sec+begin_s.tv_usec/1000000.0);
	fprintf(stderr,
		"Total user time  : %.3f sec.\n"
		"Total system time: %.3f sec.\n"
		"Simulation speed : %.3e cyc./sec.\n",
		user_time, sys_time, sima->get_cycle_count()/(user_time+sys_time));
                
	statobject->stopMeasuring(ID);
	statobject->quit(ID);
}



void Wrapper::access()
{
	while(1)
	{       do
		{
			wait();
		} while(!go);
		if (data_size%16 == 0)
		{	
			pinout.bw = 0;
			pinout.burst = data_size/4;
			for (unsigned int burst_count=0; burst_count < pinout.burst; burst_count++)
			{
				if (pinout.rw)
					pinout.data = *(data_burst + burst_count);
				pinout_ft_master[0].write(pinout);		      	// bus access
				request_to_master[0].write(true);				
				do
				{
					wait();
				} while(!ready_from_master[0].read());
				pinout = pinout_ft_master[0].read();
				if (!pinout.rw)
					*(data_burst + burst_count) = pinout.data;
			}
			request_to_master[0].write(false);
			wait();
		}
		else
		{
			while (data_size>0 && pinout.address%4!=0)				// disaligned access
			{
				pinout.bw = 1;
				pinout.burst = 1;						  
			  	if (pinout.rw)
					pinout.data = *(data_burst);
			   	pinout_ft_master[0].write(pinout);				// bus access
			    	request_to_master[0].write(true);				
			    	do
			     	{
					wait();
			     	} while(!ready_from_master[0].read());
			    	pinout = pinout_ft_master[0].read();
			    	if (!pinout.rw)
					*(data_burst) = pinout.data;
				request_to_master[0].write(false);
				pinout.address++;
				data_size--;
				(uint8_t *)data_burst = (uint8_t *)data_burst + 1;
			}
		  	while (data_size>=4)
		    	{
				pinout.bw = 0;
				pinout.burst = 1;
				if (pinout.rw)
			  		pinout.data = *(data_burst);
				pinout_ft_master[0].write(pinout);		     		// bus access
				request_to_master[0].write(true);				
				do
				{
					wait();
				} while(!ready_from_master[0].read());
				pinout = pinout_ft_master[0].read();
				if (!pinout.rw)
			  		*(data_burst) = pinout.data;
				request_to_master[0].write(false);
				pinout.address = pinout.address + 4;
				data_size = data_size - 4;
				data_burst++;
		    	}
			if (data_size > 1)
		  	{
				pinout.bw = 2;
				pinout.burst = 1;
				if (pinout.rw)
					pinout.data = *(data_burst);
				pinout_ft_master[0].write(pinout);				// bus access
				request_to_master[0].write(true);				
				do
				{
					wait();
				} while(!ready_from_master[0].read());
				pinout = pinout_ft_master[0].read();
				if (!pinout.rw)
					*(data_burst) = pinout.data;
				request_to_master[0].write(false);
				pinout.address = pinout.address + 2;
				data_size = data_size - 2;
				(uint8_t *)data_burst = (uint8_t *)data_burst + 2;
		  	}
			if (data_size > 0)
		    	{
				pinout.bw = 1;
				pinout.burst = 1;
				if (pinout.rw)
					pinout.data = *(data_burst);
				pinout_ft_master[0].write(pinout);				// bus access
				request_to_master[0].write(true);				
				do
				{
					wait();
				} while(!ready_from_master[0].read());
				pinout = pinout_ft_master[0].read();
				if (!pinout.rw)
					*(data_burst) = pinout.data;
		
				request_to_master[0].write(false);
		    	}
		}
		bus_busy = false;
		go = false;	
		wait();
	}			
 
}

void Wrapper::LoadProgram(const char *filename, int argc, char *argv[], char *envp[])
{
	sima->load_fpe(NULL);
	sima->load_program(filename,argc,argv,envp);
}
