#include "dma2transfer.h"

void dma2transfer::transfer_work()
{
if(reset_transfer.read() == true)
{
// Works for this engine are only one for now
// transfer_ack is the input fifo of T.E.
// if it's empty, we can accept a work: so at reset
// we put transfer_ack false
  transfer_ack.write(false);
  finished = false;
  finished_transfer_id.write(0);
  finished_transfer_proc.write(0);
  request.write(false);
  prog_accepted = false;
  lmi_programmed = false;
  burst = 0;
  actual_size = 0;
  write_done = true;
  data_out = false;
  end_write = false;
  st_measure_time = 0;
  
}
else
{
 finished.write(false);

  if(requestdmatransfer.read() == true)
  {
    work=datadma.read();

  TRACEX(WHICH_TRACEX, 8,"%s START TRANSFER %d \n",type,(uint)work.work.nproc);
  TRACEX(WHICH_TRACEX, 10,"%s L1:%x L2:%x size:%d state%d\n",type,(uint)work.work.l1,(uint)work.work.l2,
  		     (uint)work.work.size,(uint)work.work.state);		     
  TRACEX(WHICH_TRACEX, 12," %s at Time:%.1f \n",type,sc_simulation_time());		     

    transfer_ack.write(true);
    prog_accepted = true;
  }
  else
  {
  }

// Check if i have a L1->L2 or L2->L1
  if(prog_accepted == true)
  {
    switch(work.work.state)
    {
      case 1:   //transf from L1 to L2 
// Transfer from STBUS to SDRAM:
// STBUS READ , SDRAM WRITE

// Memory Controller - State machine programming

      if(ready.read() == true && fcount.read() == 0 && lmi_programmed == false)
      {
        request.write(true);
        AddressBus.write(work.work.l2);
        SizeBus.write(work.work.size);
        RD.write(0); // write on SDRAM
        lmi_programmed = true;
        address = work.work.l1;
        actual_size = work.work.size;
      }
      else
      {
        request.write(false);
        AddressBus.write(0);
        SizeBus.write(0);
        RD.write(0); 
      }

// STBUS operation

// End of transfer

      if(lmi_programmed == true )
      {
	fpush_processor.write(false);
	
	if(burst == 0 && actual_size == 0)
	{
	  finished.write(true);
	  finished_transfer_id.write((uint)work.num_obj);
	  finished_transfer_proc.write((uint)work.work.nproc);
	  transfer_ack.write(false);
	  prog_accepted = false;
	  lmi_programmed = false;
	}
	
// STBUS READING PART
	
	if(burst > 0 && read_done == false)
	{
	  if(read(address,buffer,burst,work.work.nproc) == true)
	  {
	    read_done = true;
	    
	    TRACEX(WHICH_TRACEX, 8,"%s READ from STBus finished\n",type);
            #ifdef OUTDEBUG
	    for(i=0;i<burst;i++)
	    {
	      TRACEX(WHICH_TRACEX, 10,"%s Buffer[%d]:%d\n",type,i,buffer[i]);
	    }
	    #endif
	    TRACEX(WHICH_TRACEX, 12," %s at Time:%.1f \n",type, sc_simulation_time());		     
	    
	  }
	  else
	  {
    	    TRACEX(WHICH_TRACEX, 8,"%s READ from STBus in Action\n",type);
	    TRACEX(WHICH_TRACEX, 12," %s at Time:%.1f \n",type,sc_simulation_time());		     
	    read_done = false;
	  }
	}

// Buffer calculation and Memory controller commands

	if(burst > 0 && read_done == true)
	{
	  if(FIFO_LENGHT - fcount.read() >= burst)
	  {
	     fpush_processor.write(true);
	     data_dimension.write(burst);

	     TRACEX(WHICH_TRACEX, 8,"%s Buffer Emptying\n",type);
	     #ifdef OUTDEBUG
	     for(i=0;i<burst;i++)
	     {
	       TRACEX(WHICH_TRACEX, 10,"%s Buffer[%d]:%d\n",type,i,buffer[i]);
	     }
	     TRACEX(WHICH_TRACEX, 12,"%s at Time:%.1f \n",type,sc_simulation_time());
	     #endif
	     
	     for(i=0;i<burst;i++)
	     {
	       data_processor[i].write(buffer[i]);
	     }
	     address = address + 4*burst;
	     burst = 0;
	  }
	  else
	  {
	    fpush_processor.write(false);
	  }

	}


// Transfer parameters calculation 

        if( burst == 0 && actual_size > 0 )
	{
	 burst = set_burst((unsigned int)address);
	  							
	  if(actual_size > burst)
	  {
	    actual_size = actual_size - burst;
	  }
	  else
	  {
	    burst = actual_size;
	    actual_size = 0;
	  }

	  read_done = false;
	
	  //printf("actual size:%d burst:%d\n",(int)actual_size,(int)burst);
	  TRACEX(WHICH_TRACEX, 10,"%s in L1->L2 :actual size:%d burst:%d address:%x\n",
	  	type,(int)actual_size,(int)burst,(int)address);

	  TRACEX(WHICH_TRACEX, 12," %s at Time:%.1f \n",type, sc_simulation_time());
	}
      }//end if(lmi_progammed == true )

    break;

    case 3:   //transf from L2 to L1 
// Transfer from SDRAM to STBUS:
// STBUS WRITE , SDRAM READ

	if(ready.read() == true && fcount.read() == 0 && write_done == true)
	{
	  request.write(true);
	  AddressBus.write(work.work.l2);
	  SizeBus.write(work.work.size);
	  RD.write(1); // READ
	  lmi_programmed = true;
	  address = work.work.l1;
	  actual_size = work.work.size;
	  buffer_ok = false;
	  write_done = false;		  
	}
	else
	{
	  request.write(false);
	  AddressBus.write(0);
	  SizeBus.write(0);
	  RD.write(1); // e' una read
	}
	
	if(lmi_programmed == true)
	{
  	  if(burst == 0 && actual_size == 0 && end_write == true)
	  {
	    finished.write(true);
	    finished_transfer_id.write((uint)work.num_obj);
	    finished_transfer_proc.write((uint)work.work.nproc);
	    transfer_ack.write(false);
	    write_done = true;
	    prog_accepted = false;
	    lmi_programmed = false;		  
		
  TRACEX(WHICH_TRACEX, 8,"%s STOP TRANSFER %d \n",type,(uint)work.work.nproc);
  TRACEX(WHICH_TRACEX, 10,"%s L1:%x L2:%x size:%d state%d\n",type,(uint)work.work.l1,(uint)work.work.l2,
  		     (uint)work.work.size,(uint)work.work.state);		     
  TRACEX(WHICH_TRACEX, 12,"%s at Time:%.1f \n",type, sc_simulation_time());		     

	  }
	
	if(fcount.read() >= burst && burst > 0 && data_out == false && buffer_ok == false)
	{
	  fpop_processor.write(true);
	  data_dimension.write(burst);
	  data_out = true;
	}
	else
	{
	  fpop_processor.write(false);
	}


	if(data_out_proc.read() == true)
	{
  	  for(i=0;i<burst;i++)
	  {
	    buffer[i] = data_processor[i].read();
  TRACEX(WHICH_TRACEX, 10,"%s buffer[%d]:%ld\n",type,i,buffer[i]);
 	  }
	  
  TRACEX(WHICH_TRACEX, 12," %s at Time:%.1f \n",type, sc_simulation_time());		     
	  buffer_ok = true;
	  data_out = false;
	}


	if(buffer_ok == true)
	{
	  if(write(address,buffer,burst,work.work.nproc) == true)
	  {
	    buffer_ok = false;			
	    
TRACEX(WHICH_TRACEX, 8,"%s WRITE to STBus finished\n",type);
TRACEX(WHICH_TRACEX, 12," %s at Time:%.1f \n",type, sc_simulation_time());
	    address = address + burst*4;
	    burst=0;
	    end_write = true;
	  }
	  else
	  {
	    buffer_ok = true;

TRACEX(WHICH_TRACEX, 8,"%s WRITE to STBus in Action\n",type);
TRACEX(WHICH_TRACEX, 12," %s at Time:%.1f \n",type, sc_simulation_time());

	    end_write = false;  	    
	  }

	}
	
	if( burst == 0 && actual_size > 0 )
	{
	 burst = set_burst((unsigned int)address);
	  							
	  if(actual_size > burst)
	  {
	    actual_size = actual_size - burst;
	  }
	  else
	  {
	    burst = actual_size;
	    actual_size = 0;
	  }
      
//printf("Burst:%d,add:%x,actual_size:%d\n",(uint)burst,(uint)address,uint(actual_size));
	  TRACEX(WHICH_TRACEX, 10,"%s in L2->L1 :actual size:%d burst:%d address:%d\n",
	  	type,(int)actual_size,(int)burst,(int)address);
	  TRACEX(WHICH_TRACEX, 12,"%s at Time:%.1f \n",type, sc_simulation_time());
        }
      } // end if(lmi_programmed == true)
      break;
    }
  } // end if(prog_accepted == true) 
  else
  {
    request.write(false);
    AddressBus.write(0);
    SizeBus.write(0);
    lmi_programmed = false;
  }

}

}

// STBUS interface

bool dma2transfer::read(uint address, UInt32* buffer, uint burst, uint nproc)
{
  lx_extmem_result risultato;
  //UInt8 byteenable[8];
  UInt8 byteenable[BUS_MAX_BURST_LENGHT];
  //for(int i =0;i<8;i++) byteenable[i]= 0xF;
  for(int i =0;i<BUS_MAX_BURST_LENGHT;i++) byteenable[i]= 0xF;

  risultato = stbusport->HwDataRead(burst,address,buffer,byteenable,NULL);

  switch (risultato)
  {
    case LX_EXTMEM_COMPLETED:

      TRACEX(WHICH_TRACEX,8,"%s READ STBUS address:0x%x burst:%d \n",type,address,burst);
      #ifdef OUTDEBUG
      for(uint i=0; i<burst; i++)
        TRACEX(WHICH_TRACEX,8,"%s --data:%d\n",type,buffer[i]);
      #endif
      st_measure_time=false;
      
      #ifdef DEBUG_PROFILE_DRAMDMA
        printf("PR:%d STOP of DMATRANSFER READ on STBus:%.1f\n",nproc, sc_simulation_time());
      #endif
      
      return true;
    break;
	
    case LX_EXTMEM_PENDING:
    
       if(st_measure_time==false)
	{
	  st_measure_time=true;
	  #ifdef DEBUG_PROFILE_DRAMDMA
            printf("PR:%d START of DMATRANSFER READ on STBus:%.1f\n",nproc, sc_simulation_time());
	  #endif
	}
		
    return false;	
    break;
	
    default:
  
      printf("HwDataRead strange result!\n");
      exit(1);
  }
}


bool dma2transfer::write(uint address, UInt32* buffer, uint burst, uint nproc)
{
  lx_extmem_result risultato;
//  UInt8 byteenable[8];
//  for(int i =0;i<8;i++) byteenable[i]= 0xF;
  UInt8 byteenable[BUS_MAX_BURST_LENGHT];
  for(int i =0;i<BUS_MAX_BURST_LENGHT;i++) byteenable[i]= 0xF;

  risultato = stbusport->HwDataWrite(burst,address,buffer,byteenable,NULL);

//printf("WRITE STBUS address:0x%x,burst:%d\n",address,burst);


  switch (risultato)
  {
    case LX_EXTMEM_COMPLETED:

      st_measure_time=false;
       
      #ifdef DEBUG_PROFILE_DRAMDMA
        printf("PR:%d STOP of DMATRANSFER WRITE on STBus:%.1f\n",nproc, sc_simulation_time());
      #endif       
       
       return true;
       break;

    case LX_EXTMEM_PENDING:
      if(st_measure_time==false)
      {
        st_measure_time=true;
	//printf("        WAITING the BUS! %d\n",(int)(sc_simulation_time()));
        #ifdef DEBUG_PROFILE_DRAMDMA
          printf("PR:%d START of DMATRANSFER WRITE on STBus:%.1f\n",nproc, sc_simulation_time());
	#endif
	
      }
      return false;	
      break;
      

     default:
       printf("HwDataWrite strange result!\n");
       exit(1);
  }
}
