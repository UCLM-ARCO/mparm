///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         dmatransfer.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Implements a DMA controller (bus side)
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DMATRANSFER_H__
#define __DMATRANSFER_H__

#include <systemc.h>
#include "dmacontrol.h"
#include "address.h"
#include "debug.h"

SC_MODULE(dmatransfer)
{
  
  sc_in_clk clock;
  //signals for the dma control
  sc_in<DMA_CONT_REG1> datadma;
  sc_out<bool> finished;
  sc_in<bool> requestdmatransfer;
 
  SC_HAS_PROCESS(dmatransfer);
  
  dmatransfer(sc_module_name nm, uint16_t id, uint32_t dimburst, uint32_t obj, uint32_t nproc) :
    sc_module(nm)
  {
    ID = id;
   
    maxobj = obj;
    numproc=nproc;
    DIM_BURST=dimburst;
    buffer=(uint32_t*) malloc (sizeof(uint32_t)*DIM_BURST*2);
    
    WHICH_TRACEX=IDMA_TRAN_TRACEX;
    
    SC_CTHREAD(simuldmatransfer, clock.pos());
  }
  
 protected:
  
  DMA_CONT_REG1 work;
  uint16_t ID;
  uint32_t WHICH_TRACEX;
  char* type;
  //pointer to the internal buffer
  uint32_t *buffer;
  uint32_t maxobj,numproc,DIM_BURST;
  //Base function which organize transfer
  void simuldmatransfer();
  //matrix reoganization transfer
  void matrix_reorg();
  
  virtual bool Write(uint32_t addr, uint32_t* data, uint32_t nburst)=0;
  virtual bool Read(uint32_t addr, uint32_t* data, uint32_t nburst)=0;
  virtual bool local(uint32_t addr)
  {return false;};
  virtual void send_int(DMA_CONT_REG1 work)
  {
   printf("%s:%d Warning you are using a state that imply send int but INT is not defined...\n",
   type,ID);
  };
 
   //Base transfer function can be variable fro different kind of interconnection
   virtual void execute_transf()
   {
   uint i,j,size1,dimburst,dimburst1;
   uint address1,address2,rowlengthl1,rowlengthl2;

   //look for the biggest allowed size of the burst cycle
   if (work.work.size<DIM_BURST)
   {dimburst=4;
    for(i=DIM_BURST;i>4;i/=2)
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
	    "\n%s %d: Start transfer from:0x%x to:0x%x\n-Size:%d-Dimburst:%d\n"
	    ,type,ID,(uint)address2,(uint)address1,(uint)work.work.size,dimburst);
	    
	   for (j=0;j<work.work.numberofrow;j++)
	   {dimburst1=dimburst;
	    size1=work.work.size;
	    while(size1 >= 4)
	    {if (size1 < dimburst1) dimburst1/=2;
	     size1-=dimburst1;
             
	     Read(address2,buffer,dimburst1);

	     //transfer data back
	     Write(address1,buffer,dimburst1);
	      
	      address2+=4*dimburst1;
	      address1+=4*dimburst1;
        
	     TRACEX(WHICH_TRACEX, 10,"%s %d burst:%x--",type,ID,address2);
	     #ifdef  OUTDEBUG
	      for (i=0;i<dimburst1;i++)
	     #endif  
	       TRACEX(WHICH_TRACEX, 10,"%x--",(uint)buffer[i]);
	       TRACEX(WHICH_TRACEX, 10,"\n");
	    }
	    //end of burst
            if(size1!=0)
	      {
	       Read(address2,buffer,size1);
	       Write(address1,buffer,size1);
	       address2+=4*size1;
	       address1+=4*size1;
	       
	       TRACEX(WHICH_TRACEX, 10,"%s %d single read1:%x,data:",type,ID,address2-4);
	       #ifdef  OUTDEBUG 
		for (i=0;i<size1;i++)
	       #endif
	        TRACEX(WHICH_TRACEX, 10,"%x--",(uint)buffer[i]); 
		TRACEX(WHICH_TRACEX, 10,"\n");

               size1=0;
	      }
	      
           if (rowlengthl1!=work.work.size && rowlengthl1!=0) 
	     address1+=(4*rowlengthl1)-work.work.size*4;
	   if (rowlengthl2!=work.work.size && rowlengthl2!=0) 
	      address2+=(4*rowlengthl2)-work.work.size*4;  
	  }//end of for
	  
	     TRACEX(WHICH_TRACEX, 8,"%s %d: End transfer from:0x%x to:0x%x\n",
	     type,ID,(uint)work.work.l2,(uint)work.work.l1);

  }; //end of execute_transfer()
  
 
};

#endif // __DMATRANSFER_H__
