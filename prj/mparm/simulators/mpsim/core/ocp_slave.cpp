///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         ocp_slave.cpp
// author       IMM - Technical University of Denmark (DTU)
//              DEIS - Universita' di Bologna
//              Shankar Mahadevan - sm@imm.dtu.dk
//              Rasmus Grondahl Olsen - rasmus_olsen@bigfoot.com
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
// info         Generic OCP slave
//
///////////////////////////////////////////////////////////////////////////////

#include "ocp_slave.h"

void ocp_slave::receive()
{
	sc_uint<MADDRWD> address;
	sc_uint<SDATAWD> data;
	sc_uint<MBURSTLENGTHWD> blen;
	sc_uint<MBYTEENWD> byteen;
	uint wcycles;
	bool is_last;
	
	while (true)
	{
		if (reset.read())
		{
			SData.write(0xdeadbeef);
			SCmdAccept.write(false);
			SDataAccept.write(false);
			SResp.write(OCPSRESNULL);
			SRespLast.write(false);
			wait();
		}
		else
		{
			SData.write(0xdeadbeef);
			SCmdAccept.write(false);
			SDataAccept.write(false);
			SResp.write(OCPSRESNULL);
			SRespLast.write(false);

			// Word, half word or byte?
			byteen = MByteEn.read();
			// FIXME assumes a fixed set
			// FIXME we should sample things while we keep SCmdAccept already high...
			if ((MCmd.read() != OCPCMDIDLE) && ((uint)byteen != OCPMBYEWORD) && ((uint)byteen != OCPMBYEBYTE) && ((uint)byteen != OCPMBYEHWRD))
			{
				printf("Fatal error: %s %u detected a malformed data size at %10.1f\n",
					type, ID, sc_simulation_time());
				exit(1);
			}
			is_last = MReqLast.read();
			address = MAddr.read();
                        
			if (0) //FIXME
				wcycles = mem_in_ws;
			else
				wcycles = mem_bb_ws;
			
			switch (MCmd.read())
			{
				// ------------
				// Posted write
				// ------------
				case OCPCMDWRITE:
					SCmdAccept.write(true);
					SDataAccept.write(true);
					wait();
					SCmdAccept.write(false); // TODO non-sreq burst writes
					
					// Is this a Single Request / Precise Burst Write, or just a write?
					if (MBurstSingleReq.read() || MBurstPrecise.read())
						blen = MBurstLength.read();
					else
						blen = 1;
						
					for (unsigned int i = 0; i < blen; i ++)
					{
						if (!MDataValid.read() && blen != 1) // FIXME even a single-beat write could have datahandshake...
						{
							do
								wait();
							while (!MDataValid.read());
						}
						
						data = MData.read();
						
						SDataAccept.write(false);
						
						if (i == 0 && wcycles > 0x1)
							wait(wcycles - 1);
						else if (i != 0 && wcycles > 0x0)
							wait(wcycles);
						
						this->Write((uint32_t)address, (uint32_t)data, (uint8_t)byteen);
						
						TRACEX(TARGET_TRACEX, 10, "%s %u:        P write  %8x at address 0x%08x | time: %10.1f\n",
							type, ID, (uint32_t)data, (uint32_t)address, sc_simulation_time());
						
						wait();
						
						SDataAccept.write(true);
					}
					break;
				
				// ----
				// Read
				// ----
				case OCPCMDREAD:
					
					SCmdAccept.write(true);
					SDataAccept.write(true);
					wait();
					SCmdAccept.write(false);
					SDataAccept.write(false);
					
					// Is this a Single Request Burst Read, or just a read?
					if (MBurstSingleReq.read() && MBurstPrecise.read())
						blen = MBurstLength.read();
					else
						blen = 0x1;
						
					for (unsigned int i = 0; i < blen; i ++)
					{
						if (i == 0 && wcycles > 0x1)
							wait(wcycles - 1);
						else if (i != 0 && wcycles > 0x0)
							wait(wcycles);
						
						data = this->Read(address + i * 4);
						
						TRACEX(TARGET_TRACEX, 10, "%s %u:        read     %8x at address 0x%08x | time: %10.1f\n",
							type, ID, (uint32_t)data, (uint32_t)address + blen * 4, sc_simulation_time());
						
						SData.write(data);
						SResp.write(OCPSRESDVA);
						if ((blen > 0x1 && i == blen - 1) || (blen == 0x1 && is_last))
							SRespLast.write(true);
						
						do
							wait();
						while (!MRespAccept.read());
						
						SData.write(0xdeadbeef);
						SResp.write(OCPSRESNULL);
						SRespLast.write(false);
					}
					
					break;
					
				// ----------------
				// Non-posted write
				// ----------------
				case OCPCMDWRNP: //TODO bursts of WRNP
					data = MData.read();
					
					SCmdAccept.write(true);
					SDataAccept.write(true);
					wait();
					SCmdAccept.write(false);
					SDataAccept.write(false);
					
					// Is this a Single Request Burst WRNP, or just a WRNP?
					if (MBurstSingleReq.read() && MBurstPrecise.read())
						blen = MBurstLength.read();
					else
						blen = 0x1;
					
					for (unsigned int i = 0; i < blen; i ++)
					{
						if (i == 0 && wcycles > 0x1)
							wait(wcycles - 1);
						else if (i != 0 && wcycles > 0x0)
							wait(wcycles);
					
						this->Write((uint32_t)address, (uint32_t)data, (uint8_t)byteen);
						TRACEX(TARGET_TRACEX, 10, "%s %u:        NP write %8x at address 0x%08x | time: %10.1f\n",
							type, ID, (uint32_t)data, (uint32_t)address, sc_simulation_time());
					
						SResp.write(OCPSRESDVA);
						if ((blen > 0x1 && i == blen - 1) || (blen == 0x1 && is_last))
							SRespLast.write(true);
						
						do
							wait();
						while (!MRespAccept.read());
						
						SResp.write(OCPSRESNULL);
						SRespLast.write(false);
					}
					
					break;
					
				// ----
				// Idle
				// ----
				case OCPCMDIDLE:
					SResp.write(OCPSRESNULL);
					SRespLast.write(false);
					wait();
					
					break;
					
				// -----------
				// Unsupported
				// -----------
				default:
					printf("Fatal error: %s: Trace requested for an invalid transaction type\n",
						name());
					exit(1);
					
					break;
			} // end switch
		} // end if reset
	} // end while(true)
} // end process


void ocp_slave::issue_interrupts()
{
	while (true)
	{
		if (reset.read())
		{
			SInterrupt.write(false);
			SFlag.write(0x00000000);
			wait();
		}
		else
		{
			if (0)
			{
				SInterrupt.write(true);
				SFlag.write(0xdead);
                                //printf("Sent int by %s at %7.1f\n", name(), sc_simulation_time());
				do
					wait();
				while (!MFlag.read());
				SInterrupt.write(false);
				SFlag.write(0x0000);
				wait();
			}
			else
				wait();
		} // end if reset
	} // end while(true)
} // end process
