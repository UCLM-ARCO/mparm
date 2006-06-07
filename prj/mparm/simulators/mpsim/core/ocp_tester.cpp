///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         ocp_tester.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Implements a simple synthesizable OCP traffic generator
//
///////////////////////////////////////////////////////////////////////////////

#include "ocp_tester.h"

#ifdef XPIPES_SIMULATION  
  #include "stats.h"
#endif

///////////////////////////////////////////////////////////////////////////////
// request - Issues OCP transactions as dictated by the configuration memories.
void ocp_tester::request()
{
  if (reset.read())
  {
	st = tester_init;
	trans_counter = 0;
	// safe initialization
	MCmd.write(OCPCMDIDLE);
	MRespAccept.write(true);
	// these will never be changed during the test
	MAddrSpace.write(0x0);
	MByteEn.write(OCPMBYEWORD);
	MBurstPrecise.write(true);
	MBurstSeq.write(OCPMBSINCR);
	// test results
	test_ok.write(false);
#ifdef XPIPES_SIMULATION
	data = 0x0;
	address = 0x0;
	burst = 0;
	idlewait = 0;
	benable = TGEN_WORD;
	wr = false;
	if (STATS && !statistics_started)
	{
		statobject->startMeasuring(ID);
		statistics_started = true;
	}
#else
	// Implement pairs of W/R transactions to the same base address
	// and check if reads match writes. Transactions can be bursts or
	// not, but they must match in pairs.
	// The latency between any transaction is configurable.
	// Check against ocp_memory size to avoid going out of bounds!
	address[0] = 0x00000000;
	address[1] = 0x00000000;
	address[2] = 0x00000200;
	address[3] = 0x00000200;
	address[4] = 0x00000300;
	address[5] = 0x00000300;
	address[6] = 0x00000600;
	address[7] = 0x00000600;
	address[8] = 0x00000b00;
	address[9] = 0x00000b00;
	
	data[0] = 0x00000000;
	data[1] = 0xdeadbeef;
	data[2] = 0x22222222;
	data[3] = 0xdeadbeef;
	data[4] = 0x44444444;
	data[5] = 0xdeadbeef;
	data[6] = 0x66666666;
	data[7] = 0xdeadbeef;
	data[8] = 0x88888888;
	data[9] = 0xdeadbeef;
	
	burst[0] = 0x4;
	burst[1] = 0x4;
	burst[2] = 0x8;
	burst[3] = 0x8;
	burst[4] = 0x4;
	burst[5] = 0x4;
	burst[6] = 0x1;
	burst[7] = 0x1;
	burst[8] = 0x4;
	burst[9] = 0x4;
	
	idlewait[0] = 0x1;
	idlewait[1] = 0x1;
	idlewait[2] = 0x8;
	idlewait[3] = 0x11;
	idlewait[4] = 0x34;
	idlewait[5] = 0x22;
	idlewait[6] = 0x1;
	idlewait[7] = 0x4;
	idlewait[8] = 0x5;
	idlewait[9] = 0x6;
	
	wr[0] = true;
	wr[1] = false;
	wr[2] = true;
	wr[3] = false;
	wr[4] = true;
	wr[5] = false;
	wr[6] = true;
	wr[7] = false;
	wr[8] = true;
	wr[9] = false;
#endif
  }
  else
  {
	switch (st)
	{
		case tester_init:
#ifdef XPIPES_SIMULATION
			transgen->get_trans(&data, &address, &burst, &wr, &benable, &idlewait);
			// Only word-sized are currently supported
			ASSERT(benable == TGEN_WORD);
			if (idlewait == -1)
			{
				if (!statistics_finished)
				{
					if (STATS)
						statobject->stopMeasuring(ID);
					statobject->quit(ID);
					statistics_finished = true;
				}
				break;
			}
			if (wr)
			{
				MCmd.write(OCPCMDWRITE);
				if (STATS)
					statobject->putOCPTransactionCommand(OCPCMDWRITE, burst, ID);
			}
			else
			{
				MCmd.write(OCPCMDREAD);
				if (STATS)
					statobject->putOCPTransactionCommand(OCPCMDREAD, burst, ID);
			}
			MData.write(data);
			MBurstLength.write(burst);
			MAddr.write(address);
#else
			if (wr[trans_counter])
				MCmd.write(OCPCMDWRITE);
			else
				MCmd.write(OCPCMDREAD);
			MData.write(data[trans_counter]);
			MBurstLength.write(burst[trans_counter]);
			MAddr.write(address[trans_counter]);
#endif
			// MRespAccept stays the same
			MDataValid.write(true);
			burst_counter = 1;
			
#ifdef XPIPES_SIMULATION
			if (!wr && burst != 1)
#else
			if (!wr[trans_counter] && burst[trans_counter] != 1)
#endif
			{
				MBurstSingleReq.write(true);
				MReqLast.write(true);
				MDataLast.write(true);
				next_st = tester_wait_last_SCA;
			}
			else
			{
				MBurstSingleReq.write(false);
#ifdef XPIPES_SIMULATION
				if (burst == 1)
#else
				if (burst[trans_counter] == 1)
#endif
				{
					MReqLast.write(true);
					MDataLast.write(true);
					next_st = tester_wait_last_SCA;
				}
				else
				{
					MReqLast.write(false);
					MDataLast.write(false);
					next_st = tester_wait_SCA;
				}
			}
			break;
		case tester_wait_SCA:
			if (SCmdAccept.read())
			{
				// MCmd stays the same
#ifdef XPIPES_SIMULATION
				MData.write(data  + burst_counter);
				MAddr.write(address + burst_counter * 4);
				if (STATS && burst_counter == 0)
					statobject->getOCPTransactionCommandAccept(wr ? OCPCMDWRITE : OCPCMDREAD, burst, ID);
#else
				MData.write(data[trans_counter] + burst_counter);
				MAddr.write(address[trans_counter] + burst_counter * 4);
#endif
				// MBurstLength stays the same
				// MRespAccept stays the same
				// MDataValid stays up
				// MBurstSingleReq stays the same
#ifdef XPIPES_SIMULATION
				if (burst_counter + 1 == burst)
#else
				if (burst_counter + 1 == burst[trans_counter])
#endif
				{
					MReqLast.write(true);
					MDataLast.write(true);
					next_st = tester_wait_last_SCA;
				}
				else
				{
					MReqLast.write(false);
					MDataLast.write(false);
					next_st = tester_wait_SCA;
				}
				burst_counter ++;
			}
			else
				next_st = tester_wait_SCA;
			break;
		case tester_wait_last_SCA:
			if (SCmdAccept.read())
			{
				MCmd.write(OCPCMDIDLE);
				MData.write(0xdeadbeef);
				MBurstLength.write(0x0);
				// MRespAccept stays the same
				MDataValid.write(false);
				MAddr.write(0xdeadbeef);
				MBurstSingleReq.write(false);
				MReqLast.write(false);
				MDataLast.write(false);
				burst_counter = 0;
#ifdef XPIPES_SIMULATION
				if (!wr)
#else
				if (!wr[trans_counter])
#endif
					next_st = tester_wait_SR;
				else
				{
					next_st = tester_idle;
					wait_counter = 1;
				}
			}
			else
				next_st = tester_wait_last_SCA;
			break;
		case tester_wait_SR:
			if (SResp.read() == OCPSRESDVA)
			{
				// MCmd stays idle
				// MData stays idle
				// MBurstLength stays idle
				// MRespAccept stays the same
				// MDataValid stays idle
				// MAddr stays idle
				// MBurstSingleReq stays idle
				// MReqLast stays idle
				// MDataLast stays idle
				// ***BEWARE*** don't do a read as the first transaction!!!!!
#ifdef XPIPES_SIMULATION
				if (SData.read() == data + burst_counter)
#else
				if (SData.read() == data[trans_counter - 1] + burst_counter)
#endif
					test_ok.write(true);
				else
					test_ok.write(false);
				

#ifdef XPIPES_SIMULATION
				if (burst_counter + 1 == burst)
#else
				if (burst_counter + 1 == burst[trans_counter])
#endif
				{
					next_st = tester_idle;
					wait_counter = 1;
#ifdef XPIPES_SIMULATION
					if (STATS)
						statobject->getOCPTransactionResp(wr ? OCPCMDWRITE : OCPCMDREAD, burst, ID);
#endif
				}
				else
				{
					next_st = tester_wait_SR;
					burst_counter ++;
				}
			}
			else
				next_st = tester_wait_SR;
			break;
		case tester_idle:
			// MCmd stays idle
			// MData stays idle
			// MBurstLength stays idle
			// MRespAccept stays the same
			// MDataValid stays idle
			// MAddr stays idle
			// MBurstSingleReq stays idle
			// MReqLast stays idle
			// MDataLast stays idle
#ifdef XPIPES_SIMULATION
			if (wait_counter + 1 >= (uint)idlewait)
#else
			if (wait_counter + 1 >= idlewait[trans_counter])
#endif
			{
				next_st = tester_init;
				wait_counter = 0;
#ifdef XPIPES_SIMULATION
				trans_counter ++;
#else
				if (trans_counter + 1 == NUM_TRANSACTIONS)
				{
					// restart from scratch
					trans_counter = 0;
				}
				else
					trans_counter ++;
#endif
			}
			else
			{
				next_st = tester_idle;
				wait_counter ++;
			}
			
			break;
	}
        st = next_st;
    }
}
