///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         transaction_gen.cpp
// author       DEIS - Universita' di Bologna
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
// info         Transaction generator for a bus traffic generator
//
///////////////////////////////////////////////////////////////////////////////

#include "transaction_gen.h"

transaction_gen::transaction_gen(uint ID, TRAFFIC_TYPE ttype) : ID(ID), ttype(ttype)
{
  trans_counter = 0;
  loop_counter = 0;
  trans_done = false;
#include "transaction_table.h"
}

void transaction_gen::get_trans(uint32_t *data, uint32_t *address, uint *burst, bool *wr, TGEN_BENABLE *benable, int *idlewait)
{
  switch (ttype)
  {
    case TGEN_RANDOM:
                          break;
    case TGEN_TABLE:      if (trans_done)
                          {
                            // Ready for shutdown
                            *idlewait = -1;
                          }
                          else
                          {
                            *data = data_array[trans_counter];
                            *address = address_array[trans_counter];
                            *burst = burst_array[trans_counter];
                            *wr = wr_array[trans_counter];
                            *benable = benable_array[trans_counter];
                            *idlewait = idlewait_array[trans_counter];
                            trans_counter ++;
                            // Let's see if we're at the end
                            // of a transaction set
                            if (trans_counter == max_trans)
                            {
                              loop_counter ++;
                              
                              // If we still have loops to go, or if
                              // we must loop forever, let's go...
                              if ((int)loop_counter < repeat_loop || repeat_loop == -1)
                                trans_counter = 0;
                              // Otherwise we're done
                              else
                                trans_done = true;
                            }
                          }
                          break;
    case TGEN_RHOSIGMA:
#if 0
    bool on = false;
    int size = 0;
    int bursts = 0;
    bool rw;
    uint64_t t;
    uint wait_cycles;
    bool firstwait = true;
    
    count = 0;
    
    if(RHO != 0 && SIGMA != 0){
    on = true;
    //wait_cycles = (uint) ((100 - RHO) / ((RHO / SIGMA)-1));
    wait_cycles = (uint) ((100 - RHO) / ((RHO / SIGMA)));}
    else{
    on = false;
    wait_cycles = 0;
    }
    printf("wait_cycles = %d\n",wait_cycles);


    while(1)
    {
     if(on){
     
     firstwait = true;
        
      address = (N_MASTERS - 1) * 0x01000000;

      address = addresser->Logical2Physical(address, SOURCE);
      
      dout = 0xfedefede;  // in onore del prode Federico Angiolini
      size = 0x4;  /* word transfer */
      //bursts = 1;  /* single transfer */
      bursts = 4*SIGMA/10;
      rw = 1;
         
         for (int i = 0; i < bursts; i++)
          t = WRITE(address, size, (TDATA)dout, i, bursts, vtid);
   
       do
          {
           count++;
           wait();
          } while (count < wait_cycles + 1);

          count = 0;
          wait();
     }
     else
     wait();
    } // end while(1)
#endif
                          break;
    case TGEN_STATISTICAL:
                          break;
  }
  return;
}
