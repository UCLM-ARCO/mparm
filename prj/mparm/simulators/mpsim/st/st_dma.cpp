///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         st_dma.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Implements a complete DMA (bus side)
//
///////////////////////////////////////////////////////////////////////////////

//This Class implement a DMA to be connected to the st bus

#include "st_dma.h"
#include "stats.h"


#define BUS_SIZE 32  //for fix the burst alignment ....
#define BYTE_BUS_SIZE (BUS_SIZE >> 3) //size of the bus in byte
#define ST_MIN_DIM_BURST 4
//this is the parameter used for decide, in read transaction, if we have to perform 
//a burst read even if the size of the transfer is smaller then the dimension of the minum
//burst. It's in word for the case of bus size = 32bit. 
#define ST_READ_WRAP_DIM 1 

bool St_Dma_transfer :: Write(uint32_t addr, uint32_t* data, uint32_t nburst)
{ 
 uint i,burstcounter;
 PINOUT pinout_mast;
 
 TRACEX(WHICH_TRACEX, 12,"%s:%d write addr:%x,nburst:%d\n",
         type,ID,addr,nburst);
 
 if(local(addr))
 {
  for(i=0;i<nburst;i++)
  {
   if (STATS)
     statobject->inspectDMAAccess(addr, false, ID);
   write_local(addr,data[i]);  

   addr+=4;
  } 
 }//end of scratch write
 else
 {
  //setup the bus for the master
  pinout_mast.address = addr;
  pinout_mast.rw = 1; //write
  pinout_mast.bw = 0; //dimension of 32 bit
  pinout_mast.benable = 1;
  

   if(nburst < (uint32_t)ST_MIN_DIM_BURST)
   {
    pinout_mast.burst=1;
    for(i=0;i<nburst;i++)
    {  
     request_to_master.write(true);
     pinout_mast.data=data[i];
     pinout.write(pinout_mast);

     if (STATS)
     statobject->inspectDMAAccess(pinout_mast.address, false, ID);
     
     wait_until(ready_from_master.delayed() == true);
     
     request_to_master.write(false);
     pinout_mast.address+=4;
    }
   } 
  else
  {
   //////////////
   //start burst_write
   pinout_mast.burst = nburst;
   pinout_mast.data = data[0];
   //write the bus for the master
   pinout.write(pinout_mast);
   //request the master
   request_to_master.write(true);
   burstcounter = 0;
	    
   while(burstcounter<nburst)
   {
    TRACEX(WHICH_TRACEX, 12,"%s:%d waiting for the bus\n",type,ID);
    
    if (STATS)
    statobject->inspectDMAAccess(pinout_mast.address+4*burstcounter, false, ID);
    
    if(burstcounter==0)
    {wait(); 
    }
    else
    wait_until(ready_from_master.delayed() == true);
    
    TRACEX(WHICH_TRACEX, 12,"%s:%d I got the the bus\n",type,ID);
 
    burstcounter++;
    if (burstcounter<nburst)
    {//write the data
     pinout_mast.data = data[burstcounter];
     pinout.write(pinout_mast);
    }    
    if (burstcounter==1) request_to_master.write(false);
   }
       
    pinout_mast.data = data[0];
    pinout_mast.address+=4*nburst;

  }//end of the burst write
 }//end of master write

return true;

};

bool St_Dma_transfer :: Read(uint32_t addr, uint32_t* data, uint32_t nburst)
{
 uint i,burstcounter;
 PINOUT pinout_mast; 
 
 TRACEX(WHICH_TRACEX, 12,"%s:%d read addr:%x,nburst:%d\n",
         type,ID,addr,nburst);
 
 if(local(addr))
 {
  for(i=0;i<nburst;i++)
  {
   if (STATS)
     statobject->inspectDMAAccess(addr, false, ID);
   
   data[i]= read_local(addr); 
   //data[i]=scra->Read(addr);
   //wait();
   addr+=4;
   
  }
 }//end of scratch read
 else
 {
  //set the bus for the master
  pinout_mast.address = addr;
  pinout_mast.rw = 0; //read
  pinout_mast.bw = 0;
  pinout_mast.benable = 1;
  
  request_to_master.write(true);
  
  if(nburst < (uint32_t)ST_MIN_DIM_BURST)   
  {
   pinout_mast.burst = 1;
   //write the bus for the master
   pinout.write(pinout_mast);
   
   for(i=0;i<nburst;i++)
   {
    if (STATS)
    statobject->inspectDMAAccess(pinout_mast.address, true, ID);
   
    wait_until(ready_from_master.delayed() == true);
	      
    pinout_mast = pinout.read();
    data[i]=pinout_mast.data; 
    request_to_master.write(false);
    if(i<nburst-1)
    {pinout_mast.address+=4;     
     request_to_master.write(true);
     pinout.write(pinout_mast);
    }
   }    
  }//end of single read
  else
  {
   pinout_mast.burst = nburst;
   //write the bus for the master
   pinout.write(pinout_mast);
   ////////////////
   // starts burst read
   burstcounter=0;
   while(burstcounter<nburst)
	     {
	     
	     TRACEX(WHICH_TRACEX, 12,"%s:%d waiting for the bus\n",type,ID);

	      if (STATS)
		statobject->inspectDMAAccess(pinout_mast.address+4*burstcounter, true, ID);

	      wait_until(ready_from_master.delayed() == true);
             
	     TRACEX(WHICH_TRACEX, 12,"%s:%d I got the the bus\n",type,ID);

	      //read the data
	      pinout_mast = pinout.read();
	      data[burstcounter]=pinout_mast.data;
	      ++burstcounter;
	     }

   pinout_mast.address+=4*nburst;
   request_to_master.write(false);     
  }//end of burst read
   
 }//end of master read
 
 return true;
};

inline void St_Dma_transfer::execute_transf()
{
 uint i,j,size1,dimburst,dimburst2,dimburst1,nread;
 uint address1,address2,rowlengthl1,rowlengthl2;
 uint offset1,offset2,opcdim,remaining_cells1,remaining_cells2; //for burst alignment
  
 offset1=offset2=remaining_cells1=remaining_cells2=opcdim=0;
  
  
  //look for the biggest allowed size of the burst cycle
  if (work.work.size<DIM_BURST)
  {dimburst=ST_MIN_DIM_BURST;
   for(i=DIM_BURST;i>ST_MIN_DIM_BURST;i/=2)
    {if (i<=work.work.size) 
      {
       dimburst=i;
       break;
      }
    }
  }
  else
   {dimburst=DIM_BURST;
   }
     
   //start of the transfer
   
	    switch(work.work.state)
	    {
	     case 1:
	     if(numproc==1)
	     {address2 = work.work.l1 & 0xFFFFFFFC; //data source address	     
	      address1 = work.work.l2 & 0xFFFFFFFC; //destination data address
	     }
	     else
	     {
	      if ((uint)work.work.nproc>numproc)
	      {
	       printf("%s:%d LOGICAL2 nproc:%x\n",type,ID,(uint)work.work.nproc);
	       exit(0);
	      }
	      address2 = ((addresser->Logical2Physical((uint)work.work.l1,(uint)work.work.nproc))& 0xFFFFFFFC);
	      address1 = ((addresser->Logical2Physical((uint)work.work.l2,(uint)work.work.nproc))& 0xFFFFFFFC);
	      //printf("address1:%x,address2:%x,nproc:%x\n",(uint)address1,(uint)address2,(uint)work.work.nproc); 
	     }
	     rowlengthl2 = work.work.rowlengthl1;
	     rowlengthl1 = work.work.rowlengthl2;
             break;
	     
	     case 3:
             if(numproc==1)
	     {address2 = work.work.l2 & 0xFFFFFFFC; //data source address     
	      address1 = work.work.l1 & 0xFFFFFFFC; //destination data address
	     }
	     else
	     {
	      if ((uint)work.work.nproc>numproc)
	      {
	       printf("%s:%d LOGICAL2 nproc:%x\n",type,ID,(uint)work.work.nproc);
	       exit(1);
	      }
	      address2 = ((addresser->Logical2Physical(work.work.l2,work.work.nproc))& 0xFFFFFFFC);
	      address1 = ((addresser->Logical2Physical(work.work.l1,work.work.nproc))& 0xFFFFFFFC);
	      //printf("address1:%x,address2:%x,nproc:%x\n",(uint)address1,(uint)address2,(uint)work.work.nproc);
	     }
	     
	     rowlengthl2 = work.work.rowlengthl2;
	     rowlengthl1 = work.work.rowlengthl1;
	     break;
	     
	     default :
	     printf("%s %d wrong state:%d",type,ID,(uint)work.work.state);
	     exit(1);
	    }

            TRACEX(WHICH_TRACEX, 8,
	     "\n%s %d: Start transfer from:0x%x to:0x%x\n-Size:%d-Dimburst:%d\n",
              type,ID,(uint)address2,(uint)address1,(uint)work.work.size,dimburst);

	local1=local(address1);
	local2=local(address2);
	
	if(dimburst==4) opcdim=1<<4;
	  else 
	    if(dimburst==8) opcdim=1<<5;    
	    
	   for (j=0;j<work.work.numberofrow;j++)
	   {dimburst1=dimburst;
	    dimburst2=dimburst;
	    size1=work.work.size;
            nread=0;

            if(size1<=ST_READ_WRAP_DIM)
	    {
	     Read(address2,buffer,size1);
	     
	     TRACEX(WHICH_TRACEX, 10,
	     "%s %d single read1,n_read:%d,add:%x,data:",type,ID,size1,address2);
	       #ifdef  OUTDEBUG 
		for (i=0;i<size1;i++)
	       #endif
               TRACEX(WHICH_TRACEX, 10,"%x--",(uint)buffer[i]);
	       TRACEX(WHICH_TRACEX, 10,"\n");
	     
             TRACEX(WHICH_TRACEX, 10,
	     "%s %d single write1,n_write:%d add:%x data:",type,ID,size1,address1);
    
	     Write(address1,buffer,size1);	        

	       #ifdef  OUTDEBUG 
	       for (i=0;i<size1;i++)
	       #endif
               TRACEX(WHICH_TRACEX, 10,"%x--",(uint)buffer[i]);
	       TRACEX(WHICH_TRACEX, 10,"\n");
	     
	     address1+=size1*BYTE_BUS_SIZE;
	     address2+=size1*BYTE_BUS_SIZE;
	    }
	    else
	    {
////////////
///address realignment calc	    
  
            //write part 
            if(local1 || size1<=1) 
	     {
	      offset1=0;
              remaining_cells1=0;
	     }
	     else
             {offset1=address1%opcdim;
              if(offset1)
              {remaining_cells1 = dimburst-(offset1/BYTE_BUS_SIZE);	       
	      }
             }
            
	    //read part
	    if(local2 || size1<=1) offset2=0;
            else
            {offset2=address2%opcdim; 
             if(offset2)
             {remaining_cells2 = dimburst-(offset2/BYTE_BUS_SIZE);

              TRACEX(WHICH_TRACEX, 10,
	      "%s %d unaligned read,n_read:%d,add:%x,data:",type,ID,remaining_cells2,address2);

              if(remaining_cells2>ST_MIN_DIM_BURST)
              { 
               if((remaining_cells2-ST_MIN_DIM_BURST)<=ST_READ_WRAP_DIM)
	        Read(address2,buffer,(remaining_cells2-ST_MIN_DIM_BURST));
	       else
	        Read(address2,buffer,ST_MIN_DIM_BURST); 
		address2+=(BYTE_BUS_SIZE*(remaining_cells2-ST_MIN_DIM_BURST));
		       
	       Read(address2,&buffer[remaining_cells2-ST_MIN_DIM_BURST],ST_MIN_DIM_BURST);
	        address2+=(BYTE_BUS_SIZE*ST_MIN_DIM_BURST);      
	      }
	      else
	       {
		if((remaining_cells2-ST_MIN_DIM_BURST)<=ST_READ_WRAP_DIM)
		 Read(address2,buffer,remaining_cells2);
	        else
		 Read(address2,buffer,ST_MIN_DIM_BURST);
		address2+=BYTE_BUS_SIZE*remaining_cells2;	       
	       }
	      
	      for(i=0;i<remaining_cells2;i++)
	        buffer1->push_front(buffer[i]);

	       #ifdef  OUTDEBUG 
	       for (i=0;i<remaining_cells2;i++)
	       #endif
               TRACEX(WHICH_TRACEX, 10,"%x--",(uint)buffer[i]);
	       TRACEX(WHICH_TRACEX, 10,"\n");		
	    	       	       
	      size1-=remaining_cells2;
	      
	     }
            }

////end of realignment calc
////////////	    
	  
	  
	  
	  while(1)  
	   { 
	    ////////read part
	    if(size1 >= ST_MIN_DIM_BURST)
	    {if (size1 < dimburst1) dimburst1/=2;
	     
	     size1-=dimburst1;
             
	     TRACEX(WHICH_TRACEX, 10,
	     "%s %d burst read,nburst:%d add:%x data:",type,ID,dimburst1,address2);

	     Read(address2,buffer,dimburst1);
	     
	     for(i=0;i<dimburst1;i++)
	       buffer1->push_front(buffer[i]);

	       #ifdef  OUTDEBUG 
	       for (i=0;i<dimburst1;i++)
	       #endif
               TRACEX(WHICH_TRACEX, 10,"%x--",(uint)buffer[i]);
	       TRACEX(WHICH_TRACEX, 10,"\n");
	       		   
	     address2+=4*dimburst1;
	    
	    }
	    else
	     {//end of burst
              if(size1!=0)
	      {

               TRACEX(WHICH_TRACEX, 10,
	       "%s %d single read,n_read:%d,add:%x,data:",type,ID,size1,address2);	       

	      if(size1<=ST_READ_WRAP_DIM)
	        Read(address2,buffer,size1);
	      else
	        Read(address2,buffer,ST_MIN_DIM_BURST);
		  
	        for(i=0;i<size1;i++)
	          buffer1->push_front(buffer[i]);

	       #ifdef  OUTDEBUG 
	       for (i=0;i<size1;i++)
	       #endif
               TRACEX(WHICH_TRACEX, 10,"%x--",(uint)buffer[i]);
	       TRACEX(WHICH_TRACEX, 10,"\n");	       	       	       	     
               
	       address2+=BYTE_BUS_SIZE*size1;

               size1=0;
	      }
	     }
	    ////////end of read part

	    ///////write part	    
	    if(offset1)  
	    {
	     if(buffer1->size()>=remaining_cells1 )
	     {
	      for(i=0;i<remaining_cells1;i++)
	         { 
		  buffer[i]=buffer1->back();
		  buffer1->pop_back();
	         }

              TRACEX(WHICH_TRACEX, 10,
	      "%s %d unaligned write add:%x,n_write:%d,data:",type,ID,address1,remaining_cells1);    

	     if(remaining_cells1>ST_MIN_DIM_BURST)
	     {
              Write(address1,buffer,(remaining_cells1-ST_MIN_DIM_BURST));
	       address1+=(BYTE_BUS_SIZE*(remaining_cells1-ST_MIN_DIM_BURST));
               
	      Write(address1,&buffer[remaining_cells1-ST_MIN_DIM_BURST],ST_MIN_DIM_BURST); 
	       address1+=BYTE_BUS_SIZE*ST_MIN_DIM_BURST;      
	     }
	     else     
	     {
	      Write(address1,buffer,remaining_cells1);
	       address1+=BYTE_BUS_SIZE*remaining_cells1;     
	     }  

	       #ifdef  OUTDEBUG 
	       for (i=0;i<remaining_cells1;i++)
	       #endif
               TRACEX(WHICH_TRACEX, 10,"%x--",(uint)buffer[i]);
	       TRACEX(WHICH_TRACEX, 10,"\n");	       	       
			       
	      offset1=0;
	     }	     
	    }//end of offset 1 
	    else
	     {
	      if(buffer1->size()>=dimburst2 && dimburst2!=1)
	      {
	       //printf("Normal write size:%d dimburst:%d\n",buffer1->size(),dimburst2);
	       
	       #if 1
	       for(i=0;i<dimburst2;i++)
	         { 
		  buffer[i]=buffer1->back();
		  buffer1->pop_back();
		 }
	       #endif	 
	
               TRACEX(WHICH_TRACEX, 10,"%s %d burst write,nburst:%d,address:%x--",
	       type,ID,dimburst2,address1);		 

	      Write(address1,buffer,dimburst2);

	       #ifdef  OUTDEBUG 
	       for (i=0;i<dimburst2;i++)
	       #endif
               TRACEX(WHICH_TRACEX, 10,"%x--",(uint)buffer[i]);
	       TRACEX(WHICH_TRACEX, 10,"\n");       
	       
	       address1+=BYTE_BUS_SIZE*dimburst2;
	      
	      }
	      else
	       if(dimburst2==1 && size1==0)
	       {
	        uint temp_dim=buffer1->size();
	        for(i=0;i<temp_dim;i++)
	          { 
		   buffer[i]=buffer1->back();
		   buffer1->pop_back();
	          }

                TRACEX(WHICH_TRACEX, 10,
		"%s %d single write,n_write:%d add:%x data:",type,ID,temp_dim,address1);		  	
			        
		Write(address1,buffer,temp_dim);
	       
	       #ifdef  OUTDEBUG 
	       for (i=0;i<temp_dim;i++)
	       #endif
               TRACEX(WHICH_TRACEX, 10,"%x--",(uint)buffer[i]);
	       TRACEX(WHICH_TRACEX, 10,"\n");	         
		 
		address1+=BYTE_BUS_SIZE*temp_dim;
		
		break;
	       }
	       
	       if((buffer1->size()==0 && size1==0))
	       {
		break;
	       }
	       else
	        {//dimburst write recomputation
		 if((buffer1->size()+size1)<ST_MIN_DIM_BURST)
	         {
	          dimburst2=1;
	         }
	         else 
	          if((buffer1->size()+size1)<dimburst2)
	          {
	           if(dimburst2==4) 
		   {
		    dimburst2=1;
		   }
		   else dimburst2/=2;
	          }
		}
	     }	     
	    ///////end of write part
	    
	      
	   }//end of while(1)	  
	  }//end of complex access	  
	  //address recomputation    
           if (rowlengthl1!=work.work.size && rowlengthl1!=0) 
	     address1+=(4*rowlengthl1)-work.work.size*4;
	   if (rowlengthl2!=work.work.size && rowlengthl2!=0) 
	      address2+=(4*rowlengthl2)-work.work.size*4;  
	  }//end of for

           TRACEX(WHICH_TRACEX, 8,"%s %d: End transfer from:0x%x to:0x%x\n",
	   type,ID,(uint)work.work.l2,(uint)work.work.l1);

};
