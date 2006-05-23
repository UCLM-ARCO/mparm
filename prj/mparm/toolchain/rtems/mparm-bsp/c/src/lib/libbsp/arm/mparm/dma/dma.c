/*-------------------------------------------------------------------------+
| console.c - ARM BSP 
+--------------------------------------------------------------------------+
| This file contains the SWARM dma I/O package.
+--------------------------------------------------------------------------+
|  POLETTI FRANCESCO
+--------------------------------------------------------------------------*/

#include <bsp.h>
#include <rtems/bspIo.h>
#include <rtems/libio.h>
#include <termios.h>
#include <malloc.h>
#include <stdio.h>
		
void answer1();
void answer2();
void answer3();
void answer4();

void answer5();
void answer6();
void answer7();
void answer8();

/*inizialized in the file main.c*/		
volatile rtems_unsigned32** smartmem;
/*volatile uint** queue;*/
volatile rtems_unsigned32** scratch;
volatile rtems_unsigned32** dma_base;		
uint max_obj=4; /*8 maximun_number of obj for each processor in the smartdma */
uint RTEMS_SMART_DMA_SIZE;
uint** Task_id_sus;
rtems_name **semaphore_name;
rtems_status_code status;
rtems_id**  semaphore_id;

/*
Dma 0 non ha concetto del multiprocessore: e' dedicato al singolo processore
Dma 1 multiprocessor
*/

inline int newobject(uint dma,uint sizerow,uint numberofrow,uint rl1,uint rl2,uint l1add,uint
l2add,uint state, uint wait)
{int object;

  /* ASK FOR A NEW OBJECT POSITION */
  if (dma>=0 && dma<=N_DMA)
  {
   object= *(dma_base[dma]);
  }
  else
  {
   printk("\nDMA Driver:: you are trying to access a device which does not exist!!!\n");
   return -1;
  }
  
  if(object== 0xFFFFFFFF) return -1;
  
  /*size of one row in the block*/
  *(volatile uint*)(dma_base[dma]+(1+7*object))=sizerow;
  /*numerofrow*/
  *(volatile uint*)(dma_base[dma]+(2+7*object))=numberofrow;
  /*rowlenghtl1*/
  *(volatile uint*)(dma_base[dma]+(3+7*object))=rl1;
  /*rowlenghtl2*/
  *(volatile uint*)(dma_base[dma]+(4+7*object))=rl2;
  /*address in l1*/
  *(volatile uint*)(dma_base[dma]+(5+7*object))=(uint)l1add; 
  /*address in l2*/
  *(volatile uint*)(dma_base[dma]+(6+7*object))=(uint)l2add;
  
  /*state*/
  if( ( (state>=4) && (state<=5) ) || (state==7) || (state==9))
  {
   newstateint(dma,state,object,wait);
  }
  else
   newstate(dma,state,object,wait);
 
  return object;
}

inline void newstate(register uint dma,register uint state,register int object,
register rtems_boolean wait_end)
{ 
 register int a;
 *(dma_base[dma]+(7+7*object))=state;
 if(wait_end)
 {
  if(state<=3)
   wait(dma,object);
 else
  if(state<=5)
   waitint(dma,object);
 }  
 
}

inline void newstate1(register uint dma,register uint state,register int object)
{
 *(dma_base[dma]+(7+7*object))=state;
}

inline void newstateint(register uint dma,register uint state,register int object,register rtems_boolean wait)
{
 register uint obj;
 
 #if 0
 if( ( (state>=4) && (state<=5) ) || (state==7) || (state==9))
 {
  if(dma==0)
  {printk("Interrupt not yet supported for internal dma...\n");
   exit(0); 
  }
 #endif
  
  obj=object%max_obj;
  
#if 0  
  printk("suspend int dev:%x, obj:%x\n",dma,obj);  
#endif
  
  Task_id_sus[dma][obj]=1;

  *(dma_base[dma]+(7+7*object))=state;
  
  if(wait)
    waitint(dma,object); 
}

inline void newstateint1(register uint dma,register uint state,register int object)
{
  register uint obj;
  
  obj=object%max_obj;

#if 0  
  printk("new state_int:%x, obj:%x\n",dma,obj);  
#endif
  
  Task_id_sus[dma][obj]=1;

  *(dma_base[dma]+(7+7*object))=state;
}

inline void wait(register uint dma,register int object)
{ 
 register uint a;
 do
 {
  a=*(dma_base[dma]+(7+7*object));
 }while(a!=0 && a!=2);
}

inline void waitint(register uint dma,register int object)
{register uint obj;
 register uint a;
#if 0
 if(dma==0)
 {printk("Interrupt not yet supported for internal dma...\n");
  exit(0); 
 }
#endif
 
 obj=object%max_obj;
  
  while(1)
  {
#if defined (EXT_DMA_SEMAPHORE)  
   status = rtems_semaphore_obtain(semaphore_id[dma][obj],
  				  RTEMS_WAIT,
				  RTEMS_NO_TIMEOUT
				 );
#endif

#if 0				 
   printk("suspend int dev:%x, obj:%x\n",dma,obj);
#endif

#if 1
   for(a=0;a<2;a++);
#endif   
   a=Task_id_sus[dma][obj];
   if (a!=1) break;

#if defined (EXT_DMA_SEMAPHORE)   				  		 				
   status = rtems_semaphore_release(semaphore_id[dma][obj]);
#endif

  };
 
}

inline uint return_state(register uint dma,register int object)
{ 
  register uint a;
  a=dma_base[dma][7+7*object];

  return a;
}

inline void newsrow(register uint dma,register uint parameter,register int object)					
{
 *(dma_base[dma]+(1+7*object))=parameter;
}
inline void newnrow(register uint dma,register uint parameter,register int object)
{
 *(dma_base[dma]+(2+7*object))=parameter;
}
inline void newrl1(register uint dma,register uint parameter,register int object)
{
 *(dma_base[dma]+(3+7*object))=parameter;
}
inline void newrl2(register uint dma,register uint parameter,register int object)
{ 
 *(dma_base[dma]+(4+7*object))=parameter;
}
inline void newl1add(register uint dma,register uint parameter,register int object)
{ 
 *(dma_base[dma]+(5+7*object))=parameter;
}
inline void newl2add(register uint dma,register uint parameter,register int object)
{ 
 *(dma_base[dma]+(6+7*object))=parameter;
}

inline void free_dma(uint dma,int object)
{
 *(dma_base[dma])=object;
}

/*##########DMA INITIALIZE###########*/
inline void dma_driver_initialize()
{
  int i,j; 
  char prevbuffer[5];
  
  /*RTEMS_SMART_DMA_SIZE=(10*(4+7*4*max_obj))+0x10;*/
  
  RTEMS_SMART_DMA_SIZE=0x500;
  
  printf("\nInitialize driver dma\n");
   
  /*arm_bare_bsp*/
  dma_base=(uint**) malloc (N_DMA *sizeof(uint*));
  dma_base[0]=(uint*)(INT_DMA_BASE_ADDRESS); 
  
  /*The first DMA is the local one from the second is for the smartmem*/
  for(i=1;i<N_DMA;i++) 
  {
   dma_base[i]=(uint*)(SMART_DMA_BASE_ADDRESS + SMARTMEM_SPACING *(i-1) +
   			        ((get_id()-1)*(28*max_obj+4)) );
  } 
  /*
  queue is an array of pointer to the semaphore of the queue system,
  queue[i] is the base adddress of the queue local to the processor "i"*/
  /*queue= (volatile uint**) malloc (get_tot()*sizeof(uint*));*/
  
  /*
  scratch is an array of pointer to the local scratch for every processor
  scratch[i] is the base address of the scratch-pad local to the core "i"
  
  */
  scratch= (volatile uint**) malloc (get_tot()*sizeof(uint*));
  
  for(i=0;i<get_tot();i++) 
  {
   scratch[i]=(volatile uint*) (CORESLAVE_BASE+i*CORESLAVE_SPACING);
   /*queue[i]=(volatile uint*) ((uint)scratch[i]+RTEMS_SCRATCH_SIZE);   
   */
  }
  
  smartmem=(volatile uint**)malloc((N_DMA-1)*sizeof(uint*));
  
  for(i=0;i<N_DMA-1;i++)
  {
   smartmem[i]=(volatile uint*)(SMART_DMA_BASE_ADDRESS+RTEMS_SMART_DMA_SIZE+SMARTMEM_SPACING*i);
  }
  /*
  offset[i] indicate the offset to the realated dma from the base address to the start of the
  first object address space
  
  offset1[i] is different for every code and indicate the base address of the dma in the case
  we have a multiprocessor 
  */
  
  /*
  offset= (volatile uint*) malloc (N_DMA *sizeof(uint));
  offset1= (volatile uint*) malloc (N_DMA *sizeof(uint));
  
  offset[0]=1;
  offset1[0]=0;
  
  for(i=1;i<N_DMA;i++) 
  {
   offset[i]=(volatile uint)(get_tot());
   offset1[i]=(volatile uint)(get_id()-1);
  }
  */
  
  Task_id_sus=(uint**) malloc(N_DMA*sizeof(uint*));  
  semaphore_name=(rtems_name**) malloc(N_DMA*sizeof(rtems_name*));
  semaphore_id=(uint**) malloc(N_DMA*sizeof(uint*)); 
    
  for(j=0;j<N_DMA;j++)
  {  
   Task_id_sus[j]=(uint*) malloc(max_obj*sizeof(uint)); 
   semaphore_name[j]=(rtems_name*) malloc(max_obj*sizeof(rtems_name));
   semaphore_id[j]=(uint*) malloc(max_obj*sizeof(uint));
    
    for(i=0;i<max_obj;i++) 
    { 
     sprintf(prevbuffer,"%d",i);   
   
     semaphore_name[j][i] = rtems_build_name( 'S', 'E', prevbuffer[0], ' ' );
     
    } 
  }
}  

/*#########INTERRUPT############*/
inline void answer(int j)
{ 
#if defined (EXT_DMA_SEMAPHORE)
 status = rtems_semaphore_obtain(semaphore_id[(j/max_obj)+1][j%max_obj],
  				  RTEMS_WAIT,
				  RTEMS_NO_TIMEOUT
				 );
#endif

#if 0 
 printk("rispondo int dev:%x, obj:%x\n",((j/max_obj)+1),(j%max_obj)); 
#endif

#if 0								 
 if (Task_id_sus[(j/max_obj)+1][j%max_obj]==0) printk("Oh,Oh...\n"); 
 else
#endif 
 {
  Task_id_sus[(j/max_obj)+1][j%max_obj]=0;			 				 
 }
 
#if defined (EXT_DMA_SEMAPHORE)
 status = rtems_semaphore_release(semaphore_id[(j/max_obj)+1][j%max_obj]);
#endif

}

inline void dmaon (const rtems_irq_connect_data* dma)
{
 son(dma); /*abilito l'int nell'int controller*/
}
inline void dmaoff (const rtems_irq_connect_data* dma)
{
 soff(dma); /*disabilito l'int nell'int controller*/
}
inline int dmaison (const rtems_irq_connect_data* dma)
{return (sison(dma));
}

inline void answer1()
{
 answer(0);
}
inline void answer2()
{
 answer(1);
}
inline void answer3()
{
 answer(2);
}

inline void answer4()
{
 answer(3);
}

inline void answer5()
{
 answer(4);
}

inline void answer6()
{
 answer(5);
}

inline void answer7()
{
 answer(6);
}

inline void answer8()
{
 answer(7);
}

#ifndef RTEMS_CORESLAVE_INT
static rtems_irq_connect_data dma_int_data1 = {  int22,
  						answer1,
  						dmaon,
  						dmaoff,
  						dmaison,
  						0,
  						0
					      };

static rtems_irq_connect_data dma_int_data2 = {  int21,
  						answer2,
  						dmaon,
  						dmaoff,
  						dmaison,
  						0,
  						0
					      };

static rtems_irq_connect_data dma_int_data3 = {  int20,
  						answer3,
  						dmaon,
  						dmaoff,
  						dmaison,
  						0,
  						0
					      };

static rtems_irq_connect_data dma_int_data4 = {  int19,
  						answer4,
  						dmaon,
  						dmaoff,
  						dmaison,
  						0,
  						0
					      };

static rtems_irq_connect_data dma_int_data5 = {  int18,
  						answer5,
  						dmaon,
  						dmaoff,
  						dmaison,
  						0,
  						0
					      };
					      
static rtems_irq_connect_data dma_int_data6 = {  int17,
  						answer6,
  						dmaon,
  						dmaoff,
  						dmaison,
  						0,
  						0
					      };
					      					  
static rtems_irq_connect_data dma_int_data7 = {  int16,
  						answer7,
  						dmaon,
  						dmaoff,
  						dmaison,
  						0,
  						0
					      };
					      
static rtems_irq_connect_data dma_int_data8 = {  int15,
  						answer8,
  						dmaon,
  						dmaoff,
  						dmaison,
  						0,
  						0
					      };
#else
static rtems_irq_connect_data dma_int_data1 = {  int21,
  						answer1,
  						dmaon,
  						dmaoff,
  						dmaison,
  						0,
  						0
					      };

static rtems_irq_connect_data dma_int_data2 = {  int20,
  						answer2,
  						dmaon,
  						dmaoff,
  						dmaison,
  						0,
  						0
					      };

static rtems_irq_connect_data dma_int_data3 = {  int19,
  						answer3,
  						dmaon,
  						dmaoff,
  						dmaison,
  						0,
  						0
					      };

static rtems_irq_connect_data dma_int_data4 = {  int18,
  						answer4,
  						dmaon,
  						dmaoff,
  						dmaison,
  						0,
  						0
					      };

static rtems_irq_connect_data dma_int_data5 = {  int17,
  						answer5,
  						dmaon,
  						dmaoff,
  						dmaison,
  						0,
  						0
					      };
					      
static rtems_irq_connect_data dma_int_data6 = {  int16,
  						answer6,
  						dmaon,
  						dmaoff,
  						dmaison,
  						0,
  						0
					      };
					      					  
static rtems_irq_connect_data dma_int_data7 = {  int15,
  						answer7,
  						dmaon,
  						dmaoff,
  						dmaison,
  						0,
  						0
					      };
					      
static rtems_irq_connect_data dma_int_data8 = {  int14,
  						answer8,
  						dmaon,
  						dmaoff,
  						dmaison,
  						0,
  						0
					      };
#endif
					      
/*Set the irq_handler on*/
void ext_Setdma()
{  int status,i,j;
  
  printk("Interrupt_dma_handling\n"); 
  
  for(j=0;j<N_DMA;j++)
  {    
    for(i=0;i<max_obj;i++) 
    { 
     status = rtems_semaphore_create(semaphore_name[j][i],
  				  1,
				  RTEMS_BINARY_SEMAPHORE | RTEMS_FIFO,
				  RTEMS_NO_PRIORITY,
				  &semaphore_id[j][i]
  				 );
     
    } 
   }
     
    i=1;
   
    switch(i)
    {
     case 1:
     status = BSP_install_rtems_irq_handler(&dma_int_data1);
     dma_int_data1.on(&dma_int_data1);
     if(i==max_obj)break;
     else i++;
     
     case 2:
     status = BSP_install_rtems_irq_handler(&dma_int_data2);
     dma_int_data2.on(&dma_int_data2);
     if(i==max_obj)break;
     else i++;
     
     case 3:
     status = BSP_install_rtems_irq_handler(&dma_int_data3);
     dma_int_data3.on(&dma_int_data3);
     if(i==max_obj)break;
     else i++;
     
     case 4:
     status = BSP_install_rtems_irq_handler(&dma_int_data4);
     dma_int_data4.on(&dma_int_data4);
     if(i==max_obj)break;
     else i++;     
     
     case 5:
     status = BSP_install_rtems_irq_handler(&dma_int_data5);
     dma_int_data5.on(&dma_int_data5);    
     if(i==max_obj)break;
     else i++;
     
     case 6:
     status = BSP_install_rtems_irq_handler(&dma_int_data6);
     dma_int_data6.on(&dma_int_data6);
     if(i==max_obj)break;
     else i++;
    
     case 7:
     status = BSP_install_rtems_irq_handler(&dma_int_data7);
     dma_int_data7.on(&dma_int_data7);
     if(i==max_obj)break;
     else i++;
     
     case 8:
     status = BSP_install_rtems_irq_handler(&dma_int_data8);
     dma_int_data8.on(&dma_int_data8);

     
    }; 

}
