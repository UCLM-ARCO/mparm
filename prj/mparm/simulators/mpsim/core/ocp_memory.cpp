///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         ocp_memory.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Implements a simple synthesizable OCP memory
//
///////////////////////////////////////////////////////////////////////////////

#include "ocp_memory.h"

#ifdef XPIPES_SIMULATION  
  #include "debug.h"
#endif

// FIXME Assumes word accesses (ignores MByteEn)
// FIXME Doesn't pay attention to burst information for writes (treats them as
// a collection of single accesses)
// FIXME Only supports single request BRD
// TODO support for multicycle latency
// TODO actually check whether WRNP is working

///////////////////////////////////////////////////////////////////////////////
// request - Issues OCP transactions as dictated by the configuration memories.
void ocp_memory::receive()
{
  if (reset.read())
  {
	st = mem_init;
	next_st = mem_init;
	// safe initialization
	SResp.write(OCPSRESNULL);
	SRespLast.write(false);
	SCmdAccept.write(false);
	SData.write(0xdeadbeef);
	SDataAccept.write(false);
  }
  else
  {
	switch (st)
	{
		case mem_init:
			SResp.write(OCPSRESNULL);
			SRespLast.write(false);
			SData.write(0xdeadbeef);
			is_last = MReqLast.read();
			address = (MAddr.read() & 0x00fffffc) >> 2;
			data = MData.read();
			burst_counter = 0;
			//TODO imprecise transactions...
			switch ((int)MCmd.read())
			{
				case OCPCMDREAD:
					SCmdAccept.write(true);
					SDataAccept.write(true);
					// Is this a Single Request / Precise Burst Read, or just a read?
					if (MBurstSingleReq.read() || MBurstPrecise.read())
						blen = MBurstLength.read();
					else
						blen = 1;
					next_st = mem_read;
					break;
				case OCPCMDWRITE:
					SCmdAccept.write(true);
					SDataAccept.write(true);
					// Is this a Single Request / Precise Burst Write, or just a write?
					if (MBurstSingleReq.read() || MBurstPrecise.read())
						blen = MBurstLength.read();
					else
						blen = 1;
					if (MBurstSingleReq.read())
						next_st = mem_accept_sr_write;
					else
						next_st = mem_write;
					break;
				case OCPCMDWRNP:
					SCmdAccept.write(true);
					SDataAccept.write(true);
					// Is this a Single Request Burst Write, or just a write?
					if (MBurstSingleReq.read() || MBurstPrecise.read())
						blen = MBurstLength.read();
					else
						blen = 1;
					next_st = mem_wrnp;
					break; // TODO single-request WRNP
				default:
					SCmdAccept.write(false);
					SDataAccept.write(false);
					break;
			}
			
			break;
		// ----
		// Read
		// ----
		case mem_read:
			SCmdAccept.write(false);
			SDataAccept.write(false);
			
#ifdef XPIPES_SIMULATION
			TRACEX(TARGET_TRACEX, 10, "%s:        read     %8x at address 0x%08x | time: %10.0f\n",
				name(), (unsigned int)memory[address], (unsigned int)(address << 2), sc_simulation_time());
#endif
			SData.write(memory[address]);
			SResp.write(OCPSRESDVA);
			if ((blen > 1 && burst_counter == blen - 1) || (blen == 1 && is_last))
				SRespLast.write(true);
			else
				SRespLast.write(false);
			next_st = mem_wait_read_mra;
			break;
		case mem_wait_read_mra:
			if (MRespAccept.read())
			{
				if (++ burst_counter == blen)
				{
					next_st = mem_init;
				}
				else
				{
					address += 1;
					next_st = mem_read;
				}
				SResp.write(OCPSRESNULL);
				SRespLast.write(false);
				SData.write(0xdeadbeef);
			}
			else
				next_st = mem_wait_read_mra;
			break;
		// ------------
		// Posted write
		// ------------
		case mem_write:
			SCmdAccept.write(false); //TODO for now, always a single wait state...
			SDataAccept.write(false);
			
			memory[address] = data;
#ifdef XPIPES_SIMULATION
			TRACEX(TARGET_TRACEX, 10, "%s:        P write  %8x at address 0x%08x | time: %10.0f\n",
				name(), (unsigned int)data, (unsigned int)(address << 2), sc_simulation_time());
#endif
			next_st = mem_init;
			break;
		// ---------------------------
		// Single-request posted write
		// ---------------------------
		case mem_accept_sr_write:
			if (MDataValid.read())
			{
				SCmdAccept.write(false);
				SDataAccept.write(false);
			
				memory[(MAddr.read() & 0x00fffffc) >> 2] = MData.read();
#ifdef XPIPES_SIMULATION
				TRACEX(TARGET_TRACEX, 10, "%s:        P write  %8x at address 0x%08x | time: %10.0f\n",
					name(), (unsigned int)data, (unsigned int)(address << 2), sc_simulation_time());
#endif
				if (MDataLast.read())
					next_st = mem_init;
				else
					next_st = mem_wait_sr_write;
			}
			else
				next_st = mem_accept_sr_write;
			break;
		case mem_wait_sr_write:
			SCmdAccept.write(true);
			SDataAccept.write(true);
			
			next_st = mem_accept_sr_write;
			break;
		// ----------------
		// Non-posted write
		// ----------------
		case mem_wrnp:
			SCmdAccept.write(false);
			SDataAccept.write(false);
			
			memory[address] = data;
#ifdef XPIPES_SIMULATION
			TRACEX(TARGET_TRACEX, 10, "%s:        NP write %8x at address 0x%08x | time: %10.0f\n",
				name(), (unsigned int)data, (unsigned int)(address << 2), sc_simulation_time());
#endif
			SResp.write(OCPSRESDVA);
			if (is_last)
				SRespLast.write(true);
			else
				SRespLast.write(false);
			next_st = mem_wait_wrnp_mra;
			break;
		case mem_wait_wrnp_mra:
			if (MRespAccept.read())
			{
				next_st = mem_init;
				SResp.write(OCPSRESNULL);
				SRespLast.write(false);
			}
			else
				next_st = mem_wait_wrnp_mra;
			break;
	}
        st = next_st;
    }
}
