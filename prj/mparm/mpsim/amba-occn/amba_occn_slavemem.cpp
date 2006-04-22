///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_occn_slavemem.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         AMBA (OCCN) memory slave
//
///////////////////////////////////////////////////////////////////////////////

#include "amba_occn_slavemem.h"

///////////////////////////////////////////////////////////////////////////////
// receive - Listens to the bus waiting for requests. When one is received,
//           it is forwarded to the memory module (a C++ class so to be able to
//           do 0-WS operation), then a configurable number of wait states is
//           inserted and the answer is put on the bus.

#define KEY 7
#define MOVE 3

void amba_occn_slavemem::receive()
{
   Pdu<AhbMasterCtrl,N_int32> *pk;
   Pdu<AhbSlaveCtrl,N_int32>  *ret_pk = new Pdu<AhbSlaveCtrl,N_int32>;
   uint32_t data;
   uint16_t size = 0;


    
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
        
       
    

        switch(op){
            case HREADY:
                // AST-Grenoble: first step in write,hready response
                tra_tmp=occn_hdr(*pk,htrans);

                switch (occn_hdr(*pk,hsize))
                {
                  case 1 :  size = MEM_BYTE;
		            break;
                  case 2 :  size = MEM_HWORD;
                            break;
                  case 4 :  size = MEM_WORD;
		            break;
	          default : printf("Fatal error: Memory %hu detected a malformed data size at time %10.1f\n",
                              my, sc_simulation_time());
                            exit(1);
                } 

                for(int i=MEM_IN_WS; i>0; i--)
        		wait(clock.posedge_event());

                
               
  
                occn_hdr(*ret_pk,hready)=1;
                occn_hdr(*ret_pk,hresp)=Response::OKAY;
                port.reply();
                //AST-Grenoble: reply() represents hready RTL signal
                //AST-Grenoble: send control back to master
                port.asend(ret_pk);
                break;
                
            case HREWR:
                // AST-Grenoble: write of previous transfer and hready response to current tranfer
            	tra_tmp=occn_hdr(*pk,htrans);  
                                                                     
                addr_tmp=occn_hdr(*pk,haddr_write_old);
                addr_tmp = addresser->Logical2Base(addresser->Physical2Logical(addr_tmp, my));                         
                data=*pk;
                switch (occn_hdr(*pk,hsize))
                {
                  case 1 :  size = MEM_BYTE;
		            break;
                  case 2 :  size = MEM_HWORD;
                            break;
                  case 4 :  size = MEM_WORD;
		            break;
	          default : printf("Fatal error: Memory %hu detected a malformed data size at time %10.1f\n",
                              my, sc_simulation_time());
                            exit(1);
                }
                myRam->Write(addr_tmp, data, size);
                
                #ifdef OCCN_DEBUG
		 printf("Slave0 %u: write %8x at address 0x%08x ", my, data, (unsigned int)addr_tmp);
                 cout << " | time: " << sc_time_stamp() << endl;
                #endif

                #ifdef EXTRAMPRINTDEBUG
                 printf("Slave0 %u:        write %8x at address 0x%08x\n", my, data, (unsigned int)addr_tmp);
                #endif

                occn_hdr(*ret_pk,hready)=1;
                occn_hdr(*ret_pk,hresp)=Response::OKAY;               

                for(int i=MEM_IN_WS; i>0; i--)
        	       wait(clock.posedge_event()); 

                port.reply();
                //AST-Grenoble: reply() represents hready RTL signal
                //AST-Grenoble: send control back to master
	        port.asend(ret_pk);           
                break;
            
            case WRITE:
                // AST-Grenoble: write of previous transfer,no hready response
                addr_tmp=occn_hdr(*pk,haddr_write_new);
                addr_tmp = addresser->Logical2Base(addresser->Physical2Logical(addr_tmp, my));                         
                data = *pk;
               
                myRam->Write(addr_tmp, data, size);
                
                #ifdef OCCN_DEBUG 
		 printf("Slave0 %u: write %8x at address 0x%08x ", my, data, (unsigned int)addr_tmp);
                 cout << " | time: " << sc_time_stamp() << endl;
                #endif
 
                #ifdef EXTRAMPRINTDEBUG
                 printf("Slave0 %u:        write %8x at address 0x%08x\n", my, data, (unsigned int)addr_tmp);
                #endif
                port.reply();
                break;
                
            case WRITEHO:
                 // AST-Grenoble: write of previous transfer in handover operation,no hready response
                addr_tmp=occn_hdr(*pk,haddr_write_old);
                addr_tmp = addresser->Logical2Base(addresser->Physical2Logical(addr_tmp, my));
                data = *pk;
               
                myRam->Write(addr_tmp, data, size);
               
                #ifdef OCCN_DEBUG
		 printf("Slave0 %u: write %8x at address 0x%08x ", my, data, (unsigned int)addr_tmp);
                 cout << " | time: " << sc_time_stamp() << endl;
                #endif

                #ifdef EXTRAMPRINTDEBUG
                 printf("Slave0 %u:        write %8x at address 0x%08x\n", my, data, (unsigned int)addr_tmp);
                #endif                
                port.reply();
                break;
                
            case READ:
                // AST-Grenoble: read transfer in slave memory
                tra_tmp=occn_hdr(*pk,htrans);

                switch (occn_hdr(*pk,hsize))
                {
                  case 1 :  size = MEM_BYTE;
		            break;
                  case 2 :  size = MEM_HWORD;
                            break;
                  case 4 :  size = MEM_WORD;
		            break;
	          default : printf("Fatal error: Memory %hu detected a malformed data size at time %10.1f\n",
                              my, sc_simulation_time());
                            exit(1);
                }

                for(int i=MEM_IN_WS; i>0; i--)
        		wait(clock.posedge_event());

                addr_tmp=occn_hdr(*pk,haddr_read);
                addr_tmp = addresser->Logical2Base(addresser->Physical2Logical(addr_tmp, my));
                data = myRam->Read(addr_tmp);

                #ifdef OCCN_DEBUG
		 printf("Slave0 %u: read  %8x at address 0x%08x ", my, data, (unsigned int)addr_tmp);
                 cout << " | time: " << sc_time_stamp() << endl;
                #endif  

                #ifdef EXTRAMPRINTDEBUG
                  printf("Slave0 %u:        read  %8x at address 0x%08x\n", my, data, (unsigned int)addr_tmp);
                #endif              
                *ret_pk=data;
                                               
                occn_hdr(*ret_pk,hready)=1;
                occn_hdr(*ret_pk,hresp)=Response::OKAY;
                port.reply();
                //AST-Grenoble: reply() represents hready RTL signal
                //AST-Grenoble: send control back to master
                port.asend(ret_pk);
                break;
                
            case READWRITE:
                 // AST-Grenoble: write of previous transfer and read of current transfer
                // AST-Grenoble: Read and write operation in a given clock cycle,in order to support RTL granularity
                tra_tmp=occn_hdr(*pk,htrans);
                //AST-Grenoble: write  operation
                addr_tmp=occn_hdr(*pk,haddr_write_new);
                addr_tmp = addresser->Logical2Base(addresser->Physical2Logical(addr_tmp, my));             
                data = *pk; 
               
                switch (occn_hdr(*pk,hsize))
                {
                  case 1 :  size = MEM_BYTE;
		            break;
                  case 2 :  size = MEM_HWORD;
                            break;
                  case 4 :  size = MEM_WORD;
		            break;
	          default : printf("Fatal error: Memory %hu detected a malformed data size at time %10.1f\n",
                              my, sc_simulation_time());
                            exit(1);
                }
               
                myRam->Write(addr_tmp, data, size);
                
                #ifdef OCCN_DEBUG
		 printf("Slave0 %u: write %8x at address 0x%08x ", my, data, (unsigned int)addr_tmp);
                 cout << " | time: " << sc_time_stamp() << endl;
                #endif
 
                #ifdef EXTRAMPRINTDEBUG
                 printf("Slave0 %u:        write %8x at address 0x%08x\n", my, data, (unsigned int)addr_tmp);
                #endif                              

                 //AST-Grenoble: read operation
                for(int i=MEM_IN_WS; i>0; i--)
        		wait(clock.posedge_event());
                        
                addr_tmp=occn_hdr(*pk,haddr_read);
                addr_tmp = addresser->Logical2Base(addresser->Physical2Logical(addr_tmp, my));
                data = myRam->Read(addr_tmp); 
               
                #ifdef OCCN_DEBUG
		 printf("Slave0 %u: read  %8x at address 0x%08x ", my, data, (unsigned int)addr_tmp);
                 cout << " | time: " << sc_time_stamp() << endl;
                #endif               

                #ifdef EXTRAMPRINTDEBUG
                  printf("Slave0 %u:        read  %8x at address 0x%08x\n", my, data, (unsigned int)addr_tmp);
                #endif   
           
                *ret_pk=data;                               
                occn_hdr(*ret_pk,hready)=1;
                occn_hdr(*ret_pk,hresp)=Response::OKAY;
                port.reply();
                //AST-Grenoble: reply() represents hready RTL signal
                //AST-Grenoble: send control back to master
                port.asend(ret_pk);
                break;   
        }
    } while(1);
}

