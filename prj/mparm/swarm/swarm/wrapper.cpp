///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         wrapper.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Wraps a C++ ARM ISS in a SystemC envelope
//
///////////////////////////////////////////////////////////////////////////////

#include "wrapper.h"
#include "globals.h"
#include "address.h"
#include <fcntl.h>
#include <string.h>
  #ifdef WIN32
  #include <IO.h>
  #else
  #include <unistd.h>
  #endif
#include <iostream>
#include <sys/stat.h>
#include "syscopro.h"
#include "user_swi.h"
#include "swi_calls.h"
#include <stdlib.h>
#include "stats.h"
#include "sim_support.h"

#ifdef DEBUGGER
///////////////////////////////////////////////////////////////////////////////
// DebugMemoryDump - Provides the ability to dump the contents of the system
//                   memories.
void armsystem::DebugMemoryDump(unsigned int address, const char *target, uint16_t TargetID)
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
// startsimul - ARM wrapping loop. Continuously cycles the processor (in sync with
//              the SystemC clock) and monitors its pinout looking for external
//              accesses.
void armsystem::startsimul()
{
  int* intforw; // intforw: external interrupts not yet forwarded to armproc
  uint8_t burst;
  PINOUT pinout;
  double pwr;

#ifdef DEBUGGER
  long long int DebuggerRepeatCount = 0;
  uint16_t TargetID;
  uint32_t DebuggerPrevPC = 999, DebuggerBreakPoint = 0;
  bool DebuggerMoreRequest, alreadycaught = false;
  char DebuggerRequest[64], sDebuggerBreakPoint[64]; // user input
#endif

  intforw = (int*) calloc(NUMBER_OF_EXT,sizeof(int));

  request_to_master[0].write(false);
  if (DMA)
    request_to_master[1].write(false);

  // Let me use mixed IO
  ios::sync_with_stdio();

  // Register SWI calls, assuming availability of processor ID
  pArm->RegisterSWI(SWI_PRINT, print_swi_call);
  pArm->RegisterSWI(SWI_EXIT, exit_swi_call);
  pArm->RegisterSWI(SWI_METRIC_START, metric_start_swi_call);
  pArm->RegisterSWI(SWI_METRIC_STOP, metric_stop_swi_call);
  pArm->RegisterSWI(SWI_METRIC_DUMP, metric_dump_swi_call);
  pArm->RegisterSWI(SWI_METRIC_DUMPL, metric_dump_light_swi_call);
  pArm->RegisterSWI(SWI_METRIC_CLEAR, metric_clear_swi_call);
  pArm->RegisterSWI(SWI_CORE_GO_IDLE, core_go_idle_swi_call);
 
  // Setup the bus safely
  pinout.fiq = 1;
  pinout.irq = 1;
  pinout.address = 0;
  pinout.rw = 0;

  while(1)
  {
      
      for(uint i=0;i<NUMBER_OF_EXT;i++)
      {
       if (extint[i].read()) 
       {
        intforw[i]++;
        extint[i].write(false);
       }
      } 

      // Cycle the ARM
      //printf("wrapper0x%08x,%d\n",intforw,ID);
      pArm->Cycle(&pinout, intforw);
      for(uint i=0;i<NUMBER_OF_EXT;i++)
      if (intforw[i])
      {
#ifdef PRINTDEBUG
        printf("Sent int to armproc:%d||%d\n",ID,i);
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
			printf("SWARM %d Debugger (h for help) [0x%x]> ", ID, DebuggerPrevPC);
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
	if (pinout.benable == 1)
	{
		// Is this a simulation support access?
		if (addresser->PhysicalInSimSupportSpace(addresser->Logical2Physical(pinout.address, ID)))
		{
		  simsuppobject->catch_sim_message(pinout.address - addresser->ReturnSimSupportPhysicalAddress(),
		    &pinout.data, pinout.rw, ID);
		  wait();
		}
		
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
		    
		    for(uint i=0;i<NUMBER_OF_EXT;i++)
		    {
		      if (extint[i].read()) 
		      {
		        intforw[i]++;
		        extint[i].write(false);
		      }
		    }
		    // The core is blocked waiting for the bus. Notify the statistics module!
		    // Most call arguments are pointless and thus randomly set, except the mode
		    // (P_BLOCKED) and the ID
		    if (STATS && !ready_from_master[1].read())
		      statobject->inspectSWARMAccess(0x0, P_BLOCKED, false, false, ID);
		  } while (!ready_from_master[1].read());
		  
		  pinout = pinout_ft_master[1].read();
		  request_to_master[1].write(false);
		  
		  if (POWERSTATS)
		  {
		    pwr = powerDMA(ID, INTERNAL_DMA_SIZE, 32, (pinout.rw==0)?READop:WRITEop);
		    statobject->inspectDMAprogramAccess(pinout.address,(pinout.rw==0)?true:false,pwr, ID);
		  }
		}
		
		else // Normal L2 memory access
		{
		  // Pass ARM pinout to master and assert request
		  pinout_ft_master[0].write(pinout);
		  request_to_master[0].write(true);
		  burst = pinout.burst;
		  // Fake OCP statistics to be able to compare latencies even when not using OCP
		  if (STATS)
		    statobject->putOCPTransactionCommand(pinout.rw ? OCPCMDWRITE : OCPCMDREAD, burst, ID);
		  
		  for (uint burstcounter = 0; burstcounter < burst; burstcounter++)
		  {
		    // Cycle the ARM, but not during the first iteration (already done).
		    // Don't pass external interrupts to processor (wouldn't be collected during bursts)
		    // and don't pass new processor pinout to bus master (meaningless during bursts)
		    if (burstcounter)
		      pArm->Cycle(&pinout, 0);
		    // the following loop explicits a wait_until(readymaster.delayed()), checking for interrupts
		    do
		    {
		      wait();
		      
		      for(uint i=0;i<NUMBER_OF_EXT;i++)
		      {
		        if (extint[i].read()) 
		        {
		          intforw[i]++;
		          extint[i].write(false);
		        }
		      }
		      // The core is blocked waiting for the bus. Notify the statistics module!
		      // Most call arguments are pointless and thus randomly set, except the mode
		      // (P_BLOCKED) and the ID
		      if (STATS && !ready_from_master[0].read())
		        statobject->inspectSWARMAccess(0x0, P_BLOCKED, false, false, ID);
		    } while (!ready_from_master[0].read());
		    pinout = pinout_ft_master[0].read();
		    if (pinout.rw == 0)
		    {
		      TRACEX(ISS_ACC_TRACEX, 10, "Processor %u received:  %8x from address   %8x\n",
		        ID, pinout.data, (pinout.address + burstcounter * 4));
		      // Fake OCP statistics to be able to compare latencies even when not using OCP
		      if (STATS && burstcounter == 0)
		        statobject->getOCPTransactionCommandAccept(OCPCMDREAD, burst, ID);
		      if (STATS && burstcounter + 1 == burst)
		        statobject->getOCPTransactionResp(OCPCMDREAD, burst, ID);
		    }
		    else
		    {
		      TRACEX(ISS_ACC_TRACEX, 10, "Processor %u asserted:  %8x to address   %8x\n",
		        ID, pinout.data, pinout.address);
		      // Fake OCP statistics to be able to compare latencies even when not using OCP
		      if (STATS && burstcounter == 0)
		        statobject->getOCPTransactionCommandAccept(OCPCMDWRITE, burst, ID);
		    }
		  }
		  request_to_master[0].write(false);                
		}// end of L2 access
	}//end of bus access
	else
	  wait(); // only if no access made, wait one clock before cycling the ARM again
    }//end of while
}
