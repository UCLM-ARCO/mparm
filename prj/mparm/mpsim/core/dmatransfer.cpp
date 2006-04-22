///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
//
// name         dmatransfer.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Implements a DMA controller (bus side)
//
///////////////////////////////////////////////////////////////////////////////

#include "dmatransfer.h"
#include "stats.h"
#include "globals.h"
#include "mem_class.h"

void dmatransfer::simuldmatransfer()
{uint i;
 bool first=true;
 
 finished.write(false);
 
 while(1)
 {for (i=0;i<DIM_BURST;i++)
  buffer[i]=0;  
  //wait_until(requestdmatransfer.delayed() == true);
  do
  {
   wait();
   if(first)
   {
    finished.write(false);
    first=false;
   }
  }while(requestdmatransfer.read() == false);

  TRACEX(WHICH_TRACEX, 10,"%s %d access\n",type ,ID);

  work=datadma.read(); 
  if(work.work.data_transf==1) 
  {
   matrix_reorg();
  }
  else
  {execute_transf();
  }
  
  finished.write(true);
  if (work.work.send_int) 
  {
   TRACEX(WHICH_TRACEX, 8,"%s:%d send_int:%d\n",type,ID,(uint)work.work.nproc);

   send_int(work); 
  }
  first=true;
 }//end while
}

inline void dmatransfer::matrix_reorg()
{
 uint i,row,col,send,temp;
   if(work.work.state==3)
   {
    printf("HEy\n");
    exit(0);
   }
   else
   {
    col=work.work.size;
    row=work.work.numberofrow;
    TRACEX(WHICH_TRACEX, 8,"%s:%d col:%u,row:%u\n",type,ID,col,row);
      
    work.work.size=1;  //I'm transferring one column in row-major matrix
    temp=work.work.rowlengthl2;
    work.work.rowlengthl2=0; //This avoid increment in the matrix destination 
    send=work.work.send_int;
    work.work.send_int=false;

    for(i=0;i<col;i++)
    {
     TRACEX(WHICH_TRACEX, 8,
           "%s:%d size:%u,nrow:%u,rl1:%u,rl2:%u,l1:0x%x,l2:0x%x,\n",
	    type,ID,(uint)work.work.size,(uint)work.work.numberofrow,
	    (uint)work.work.rowlengthl1,(uint)work.work.rowlengthl2,
	    (uint)work.work.l1,(uint)work.work.l2);
     
     if(i==col-1 && send) work.work.send_int=true;
      execute_transf();
     work.work.l1+=4;
     if (temp!=work.work.size && temp!=0) 
       work.work.l2 +=(4*(temp+1))-work.work.size*4; 
    }
   }
}   
