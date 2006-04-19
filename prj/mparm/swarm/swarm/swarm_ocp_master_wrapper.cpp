///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         swarm_ocp_master_wrapper.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Wraps a C++ ARM ISS in a SystemC OCP envelope
//
///////////////////////////////////////////////////////////////////////////////

#include "swarm_ocp_master_wrapper.h"
#include "sim_support.h"
#include "stats.h"

//#define USE_WRITENONPOSTED
// Please note: USE_READBURSTSINGLEREQ is REQUIRED for TG tracing
#define USE_READBURSTSINGLEREQ

#ifdef DEBUGGER
///////////////////////////////////////////////////////////////////////////////
// DebugMemoryDump - Provides the ability to dump the contents of the system
//                   memories.
void ocpswarm::DebugMemoryDump(unsigned int address, const char *target, uint16_t TargetID)
{
  int i,j;
  char *mdebug = 0;

  printf("\n");
  if ((N_SHARED > 0) && (*target == 's'))
  {
    if (!addresser->IsShared(TargetID) || address >= addresser->ReturnSharedSize(TargetID))
    {
      printf("Fatal error: Device ID %hu does not match any shared memory, or address 0x%08x out of bounds!\n",
        TargetID, address);
      exit(1);
    }
    mdebug = addresser->pMemoryDebug[TargetID];
    printf("Shared Memory %hu Base Address 0x%08x - Debugging 0x%08x\n", TargetID, (uint)addresser->pMemoryDebug[TargetID], address);
  }
  else if (SCRATCH && (*target == 'e'))
       {
         if (address >= addresser->ReturnScratchSize())
         {
           printf("Fatal error: Address 0x%08x out of bounds!\n", address);
           exit(1);
         }
         mdebug = addresser->scratchMemoryDebug[TargetID];
         printf("Scratchpad Memory %hu Base Address 0x%08x - Debugging 0x%08x\n", TargetID, (uint)addresser->scratchMemoryDebug[TargetID], address);
       }
       else if ((N_SEMAPHORE > 0) && (*target == 'l'))
            {
              if (!addresser->IsSemaphore(TargetID) || address >= addresser->ReturnSemaphoreSize(TargetID))
              {
                printf("Fatal error: Device ID %hu does not match any semaphore memory, or address 0x%08x out of bounds!\n",
                  TargetID, address);
                exit(1);
              }
	      mdebug = addresser->pMemoryDebug[TargetID];
	      printf("Semaphore Memory %hu Base Address 0x%08x - Debugging 0x%08x\n", TargetID, (uint)addresser->pMemoryDebug[TargetID], address);
            }
            else if (*target == 'p')
                 {
                   if (!addresser->IsPrivate(TargetID)  || address >= addresser->ReturnPrivateSize())
                   {
                     printf("Fatal error: Device ID %hu does not match any private memory, or address 0x%08x out of bounds!\n",
                       TargetID, address);
                     exit(1);
                   }
                   mdebug = addresser->pMemoryDebug[TargetID];
                   printf("Private Memory %hu Base Address 0x%08x - Debugging 0x%08x\n", TargetID, (uint)addresser->pMemoryDebug[TargetID], address);
                 }

  address &= 0xFFFFFFF0;
  
  for (j = 0; j < 8; ++j, address += 16)
  {
    if (*target == 'p' && address >= addresser->ReturnPrivateSize())
      break;
    if (*target == 's' && address >= addresser->ReturnSharedSize(TargetID))
      break;
    if (*target == 'l' && address >= addresser->ReturnSemaphoreSize(TargetID))
      break;
    if (*target == 'e' && address >= addresser->ReturnScratchSize())
      break;
    printf("%08x: ", address);
    for (i = 0; i < 16; ++i)
    printf("%02x ", ((unsigned char *)mdebug)[address+i]);
    for (i = 0; i < 16; ++i)
      if (((unsigned char *)mdebug)[address+i] >= 0x20)
        printf("%c", mdebug[address+i]);
      else
        printf(".");
    printf("\n");
  }
}
#endif


///////////////////////////////////////////////////////////////////////////////
// request - ARM wrapping loop. Continuously cycles the processor (in sync with
//           the SystemC clock) and monitors its pinout looking for external
//           accesses.
void ocpswarm::request()
{
  
#ifdef DEBUGGER
  long long int DebuggerRepeatCount = 0;
  uint16_t TargetID;
  uint32_t DebuggerPrevPC = 999, DebuggerBreakPoint = 0;
  bool DebuggerMoreRequest, alreadycaught = false;
  char DebuggerRequest[64], sDebuggerBreakPoint[64]; // user input
#endif

  // #ifndef OLD_INT_STYLE
  int_caught = false;
  MFlag[0].write(0);
  // #endif

  uint32_t addr;

  MCmd[0].write(OCPCMDIDLE);
  //FIXME Still unsupported
/*  if (DMA)
    request_to_master[1].write(false);*/

  // Let me use mixed IO
  ios::sync_with_stdio();

  // Register SWI calls, assuming availability of processor ID
  pArm->RegisterSWI(SWI_PRINT, print_swi_call);
  pArm->RegisterSWI(SWI_EXIT, exit_swi_call);
  pArm->RegisterSWI(SWI_METRIC_START, metric_start_swi_call);
  pArm->RegisterSWI(SWI_METRIC_STOP, metric_stop_swi_call);
  pArm->RegisterSWI(SWI_METRIC_DUMP, metric_dump_swi_call);
  
  // Setup the bus safely
  pinout.fiq = 1;
  pinout.irq = 1;
  pinout.address = 0;
  pinout.rw = 0;
  
  cmd_type = OCPCMDREAD;

  while(true)
  {
#ifdef OLD_INT_STYLE
      for(int i=0;i<NUMBER_OF_EXT;i++)
      {
       if (extint[i].read()) 
       {
        intforw[i]++;
        //printf("Wrapper %s detected an int (%d) @ %7.1f\n", name(), i, sc_simulation_time());
        extint[i].write(false);
       }
      }
#else
      if (CURRENT_INTERC == XPIPES)
      {
        for(int i=0;i<NUMBER_OF_EXT;i++)
        {
          if (extint[i].read()) 
          {
            intforw[i]++;
            //printf("Wrapper %s detected an int (%d) @ %7.1f\n", name(), i, sc_simulation_time());
            extint[i].write(false);
          }
        }
      }
      else
      {
        if (SInterrupt[0].read())
        {
          if (!int_caught)
          {
            int_mask = SFlag[0].read();
            //if (int_mask)
            //  printf("Wrapper %s detected an int (mask = 0x%x) @ %7.1f\n", name(), (uint32_t)int_mask, sc_simulation_time());
            for (int i = 0; i < NUMBER_OF_EXT; i ++)
              if (int_mask[i])
                intforw[i]++;
            MFlag[0].write(1);
            int_caught = true;
          }
        }
        else
        {
          MFlag[0].write(0);
          int_caught = false;
        }
      }
#endif

      // Cycle the ARM
      pArm->Cycle(&pinout, intforw);
      for(int i=0;i<NUMBER_OF_EXT;i++)
      if (intforw[i])
      {
#ifdef PRINTDEBUG
        printf("Sent int to armproc %d\n", ID);
#endif
        intforw[i]--;
      }


#ifdef DEBUGGER
	if(DebuggerPrevPC != (unsigned int)pArm->NextPC())
	{
	  DebuggerPrevPC = (unsigned int)pArm->NextPC();

	  // Check for breakpoints; if found, set an execution count boundary to exactly this cycle.
	  // The check has to look for a range of addresses due to anomalies nearby jumps.
	  // The flag "alreadycaught" avoids multiple pauses at a single breakpoint
	  if( (DebuggerPrevPC >= (DebuggerBreakPoint)) && (DebuggerPrevPC <= (DebuggerBreakPoint+0x8)) && !alreadycaught)
	  {
	    DebuggerRepeatCount = 0;
	    alreadycaught = true;
	  }

	  if(DebuggerRepeatCount == 0)    // Reached an execution count boundary (might be a breakpoint)
	  {
		DebuggerRepeatCount = 1;  // Set a new default execution count boundary one cycle from now

		do  // Ask at least once the user what to do
		{
			printf("SWARM %d Debugger[0x%x]> ", ID, DebuggerPrevPC);
			cin >> DebuggerRequest;
			DebuggerMoreRequest = 0;    // By default, ask only once (but will depend on user choice)

			if(DebuggerRequest[0] == 'q')
				exit(1);
			else if(DebuggerRequest[0] == 'd')
			{
				pArm->DebugDumpCore();
			}
			else if(DebuggerRequest[0] == 'D')
			{
				pArm->DebugDump();
				DebuggerMoreRequest = 1;
			}
			else if(DebuggerRequest[0] == 'm')
			{
				cout << "Enter the Private address (as 0x....): ";
				cin >> sDebuggerBreakPoint;
				DebuggerBreakPoint = strtoul(sDebuggerBreakPoint, NULL, 0);
				if (DebuggerBreakPoint < addresser->ReturnPrivateSize())
				  DebugMemoryDump(DebuggerBreakPoint, "p", ID);
				else
				{
				  cout << "Error in entering the address. Addresses must be <= 0x" << hex <<
				    addresser->ReturnPrivateSize() - 1 << "!" << endl;
				}
				DebuggerMoreRequest = 1;
			}
			else if((N_SHARED > 0) && (DebuggerRequest[0] == 's'))
			{
				if (N_SHARED > 1)
				{
				  cout << "Enter the Shared ID: ";
				  cin >> TargetID;
				}
				else
				  TargetID = N_PRIVATE;
				cout << "Enter the Shared address (as 0x....): ";
				cin >> sDebuggerBreakPoint;
				DebuggerBreakPoint = strtoul(sDebuggerBreakPoint, NULL, 0);
				if (addresser->IsShared(TargetID) && DebuggerBreakPoint < addresser->ReturnSharedSize(TargetID))
				  DebugMemoryDump(DebuggerBreakPoint, "s", TargetID);
				else if (addresser->IsShared(TargetID))
				{
				  cout << "Error in entering the address. Addresses must be <= 0x" << hex <<
				    addresser->ReturnSharedSize(TargetID) - 1 << "!" << endl;
				}
				else
				{
				  cout << "Error in entering the ID. Valid IDs range from " << addresser->SharedStartID() <<
				    " to " << N_PRIVATE + N_SHARED - 1 << "!" << endl;
				}
				DebuggerMoreRequest = 1;
			}
			else if((N_SEMAPHORE > 0) && (DebuggerRequest[0] == 'l'))
			{
				if (N_SEMAPHORE > 1)
				{
				  cout << "Enter the Semaphore ID: ";
				  cin >> TargetID;
				}
				else
				  TargetID = N_PRIVATE + N_SHARED;
				cout << "Enter the Semaphore address (as 0x....)): ";
				cin >> sDebuggerBreakPoint;
				DebuggerBreakPoint = strtoul(sDebuggerBreakPoint, NULL, 0);
				if (addresser->IsSemaphore(TargetID) && DebuggerBreakPoint < addresser->ReturnSemaphoreSize(TargetID))
				  DebugMemoryDump(DebuggerBreakPoint, "l", TargetID);
				else if (addresser->IsSemaphore(TargetID))
				{
				  cout << "Error in entering the address. Addresses must be <= 0x" << hex <<
				    addresser->ReturnSemaphoreSize(TargetID) - 1 << "!" << endl;
				}
				else
				{
				  cout << "Error in entering the ID. Valid IDs range from " << addresser->SemaphoreStartID() <<
				    " to " << N_PRIVATE + N_SHARED + N_SEMAPHORE - 1 << "!" << endl;
				}
				DebuggerMoreRequest = 1;
			}
			else if(SCRATCH && (DebuggerRequest[0] == 'e'))
			{
				cout << "Enter the Scratch address (as 0x....): ";
				cin >> sDebuggerBreakPoint;
				DebuggerBreakPoint = strtoul(sDebuggerBreakPoint, NULL, 0);
				if (DebuggerBreakPoint < addresser->ReturnScratchSize())
				  DebugMemoryDump(DebuggerBreakPoint, "e", ID);
				else
				{
				  cout << "Error in entering the address. Addresses must be <= 0x" << hex << addresser->ReturnScratchSize() - 1 << "!" << endl;
				}
				DebuggerMoreRequest = 1;
			}
			else if(DebuggerRequest[0] == 'c')    // by setting a negative count, execution will not
				DebuggerRepeatCount = -1;     // be stopped any more (except in case of breakpoints)
			else if(DebuggerRequest[0] == '0')
				DebuggerRepeatCount = 1;
			else if(DebuggerRequest[0] == '1')
				DebuggerRepeatCount = 10;
			else if(DebuggerRequest[0] == '2')
				DebuggerRepeatCount = 100;
			else if(DebuggerRequest[0] == '3')
				DebuggerRepeatCount = 1000;
			else if(DebuggerRequest[0] == '4')
				DebuggerRepeatCount = 10000;
			else if(DebuggerRequest[0] == 'b')
			{
				cout << "Enter the breakpoint (in hex, starting with 0x): ";
				cin >> sDebuggerBreakPoint;
				DebuggerBreakPoint = strtoul(sDebuggerBreakPoint,NULL,0);
				printf("Setting breakpoint to: 0x%08x\n", DebuggerBreakPoint);
				alreadycaught = false;
				DebuggerMoreRequest = 1;
			}
			else if( (DebuggerRequest[0] == 'h') || (DebuggerRequest[0] == '?') )
			{
				cout << "Supported commands: \n";
				cout << " c   - Continue execution without any more debugging\n";
				cout << " d   - Dump processor core status and execute 1 instruction\n";
				cout << " D   - Dump status of full processor\n";
				cout << " b   - Set breakpoint\n";
				cout << " m   - Dump Private memory\n";
				if (N_SHARED > 0)
				  cout << " s   - Dump Shared memory\n";
				if (N_SEMAPHORE > 0)
				  cout << " l   - Dump Semaphore memory\n";
				if (SCRATCH)
				  cout << " e   - Dump Scratch memory\n";
				cout << " 0   - Execute 1 instruction \n";
				cout << " 1   - Execute 10 instructions \n";
				cout << " 2   - Execute 100 instructions \n";
				cout << " 3   - Execute 1000 instructions \n";
				cout << " 4   - Execute 10000 instructions \n";
				cout << " h/? - Help\n";
				cout << " q   - Quit\n";
				DebuggerMoreRequest = 1;
			}
		} while(DebuggerMoreRequest);
	  }
	  DebuggerRepeatCount--;
	}
#endif //DEBUGGER

	// Do we need to do anything with the bus?
	if (pinout.benable)
	{
		// Is this a simulation support access?
		if (addresser->PhysicalInSimSupportSpace(addresser->Logical2Physical(pinout.address, ID)))
		{
		  simsuppobject->catch_sim_message(pinout.address - addresser->ReturnSimSupportPhysicalAddress(),
		    &pinout.data, pinout.rw, ID);
		  wait();
		}
#if 0
//FIXME: DMA unsupported yet
		// Is this an access to the DMA?
		else if (addresser->PhysicalInDMASpace(addresser->Logical2Physical(pinout.address, ID)))
		{
		  // Pass ARM pinout to DMA and assert request
		  pinout_ft_master[1].write(pinout);
		  request_to_master[1].write(true);
		  // The following loop explicits a wait_until(readydma.delayed()), checking for interrupts
		  do
		  {
		    wait();
      		    for(int i=0;i<NUMBER_OF_EXT;i++)
            	    {
                     if (extint[i].read()) 
                     {
                      intforw[i]++;
                      extint[i].write(false);
                     }
                    }		    
		  } while (!ready_from_master[1].read());
		  pinout = pinout_ft_master[1].read();
		  request_to_master[1].write(false);
		}

#endif
		else // Normal L2 memory access
		{
		  // Pass ARM pinout to master and assert request
		  if (pinout.rw == 1)
		  {
#ifdef USE_WRITENONPOSTED
		    MCmd[0].write(OCPCMDWRNP);
		    cmd_type = OCPCMDWRNP;
#else
		    MCmd[0].write(OCPCMDWRITE);
		    cmd_type = OCPCMDWRITE;
#endif
		    MDataLast[0].write(true);  //FIXME datahandshake
		    MData[0].write(pinout.data);
		  }
		  else
		  {
		    MCmd[0].write(OCPCMDREAD);
		    cmd_type = OCPCMDREAD;
		    MDataLast[0].write(false);  //FIXME datahandshake
		    MData[0].write(0xdeadbeef);
		  }
		  if (STATS)
		    statobject->putOCPTransactionCommand(cmd_type, pinout.burst, ID);
		  MAddrSpace[0].write(0);
		  burst = pinout.burst;
		  switch (pinout.bw)
		  {
		    case 0 :  MByteEn[0].write(OCPMBYEWORD);
		              break;
		    case 1 :  MByteEn[0].write(OCPMBYEBYTE);
		              break;
		    case 2 :  MByteEn[0].write(OCPMBYEHWRD);
		              break;
		    default : printf("Fatal error: Detected malformed data size at time %10.1f\n",
		                sc_simulation_time());
		              exit(1);
		  }
		  
		  MBurstPrecise[0].write(true);
		  MBurstSeq[0].write(OCPMBSINCR);
		  MBurstLength[0].write(burst);
		  MReqLast[0].write(false);
#ifdef USE_READBURSTSINGLEREQ
		  if (pinout.rw == 0 && burst != 0x1)
		  {
		    MBurstSingleReq[0].write(true);
		    addr = addresser->Logical2Physical(pinout.address, ID);
		    MAddr[0].write(addr);
		    MReqLast[0].write(true);
		    MDataValid[0].write(true);
		    do
		    {
		      wait();
		      // The core is blocked waiting for the bus. Notify the statistics module!
		      // Most call arguments are pointless and thus randomly set, except the mode
		      // (P_BLOCKED) and the ID
		      if (STATS)
		        statobject->inspectSWARMAccess(0x0, P_BLOCKED, false, false, ID);
		    }
		    while (!SCmdAccept[0].read());
		    if (STATS)
		      statobject->getOCPTransactionCommandAccept(OCPCMDREAD, burst, ID);
		  }
		  else
#endif
		  {
		    MBurstSingleReq[0].write(false);
		    
		    for (uint burstcounter = 0; burstcounter < burst; burstcounter ++)
		    {
		      if (pinout.rw == 1)
		        TRACEX(ISS_ACC_TRACEX, 10, "Processor %u asserted:  %8x to address   %8x\n",
		          ID, pinout.data, pinout.address);
		
		      addr = addresser->Logical2Physical(pinout.address, ID);
		
		      MAddr[0].write(addr + burstcounter * 4);
		
		      if ((int)burstcounter == burst - 1)
		        MReqLast[0].write(true);
		
		      MDataValid[0].write(true);
		  
		      do
		      {
		        wait();
		        // The core is blocked waiting for the bus. Notify the statistics module!
		        // Most call arguments are pointless and thus randomly set, except the mode
		        // (P_BLOCKED) and the ID
#ifndef USE_WRITENONPOSTED
		        if (!SCmdAccept[0].read() || pinout.rw == 0)
#endif
		          if (STATS)
		            statobject->inspectSWARMAccess(0x0, P_BLOCKED, false, false, ID);
		      }
		      while (!SCmdAccept[0].read());
		      if (STATS && burstcounter == 0)
		        statobject->getOCPTransactionCommandAccept(cmd_type, burst, ID); //FIXME not just for writes, not always single...
		    }
		  }
		  
		  // The request was asserted and accepted. Now deassert it and wait
		  // for the response channel to free us up for new requests
		  MCmd[0].write(OCPCMDIDLE);
		  MBurstSingleReq[0].write(false);
		  MReqLast[0].write(false);
		  MDataValid[0].write(false);
		  
		  // Wait for responses if it is a read...  //FIXME. Might be done better?
		  if (pinout.rw == 0)
		    for (uint burstcounter = 0; burstcounter < burst; burstcounter++)
		      do
		      {
		        wait();
		        // The core is blocked waiting for the bus. Notify the statistics module!
		        // Most call arguments are pointless and thus randomly set, except the mode
		        // (P_BLOCKED) and the ID
		        if (STATS && SResp[0].read() != OCPSRESDVA && !(burstcounter + 1 == burst && go.read()))
		          statobject->inspectSWARMAccess(0x0, P_BLOCKED, false, false, ID);
		      } while (!go.read());
		  
		  // ...or a non-posted write
#ifdef USE_WRITENONPOSTED
		  if (pinout.rw == 1)
		  {
		    for (uint burstcounter = 0; burstcounter < burst; burstcounter++)
		      do
		      {
		        wait();
		        // The core is blocked waiting for the bus. Notify the statistics module!
		        // Most call arguments are pointless and thus randomly set, except the mode
		        // (P_BLOCKED) and the ID
		        if (STATS && !(burstcounter + 1 == burst && go.read()))
		          statobject->inspectSWARMAccess(0x0, P_BLOCKED, false, false, ID);
		      } while (!go.read());
		  }
#endif
		}// end of L2 access
	}//end of bus access
	else
	  wait(); // only if no access made, wait one clock before cycling tha ARM again
    }//end of while
}


///////////////////////////////////////////////////////////////////////////////
// receive - ARM wrapping loop. Continuously looks for responses to transactions
//           initiated by the ARM
void ocpswarm::receive()
{
  go.write(false);
  MRespAccept[0].write(true);
  
  while(true)
  {
    // wait until(SResp.read() == OCPSRESDVA), checking for interrupts in the meanwhile
    do
    {
      wait();

      if (go.read())
        go.write(false);
      
      if (CURRENT_INTERC == XPIPES)
      {
        for (int i = 0; i < NUMBER_OF_EXT; i ++)
        {
          if (extint[i].read()) 
          {
            intforw[i]++;
            //printf("Wrapper %s detected an int (%d) @ %7.1f\n", name(), i, sc_simulation_time());
            extint[i].write(false);
          }
        }
      }
      else
      {
        //FIXME recheck interrupt operation...
        if (!SInterrupt[0].read())
        {
          MFlag[0].write(0);
          int_caught = false;
        }
      }
    } while (SResp[0].read() != OCPSRESDVA);
    
    if (STATS && SRespLast[0].read())
      statobject->getOCPTransactionResp(cmd_type, burst, ID);
    
    // Notice: we don't perform any check on byte enables, since SWARM won't mind about them
    // on reads or responses
    // Notice: we blindly put data on pinout even on writes, since SWARM won't reread that
    if (cmd_type == OCPCMDWRNP)
      TRACEX(ISS_ACC_TRACEX, 10, "Processor %u received:  CONFIRMD from address   %8x\n", ID, pinout.address);
    else
    {
      pinout.data = SData[0].read();
      TRACEX(ISS_ACC_TRACEX, 10, "Processor %u received:  %8x from address   %8x\n", ID, pinout.data, pinout.address);
      pArm->Cycle(&pinout, 0);
    }
    
    go.write(true);
  }
}
