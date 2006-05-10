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
		
volatile uint* int_dma_base;
 
int int_newobject(uint sizerow,uint numberofrow,uint rl1,uint rl2,uint l1add,uint l2add,uint state, rtems_boolean waitdma)
{
 int object;
   /* ASK FOR A NEW OBJECT POSITION */
  #if defined COMPLETE_DMA
  object= *int_dma_base;
  #else
  object= *(uint*)INT_DMA_BASE_ADDRESS;
  #endif
  
  if(object== 0xFFFFFFFF) return -1;
  
  #if defined COMPLETE_DMA
  /*size of one row in the block*/
  int_dma_base[1+7*object]=sizerow;
  /*numerofrow*/
  int_dma_base[2+7*object]=numberofrow;
  /*rowlenghtl1*/
  int_dma_base[3+7*object]=rl1;
  /*rowlenghtl2*/
  int_dma_base[4+7*object]=rl2;
  /*address in l1*/
  int_dma_base[5+7*object]=(uint)l1add; 
  /*address in l2*/
  int_dma_base[6+7*object]=(uint)l2add;
  /*state*/
  int_dma_base[7+7*object]=state;
  #else
    /*size of one row in the block*/
  *((uint*)INT_DMA_BASE_ADDRESS+1+7*object)=sizerow;
  /*numerofrow*/
  *((uint*)INT_DMA_BASE_ADDRESS+2+7*object)=numberofrow;
  /*rowlenghtl1*/
  *((uint*)INT_DMA_BASE_ADDRESS+3+7*object)=rl1;
  /*rowlenghtl2*/
  *((uint*)INT_DMA_BASE_ADDRESS+4+7*object)=rl2;
  /*address in l1*/
  *((uint*)INT_DMA_BASE_ADDRESS+5+7*object)=(uint)l1add; 
  /*address in l2*/
  *((uint*)INT_DMA_BASE_ADDRESS+6+7*object)=(uint)l2add;
  /*state*/
  *((uint*)INT_DMA_BASE_ADDRESS+7+7*object)=state;
  #endif
  
  if(waitdma)
  int_wait(object);
  
  return object;
}


inline void int_newsrow(register uint parameter,register int object)					
{
 #if defined COMPLETE_DMA
 int_dma_base[1+7*object]=parameter;
 #else
 *((uint*)INT_DMA_BASE_ADDRESS+1+7*object)=parameter;
 #endif
}
inline void int_newnrow(register uint parameter,register int object)
{
 #if defined COMPLETE_DMA
 int_dma_base[2+7*object]=parameter;
 #else
 *((uint*)INT_DMA_BASE_ADDRESS+2+7*object)=parameter;
 #endif
}
inline void int_newrl1(register uint parameter,register int object)
{
 #if defined COMPLETE_DMA
 int_dma_base[3+7*object]=parameter;
 #else
 *((uint*)INT_DMA_BASE_ADDRESS+3+7*object)=parameter;
 #endif
}
inline void int_newrl2(register uint parameter,register int object)
{
 #if defined COMPLETE_DMA
 int_dma_base[4+7*object]=parameter;
 #else
 *((uint*)INT_DMA_BASE_ADDRESS+4+7*object)=parameter;
 #endif
}
inline void int_newl1add(register uint parameter,register int object)
{
 #if defined COMPLETE_DMA
 int_dma_base[5+7*object]=parameter;
 #else
 *((uint*)INT_DMA_BASE_ADDRESS+5+7*object)=parameter;
 #endif
}
inline void int_newl2add(register uint parameter,register int object)
{ 
 #if defined COMPLETE_DMA
 int_dma_base[6+7*object]=parameter;
 #else
 *((uint*)INT_DMA_BASE_ADDRESS+6+7*object)=parameter;
 #endif
}

inline void int_newstate1(register uint state,register int object)
{
 #if defined COMPLETE_DMA
 int_dma_base[7+7*object]=state;
 #else
 *((uint*)INT_DMA_BASE_ADDRESS+7+7*object)=state;
 #endif
}

inline void int_newstate(register uint state,register int object, rtems_boolean waitdma)
{
 register uint a;
 #if defined COMPLETE_DMA
 int_dma_base[7+7*object]=state;
 #else
 *((uint*)INT_DMA_BASE_ADDRESS+7+7*object)=state;
 #endif
 do
 {a=int_dma_base[7+7*object];
 /*
 a=(*((uint*)INT_DMA_BASE_ADDRESS+7+7*object));
 */
 }
 while(a!=0 && a!=2);
}

inline void int_wait(register int object)
{
#if defined COMPLETE_DMA 
 while(int_dma_base[7+7*object]!=0 && int_dma_base[7+7*object]!=2);
#else
register uint a;
do
{a=int_dma_base[7+7*object];
 /*
 a=(*((uint*)INT_DMA_BASE_ADDRESS+7+7*object));
 */
}
while(a!=0 && a!=2);
#endif
}

inline uint int_return_state(register int object)
{ 
  register uint a;
  a=int_dma_base[7+7*object];

  return a;
}

inline void int_free_dma(int object)
{
 #if defined COMPLETE_DMA
 *int_dma_base=object;
 #else
 *(uint*)INT_DMA_BASE_ADDRESS=object;
 #endif
}
