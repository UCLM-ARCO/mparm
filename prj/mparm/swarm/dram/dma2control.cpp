///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         dmacontrol.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Implements a DMA controller (processor side)
//
///////////////////////////////////////////////////////////////////////////////


#include "dma2control.h"
#include "debug.h"
void dma2control::simuldma2control()
{
readydma.write(false);
requestdmatransfer.write(false);

// Phase 1: New object state and new commit table refresh
// when a job is terminated

while(1)
{  
readydma.write(false);
if(finished.read())
{
//Operation Id reading from dma2transfer
  transfer_id=finished_transfer_id.read();
  processor_id=finished_transfer_proc.read();
    
TRACEX(WHICH_TRACEX, 8,"%s,Finish transfer object:%d,processor:%d\n",
  type,transfer_id,processor_id);

//Committ table refresh
  committ[processor_id][transfer_id]=false;

#ifdef DEBUG_PROFILE_DRAMDMA
  if(reg[processor_id][transfer_id].work.state == 1)
  {
    printf("PR:%d STOP WRITE to SDRAM - DMACONTROL SECTION:%.1f\n",
     reg[processor_id][transfer_id].work.nproc, sc_simulation_time());
  }
  
  if(reg[processor_id][transfer_id].work.state == 3)
  {
    printf("PR:%d STOP READ from SDRAM - DMACONTROL SECTION:%.1f\n",
    reg[processor_id][transfer_id].work.nproc, sc_simulation_time());
  }
#endif

//objext state refresh
  switch(reg[processor_id][transfer_id].work.state)
  {
    case 1: reg[processor_id][transfer_id].work.state=(sc_uint<4>)2;  
    //transf from L1 to L2 ended => new state L2
      break;
    case 3: reg[processor_id][transfer_id].work.state=(sc_uint<4>)0;
      //transf from L2 to L1 ended => new state L1
      break;
  }

//Poletti
//wait();
}

//End phase 1

//Phase 2: DMA programming 

request=requestdma.read();

if ((request == true) && (first_prog == false))
{
  first_prog = true;

//New request

  membus = dmapin.read();
  
TRACEX(WHICH_TRACEX, 10,"%s address:0x%x\n",type,membus.address);
  membus.address = addressing(membus.address);
    
  // Quick sanity check
  if (membus.address >= (uint) size )
  {
    printf("\n%s:%d: Bad address - 0x%08X\n",type,ID,membus.address);
    exit(1);
  }

  for(i=0;i<numproc;i++)
  {
    if(membus.address<((4+28*maxobj)*(i+1)))
    {
      whichproc=i;
      
TRACEX(WHICH_TRACEX, 10,"%s PROCESSOR:%d\n",type,whichproc);
   
      membus.address-=((4+28*maxobj)*i);
     			
      if (membus.address<4)
        numberofobject=0;
      else
      { 
        membus.address-=4;
        numberofobject=1;
      }
      
      break;
    }
  }
    
  if (numberofobject!=0) 
    numberofobject=(sc_uint<32>)(membus.address+28)/28;
//1b is the number data into DMA_CONT_REG multiplied for 4 all - 1   
   	
  if (numberofobject!=0)
    numberofregister=(membus.address%28)/4; 
  else 
    numberofregister=0xFFFF;
   
  if (membus.benable == 0) 
  {
//cout<<"ERROR"<<endl;g++
//exit(1);
  }
  else 
  {    
// Is a read or write we need to do?
    if (membus.rw == 1)
    {//write 
      read_done=false;
      if (numberofobject==0) 
      {//looking for free a data in the reg table
        if (membus.data>=maxobj) 
	{
	  printf("%s:%d: What are you doing? You can't free a not existing position at address:%d!\n",
	  type,ID,membus.data);
   	  exit(1);
	}
		
	//int_object=membus.data%maxobj;
		
	if (free[whichproc][membus.data]==true)
	{
	  printf("%s:%d,%d: Warning you are freeing a free position:%d!\n",
	  type,ID,(uint)whichproc,(uint)membus.data);
	}
	else 
	{
	  free[whichproc][membus.data]=true;
	  counter[whichproc]--;
	}
		 
TRACEX(WHICH_TRACEX, 8,"%s Freed position:%d||proc:%d,tot number of object:0x%x\n",
type,(uint)membus.data,whichproc,(uint)counter[whichproc]);
      }
      else
      {//
	numberofobject--;
	
	if(free[whichproc][numberofobject]==true)
	{
	  printf("%s:%d, Error:write on a free object nproc:%d,nobj:%d",
	  type,ID,whichproc,numberofobject);
	  exit(1);
	}

// Interna matrix organization for objects

	switch (membus.bw)
	{
	  case 0: // Write word
          {
	   //printf("\n\nNumberofregister:%d\n",numberofregister);
	    switch(numberofregister)
	    {
	      case 0: reg[whichproc][numberofobject].work.size=membus.data;
	      //printf("\n\nsize:%u,mem_bus:%d\n",(uint)(reg[whichproc][numberofobject].work.size),(uint)membus.data);
	      break;
	      
	      case 1: reg[whichproc][numberofobject].work.numberofrow=membus.data;
	      break;
	      
	      case 2: reg[whichproc][numberofobject].work.rowlengthl1=membus.data;
	      break;
	      
	      case 3: reg[whichproc][numberofobject].work.rowlengthl2=membus.data;
	      break;
	      
	      case 4:
	      if (addresser->MapPhysicalToSlave(membus.data)==-1)
	      {
	        printf("%s%d wrong address for the first:0x%x\n",type,ID,membus.data);
	        exit(1);
	      }
	      reg[whichproc][numberofobject].work.l1=(sc_uint<32>)membus.data;
	      break;
	      
	      case 5:
	      {
		if(DRAM_BASE > membus.data || membus.data > DRAM_BASE + DRAM_SIZE)
		{
		  printf("%s:%x PROC:%d wrong address for the DRAM:%x\n",type,ID,whichproc,membus.data);
		  exit(1);
		}
		reg[whichproc][numberofobject].work.l2=(sc_uint<32>)membus.data;
	      break;
	      };
	      	      
	      case 6:
	      {
		if ( membus.data > 9 )
		{
		  printf("DMA%d: wrong type of state:%d\n",ID,membus.data);
		  exit(1);
		}
		  if ( membus.data == 4)
		  {
		    reg[whichproc][numberofobject].work.state=1;
		    reg[whichproc][numberofobject].work.send_int=true;
		    reg[whichproc][numberofobject].work.data_transf=0;
		  }	  
		  else
		    if ( membus.data == 5)
		    {
		      reg[whichproc][numberofobject].work.state=3;
		      reg[whichproc][numberofobject].work.send_int=true;
		      reg[whichproc][numberofobject].work.data_transf=0;
		    }
		    else 
		      if ( membus.data == 6)
		      {
			reg[whichproc][numberofobject].work.state=1;
	                reg[whichproc][numberofobject].work.send_int=false;
			reg[whichproc][numberofobject].work.data_transf=1;
		      }
		      else 
			if ( membus.data == 7)
			{
			  reg[whichproc][numberofobject].work.state=1;
			  reg[whichproc][numberofobject].work.send_int=true;
			  reg[whichproc][numberofobject].work.data_transf=1;
			}
			else 
			  if ( membus.data == 8)
			  {
			    reg[whichproc][numberofobject].work.state=3;
			    reg[whichproc][numberofobject].work.send_int=false;
			    reg[whichproc][numberofobject].work.data_transf=1;
			  }
			  else 
			    if ( membus.data == 9)
			    {
			      reg[whichproc][numberofobject].work.state=3;
			      reg[whichproc][numberofobject].work.send_int=true;
			      reg[whichproc][numberofobject].work.data_transf=1;
			    }	    
			    else
			    {
			      reg[whichproc][numberofobject].work.state=(sc_uint<4>)membus.data;
			      reg[whichproc][numberofobject].work.send_int=false;
			      reg[whichproc][numberofobject].work.data_transf=0;
			    }
											
	      if(membus.data==3 || membus.data==1)
	      {
	       if (nwork==(maxobj*numproc))						   
	       {									   
	         printf("%s Too many works to do:%d,%d!\n",type,nwork,(maxobj*numproc));   
	         exit(1);								   
	       }									   
	       else									   
	       //new work in queue							   
	       {	        									   
	         nwork++;								   
	       }									   
	      }
             
TRACEX(WHICH_TRACEX, 10,"%s ,%d New state:%d,Send_int:%d,object:%d\n",
type,whichproc,(uint)reg[whichproc][numberofobject].work.state,
reg[whichproc][numberofobject].work.send_int,numberofobject);

	      break;
	    }
	  }
	}
	break;
	
	
	default:	
	printf("Error: wrong type of write on the DMA%d\n",ID);
	exit(1);
      }
    }
	       
TRACEX(WHICH_TRACEX, 8,"%s:%d Write at the address:0x%x,in the register:0x%x.%x,the value:0x%x,size:%d\n",
type,whichproc, membus.address, numberofobject, numberofregister, membus.data, (uint)reg[whichproc][numberofobject].work.size);

  }
  //read
  else
  {
    read_done=true;
		
// Read of the address 0 of the dma => ask for a free position
    if (numberofobject==0)
    {
      if(counter[whichproc] == maxobj)
      {
    	membus.data=0xFFFFFFFF;
TRACEX(WHICH_TRACEX, 8,"%s,%d: No free position\n",type,whichproc);
      } 
    else
    {
      for (i=0;i<maxobj;i++)
      {
        if (free[whichproc][i]==true) break;
      }
      free[whichproc][i]=false;
      counter[whichproc]++;
			
      membus.data=(sc_uint<32>)(i);
	
TRACEX(WHICH_TRACEX, 8,"%s New object in position:0x%x\n",type,i);

    }
  }
  else
  {
    numberofobject--;
    if(free[whichproc][numberofobject]==true)
    {
      printf("%s, Error:write on a free object nproc:%d,nobj:%d",
      type,whichproc,numberofobject);
      exit(0);
    }
    
    switch(numberofregister)
    {
      case 0:
        membus.data=reg[whichproc][numberofobject].work.size;
        break;
      case 1:
        membus.data=reg[whichproc][numberofobject].work.numberofrow;
        break;
      case 2:
        membus.data=reg[whichproc][numberofobject].work.rowlengthl1;
        break;
      case 3:
        membus.data=reg[whichproc][numberofobject].work.rowlengthl2;
        break;
      case 4:
        membus.data=reg[whichproc][numberofobject].work.l1;
        break;
      case 5:
        membus.data=reg[whichproc][numberofobject].work.l2;
        break;
      case 6:
        membus.data=(sc_uint<32>)reg[whichproc][numberofobject].work.state;
        break;
    }
  }
	      
TRACEX(WHICH_TRACEX, 10,"%s: time:%.1f Read word at address:0x%x,register:0x%x.%x,value:0x%x whichproc:%d\n",
	       type, sc_simulation_time(), membus.address, numberofobject, numberofregister, membus.data,whichproc);
}
       
}//end bus_enable 	 

readydma.write(true);

if(read_done)
{
  dmapin.write(membus);
  read_done=false;
}

}//end requestdma


if(first_prog == true && requestdma.read()== false) first_prog = false;


//End phase 2


// Phase 3: works sent to DMA2transfer engine
if(done==true) 
{
 //we have setted a request to the transfer the previous cycle lets wait for one cyle!
 done=false;
}
else
{
  dma_transfer_ack=transfer_ack.read();
  // Work are accepted if there are non preesitent works active
  if (!dma_transfer_ack)
  {
    if(nwork!=0)
    {
      for (t=0;t<maxobj;t++)
      {
	for (r=0;r<numproc;r++)
	{
  //if an object is in state 1 or 3 is not committed
	  if(committ[r][t]==false && (reg[r][t].work.state==1 || reg[r][t].work.state==3) && done==false)
	  {
  // Setting object to be transferred
	    reg[r][t].work.nproc=r;
	    reg[r][t].num_obj=t;

  TRACEX(WHICH_TRACEX, 10,"%s START TRANSFER %d num_obj:%d Time:%.1f l1:%x l2:%x size:%d state%d\n",
	     type,(uint)reg[r][t].work.nproc,(uint)reg[r][t].num_obj, sc_simulation_time(),
	     (uint)reg[r][t].work.l1,(uint)reg[r][t].work.l2,
	     (uint)reg[r][t].work.size,(uint)reg[r][t].work.state);

  //send object to trnsfer engine
	    datadma.write(reg[r][t]);
  //activate request signal
	    requestdmatransfer.write(true);
  //refresh committ table state
	    committ[r][t]=true;
  //decrement numer of objects of dma2control
	    nwork--;	  
  //use to exit from cycle
	    done=true;

  //debug
  #ifdef DEBUG_PROFILE_DRAMDMA
    if(reg[r][t].work.state == 1)
    {
      printf("PR:%d START WRITE to SDRAM - DMACONTROL SECTION:%.1f\n",reg[r][t].work.nproc, sc_simulation_time());
    }

    if(reg[r][t].work.state == 3)
    {
      printf("PR:%d START READ from SDRAM - DMACONTROL SECTION:%.1f\n",reg[r][t].work.nproc, sc_simulation_time());
    }
  #endif	
           //It's better to wait for one cycle more when we set a request to the transfer due to synchronization issue...
	  }
	  if(done==true)
           break;

	}
	if(done==true)
	 break;
      }
    }
  }//end of dma_transfer_ack
}

wait();

requestdmatransfer.write(false);


}//end while

}
