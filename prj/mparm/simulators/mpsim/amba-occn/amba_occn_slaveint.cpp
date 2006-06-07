///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_occn_slaveint.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         AMBA (OCCN) interrupt slave
//
///////////////////////////////////////////////////////////////////////////////

#include "amba_occn_slaveint.h"

///////////////////////////////////////////////////////////////////////////////
// receive - Listens to the bus waiting for requests. When one is received,
//           it is forwarded to the memory module (a C++ class so to be able to
//           do 0-WS operation), then a configurable number of wait states is
//           inserted and the answer is put on the bus.

#define KEY 7
#define MOVE 3

void amba_occn_slaveint::receive()
{
   Pdu<AhbMasterCtrl,N_int32> *pk;
   Pdu<AhbSlaveCtrl,N_int32>  *ret_pk = new Pdu<AhbSlaveCtrl,N_int32>;
  
  
   int intno=0;
   
    printf("Interrupt Slave %hu  - Size: 0x%08x, Base Address: 0x%08x\n\n", my, addresser->ReturnInterruptSize(), addresser->ReturnSlavePhysicalAddress(my));

  for(int i=0; i<N_CORES; i++)
    extint[i].write(false);
    
   do {

        AhbArb::HTRANS  tra_tmp;
        AhbArb::HBURST bur_tmp;
        
        unsigned int addr_tmp;
        N_uint step_tmp;
         

        pk=port.receive();
        
        step_tmp=occn_hdr(*pk,step);
        // AST-Grenoble: Slave computes step that determines current operation
        step_tmp &=(KEY<< (MOVE*shift));

        step_tmp >>=(MOVE*shift);

        op=(operation) step_tmp;
        
        tra_tmp=occn_hdr(*pk,htrans);
        bur_tmp=occn_hdr(*pk,hburst);
        
        

     // AST-Grenoble: Interrupt operation,only write transfer  allowed

        switch(op){
            case HREADY:
               // AST-Grenoble: hready response,first stage in pipeline
                tra_tmp=occn_hdr(*pk,htrans);

                for(int i=INT_WS; i>0; i--)
        		wait(clock.posedge_event());
                occn_hdr(*ret_pk,hready)=1;
                occn_hdr(*ret_pk,hresp)=Response::OKAY;
                port.reply();                          
                port.asend(ret_pk);
                break;
                
            case HREWR:
               // AST-Grenoble: hready response,sending interrupt to a given Processor
            	tra_tmp=occn_hdr(*pk,htrans);                                
                
                addr_tmp=occn_hdr(*pk,haddr_write_old);
                intno = (addresser->Logical2Base(addresser->Physical2Logical(addr_tmp, my)) / 4) - 1;
                if (intno < 0 || intno >= N_CORES)
                {
                  printf("%s: Fatal Error: cannot send interrupt on wire %d!\n", name(), intno);
                  exit(1);
                }
                    
                extint[intno].write (true);
                      
                #ifdef OCCN_DEBUG                         
                 cout << "Time for interrupt: " << sc_time_stamp() << endl;
                #endif

	        occn_hdr(*ret_pk,hready)=1;
                occn_hdr(*ret_pk,hresp)=Response::OKAY;
               
                for(int i=INT_WS; i>0; i--)
        		wait(clock.posedge_event()); 

                port.reply();
	        port.asend(ret_pk);           
                break;
            
            case WRITE:
                 // AST-Grenoble: sending interrupt to a given Processor
                addr_tmp=occn_hdr(*pk,haddr_write_new);
                intno = (addresser->Logical2Base(addresser->Physical2Logical(addr_tmp, my)) / 4) - 1;
                if (intno < 0 || intno >= N_CORES)
                {
                  printf("%s: Fatal Error: cannot send interrupt on wire %d!\n", name(), intno);
                  exit(1);
                }
                    
                extint[intno].write (true);               
               
                #ifdef OCCN_DEBUG                         
                 cout << "Time for interrupt: " << sc_time_stamp() << endl;
                #endif

                port.reply();
                break;
                
            case WRITEHO:
                // AST-Grenoble: sending interrupt to a given Processor,handover write transfer
                addr_tmp=occn_hdr(*pk,haddr_write_old);
                intno = (addresser->Logical2Base(addresser->Physical2Logical(addr_tmp, my)) / 4) - 1;
                if (intno < 0 || intno >= N_CORES)
                {
                  printf("%s: Fatal Error: cannot send interrupt on wire %d!\n", name(), intno);
                  exit(1);
                }
                
                extint[intno].write (true);

                #ifdef OCCN_DEBUG                         
                 cout << "Time for interrupt: " << sc_time_stamp() << endl;
                #endif

                port.reply();
                break;
                
            case READ:
                // AST-Grenoble: Transfer error,no red operation allowed in interrupt slaves
                printf("Fatal error: Interrupt slave is a write only slave, received read request at time %10.1f\n", sc_simulation_time());
                exit(1);
                break;
                
            case READWRITE:
                 // AST-Grenoble: Transfer error,no red operation allowed in interrupt slaves
                printf("Fatal error: Interrupt slave is a write only slave, received read request at time %10.1f\n", sc_simulation_time());
                exit(1);
                break;   
        }
    } while(1);
}

