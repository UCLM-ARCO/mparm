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

#include "dmacontrol.h"

void dmacontrol::simuldmacontrol()
{
readydma.write(false);
while(1)
{  
   if(finished.read() && !endtransfer)
   {
    endtransfer=true;
    request=false;
   }
   else
    {
     wait_until(requestdma.delayed() == true  || finished.delayed() == true);
     request=requestdma.read();
     endtransfer=finished.read();
    }
  if (endtransfer)
    {
     TRACEX(WHICH_TRACEX, 8,"%s:%d,%d Finish transfer object:%d,nwork:%d\n",
     type,ID,(uint)reg[work[working]].nproc,work[working],nwork);

     //fix the state of the object
     switch(reg[work[working]].state)
     {case 1: reg[work[working]].state=2;  //transf from l1 to l2 ended => new state l2
   	    break;
      case 3: reg[work[working]].state=0;  //transf from l2 to l1 ended => new state l1
   	    break;
   
     }
     working++;
     if (working==maxobj*numproc) working=0;
     nwork--;
    }
  
  if (request)
  {
   changestate=false;
  //New request
  membus = dmapin.read();
  
  TRACEX(WHICH_TRACEX, 10,"%s:%d address:0x%x\n",type,ID,membus.address);

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
     
     TRACEX(WHICH_TRACEX, 10,"%s:%d PROCESSOR:%d\n",type,ID,whichproc);
   
     membus.address-=((4+28*maxobj)*i);
     if (membus.address<4) 
      numberofobject=0;
     else
     { 
      membus.address-=4;
      numberofobject=1;
     }
     
     TRACEX(WHICH_TRACEX, 10,"%s:%d: processor:%d \n",type,ID,whichproc);   

     break;
    }
   }
    
   if (numberofobject!=0) 
    numberofobject=(sc_uint<32>)(membus.address+28)/28;
   
   //1b is the number data into DMA_CONT_REG multiplied for 4 all - 1   
   if (numberofobject!=0) numberofregister=(membus.address%28)/4; 
   else numberofregister=0xFFFF;
   
   if (membus.benable == 0) 
   {
   //cout<<"ERROR"<<endl;
   //exit(1);
   }
   else 
   {    
	   // Is a read or write we need to do?
	   if (membus.rw == 1)
	    {//write 
	      if (numberofobject==0) 
	      {//looking for free a data in the reg table
	       if (membus.data>=maxobj) 
	        {printf("%s:%d: What are you doing? You can't free a not existing position at address:%d!\n",
		  type,ID,membus.data);
		 exit(1);
		}
		
		//int_object=membus.data%maxobj;
		
	        if (free[whichproc][membus.data]==true)
		{printf("%s:%d,%d: Warning you are freeing a free position:%d!\n",
		  type,ID,(uint)whichproc,(uint)membus.data);
		}
		else 
		 {free[whichproc][membus.data]=true;
		  counter[whichproc]--;
		 }
		 
		TRACEX(WHICH_TRACEX, 8,
		"%s:%d: Freed position:%d||proc:%d,tot number of object:0x%x\n",
		type,ID,(uint)membus.data,whichproc,(uint)counter[whichproc]);

	      }
	      else
	       {//
	        numberofobject--;
	        
		if(free[whichproc][numberofobject]==true)
		{
		 printf("%s:%d, Error:write on a free object nproc:%d,nobj:%d",
		 type,ID,whichproc,numberofobject);
		 exit(0);
		}
		//Riscale the number of object with the total work
		numberofobject+=(whichproc*maxobj);
		
	        switch (membus.bw)
		{
		  case 0: // Write word
		  {switch(numberofregister)
		   {
		    case 0: reg[numberofobject].size=membus.data;
		    	break;
		    case 1: reg[numberofobject].numberofrow=membus.data;
		    	break;
		    case 2: reg[numberofobject].rowlengthl1=membus.data;
		    	break;
		    case 3: reg[numberofobject].rowlengthl2=membus.data;
		    	break;
		    case 4:
			if (control(membus.data))
			{printf("DMA%d wrong address for the first:0x%x\n",ID,membus.data);
			 exit(1);
			}
			reg[numberofobject].l1=(sc_uint<32>)membus.data;
			
		    TRACEX(WHICH_TRACEX, 10,
		     "%s:%d data:%x,l1:%x,reg:%x,&reg[numberofobject].l1:%x,%d\n",
                     type,ID,(uint)membus.data,(uint)reg[numberofobject].l1,(uint)reg,
		     (uint)&reg[numberofobject].l1,numberofobject);

			break;
		    case 5:
		       {
			if(control(membus.data))
			{printf("%s:%d wrong address for the second\n",type,ID);
			 exit(1);
			}
			reg[numberofobject].l2=(sc_uint<32>)membus.data;
			
			TRACEX(WHICH_TRACEX, 10,
			 "%s:%d data:%x,l2:%x,reg:%x,&reg[numberofobject].l2:%x,%d\n",
                         type,ID,(uint)membus.data,(uint)reg[numberofobject].l2,(uint)reg,
			 (uint)&reg[numberofobject].l1,numberofobject);
	    	
			break;
		       };
		    case 6: {if ( membus.data > 9 ) 
		    		{printf("DMA%d: wrong type of state:%d\n",ID,membus.data);
		                 exit(1);
				}
			     if ( membus.data == 4)
			     {reg[numberofobject].send_int=true;
			      reg[numberofobject].state=1;
			      reg[numberofobject].data_transf=0;
			     }	
			     else
			      if ( membus.data == 5)
			      {reg[numberofobject].send_int=true;
			       reg[numberofobject].state=3;
			       reg[numberofobject].data_transf=0;
			      }
			      else 
			       if ( membus.data == 6)
			       {reg[numberofobject].state=1;
	                        reg[numberofobject].send_int=false;
				reg[numberofobject].data_transf=1;
			       }
			       else 
			        if ( membus.data == 7)
			        {reg[numberofobject].state=1;
	                         reg[numberofobject].send_int=true;
				 reg[numberofobject].data_transf=1;
			        }
			      else 
			       if ( membus.data == 8)
			       {reg[numberofobject].state=3;
	                        reg[numberofobject].send_int=false;
				reg[numberofobject].data_transf=1;
			       }
			       else 
			        if ( membus.data == 9)
			        {reg[numberofobject].state=3;
	                         reg[numberofobject].send_int=true;
				 reg[numberofobject].data_transf=1;
			        }	
			      else
			      {reg[numberofobject].state=(sc_uint<4>)membus.data;
			       reg[numberofobject].send_int=false;
			       reg[numberofobject].data_transf=0;
			      }
			     //put in int the work queue if the state implies a transfer
			     if ( reg[numberofobject].state == 1 || 
			     reg[numberofobject].state == 3)
			     {changestate=true;
			      numberofchange=numberofobject;
			     }
			     
			     TRACEX(WHICH_TRACEX, 10,"%s:%d,%d New state:%d,Send_int:%d,object:%d\n",
			     type,ID,(uint)reg[numberofobject].nproc,
			     (uint)reg[numberofobject].state,reg[numberofobject].send_int,
			     numberofobject);
		     
			     break;
			    }
		   }
		  }
		  break;
		  printf("Error: wrong type of write on the DMA%d\n",ID);
		  exit(1);
		}
	       }
	       
	       TRACEX(WHICH_TRACEX, 8,
	        "%s:%d: Write at the address:0x%x,in the register:0x%x.%x,the value:0x%x\n",
		type,ID, membus.address, numberofobject, numberofregister, membus.data);

	     }
	  else
	    {
	      // Read of the address 0 of the dma => ask for a free position
	      if (numberofobject==0)
	      {
	       if(counter[whichproc] == maxobj)
	       {membus.data=0xFFFFFFFF;
                
		TRACEX(WHICH_TRACEX, 8,"%s:%d,%d: No free position\n",type,ID,whichproc);
	       } 
	       else
	       {for (i=0;i<maxobj;i++)
	        {
	         if (free[whichproc][i]==true) break;
		}
		free[whichproc][i]=false;
	       	counter[whichproc]++;
		reg[i+(whichproc*maxobj)].nproc=whichproc;
	        membus.data=(sc_uint<32>)(i);
		
		TRACEX(WHICH_TRACEX, 8,"%s:%d,%d New object in position:0x%x\n",
		type,ID,(uint)reg[i+(whichproc*maxobj)].nproc,i);
	       }
	      }
	      else
	      {
	       numberofobject--;
	       if(free[whichproc][numberofobject]==true)
		{
		 printf("%s:%d, Error:write on a free object nproc:%d,nobj:%d",
		 type,ID,whichproc,numberofobject);
		 exit(0);
		}
		//Riscale the number of object with the total work
		numberofobject+=(whichproc*maxobj);
	       
	       switch(numberofregister)
		   {
		    case 0: membus.data=reg[numberofobject].size;
		    	break;
		    case 1: membus.data=reg[numberofobject].numberofrow;
		    	break;
		    case 2: membus.data=reg[numberofobject].rowlengthl1;
		    	break;
		    case 3: membus.data=reg[numberofobject].rowlengthl2;
		    	break;
		    case 4: membus.data=reg[numberofobject].l1;
		    	break;
		    case 5: membus.data=reg[numberofobject].l2;
		    	break;
		    case 6: membus.data=(sc_uint<32>)reg[numberofobject].state;
			break;
	            }
	      }
	      
	      TRACEX(WHICH_TRACEX, 10,
	       "%s:%d: Read word at address:0x%x,register:0x%x.%x,value:0x%x\n",
	       type,ID, membus.address, numberofobject, numberofregister, membus.data);
	    }
       
       if (changestate)
       {
        if (nwork==(maxobj*numproc)) 
        {printf("%s:%d: Too many works to do!\n",type,ID);
	 exit(1);
	}
	else
	//new work in queue
	{nwork++;
	 work[freepos]=numberofchange;
	 freepos++;
	 if (freepos==maxobj*numproc) freepos=0;
	}
       }
      }//end bus_enable 	 
     
     }//end requestdma
    
  if (request && endtransfer)
    {if(nwork!=0)
      {requestdmatransfer.write(true);
       now.work=reg[work[working]];
       now.num_obj=work[working];
       datadma.write(now);
       
       TRACEX(WHICH_TRACEX, 8,"%s:%d,%d Startwork1:%x||%d\n",
       type,ID,(uint)reg[work[working]].nproc,(uint)work[working],work[working]%maxobj); 

       readydma.write(true);	    
       dmapin.write(membus);	
       wait();
       readydma.write(false);
       requestdmatransfer.write(false);
      } 
      else
        {readydma.write(true);
         dmapin.write(membus);
         wait();
         readydma.write(false);
	}
 
     }
      else 
         if(request)
           {
	    if(nwork==1 && changestate)
	    {requestdmatransfer.write(true);
	     now.work=reg[work[working]];
             now.num_obj=work[working];
       	     datadma.write(now);
	     
	     TRACEX(WHICH_TRACEX, 8,
	     "%s:%d Startwork2 proc:%d abs_obj:%x,rel_obj:%d,a1:%x,a2:%x\n",
              type,ID,(uint)now.work.nproc,(uint)now.num_obj,(uint)now.num_obj%maxobj,
              (uint)now.work.l1,(uint)now.work.l2);

	     readydma.write(true);
             dmapin.write(membus);
             wait();
             readydma.write(false);
	     requestdmatransfer.write(false);
	    }
	    else
	    {readydma.write(true);
             dmapin.write(membus);
             wait();
             readydma.write(false);
	    }
	   }
	   else
	     if(endtransfer && nwork!=0)
               {
	        requestdmatransfer.write(true);	    
	        now.work=reg[work[working]];
                now.num_obj=work[working];
                datadma.write(now);
		
		TRACEX(WHICH_TRACEX, 8,"%s:%d,%d Startwork3:%x||%d\n",
                 type,ID,(uint)reg[work[working]].nproc,(uint)work[working],
		 work[working]%maxobj);

                wait();
                readydma.write(false);
                requestdmatransfer.write(false);
               }
	            
 }//end while

}
