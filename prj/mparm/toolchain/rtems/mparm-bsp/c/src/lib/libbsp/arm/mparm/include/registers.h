/*
 *  BSP registers declaration
 *
 * Copyright (c) 2000 Canon Research France SA.
 * Emmanuel Raguet, mailto:raguet@crf.canon.fr
 *
 * The license and distribution terms for this file may be
 * found in found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 *
 */


#ifndef  __REGS_H__
#define __REGS_H__  

/*
 *  PARAMETERS FOR THE BOARD COMPILATION
 */

/*This macro return the ID of a core*/  
#define get_id() \
({ rtems_unsigned32 result; __asm__ volatile ("mrc 15, 0, %0, c11, c0, 3" : "=r" (result) :); (result=result+1); }) 

/*This macro return the ID of a core*/  
#define get_tot() \
({ rtems_unsigned32 result; __asm__ volatile ("mrc 15, 0, %0, c11, c0, 4" : "=r" (result) :); result; }) 
 
/*This define sets some prints thrown to the serial for the boot of the OS*/ 
#define PRINTDEBUGINFO

/*This define sets some prints thrown to the serial for the boot of the OS regarding
the multiprocessor communication*/
/*#define PRINTDEBUGINFOMULTI*/

/*Base address of the interrupt slave*/
#define SLAVE1_BASE_ADDRESS 0x21000000

/*Base address for the shared memory*/
#define SHARED_BASE_ADDRESS 0x19000000

/*Size of the shared memory*/
#define RTEMS_SHARED_SIZE (600 * 1024)

/*Base address of the semaphore*/
#define SEMAPHORE_BASE_ADDRESS 0x20000000

extern volatile rtems_unsigned32* int_dma_base;
extern volatile rtems_unsigned32** dma_base;
extern volatile rtems_unsigned32** smartmem;
/*extern volatile rtems_unsigned32** queue;*/
extern volatile rtems_unsigned32** scratch;

/*Base address of the scratch-pad memory*/
#define SCRATCH_BASE_ADDRESS   	0x22000000
/*Size of the scratch-pad memory*/
/*#define RTEMS_SCRATCH_SIZE  	0x00010000*/

#define RTEMS_SCRATCH_SPACING  	0x00020000

/*This enable the interrupt manager of the coreslave*/
#define RTEMS_CORESLAVE_INT
/*Base address for the coreslave*/
#define CORESLAVE_BASE     0x22000000
#define CORESLAVE_SPACING  0x00020000

/*Base address of the smartdma dma*/
#define SMART_DMA_BASE_ADDRESS   0x70000000
#define SMARTMEM_SPACING       0x02000000
/*total number of dma*/
#define N_DMA 4

/*Base address of the INTERNAL DMA*/
#define INT_DMA_BASE_ADDRESS   0x60000000
/*Base address of the EXTERNAL DMA*/
#define EXT_DMA_BASE_ADDRESS   0x70000000

/*Size of the heap library in the L2 memory*/
#define HEAP_SIZE 0x300000

/*
 * VARIABLE DECLARATION   
 */

#ifndef __asm__
extern volatile unsigned long *Regs;        /* Chip registers        */
#endif



/*
 * Here must be "defined" each register, to use with Regs as
 * LM_Regs[REGISTER1] = value
 */

#define	REGISTER1      1
#define REGISTER2      2


/*
 * define for UART registers to be able
 * to compile and link arm_bare_bsp
 */
#define SWARM_UART0_BASE     0x90081000
#define SWARM_UART0_TXBASE   0x90081000
#define SWARM_UART0_RXBASE   0x90081004
#define SWARM_UART0_CR       0x90081008
#define SWARM_UART0_SR       0x9008100c

/* timers */
#define SWARM_TIMER_BASE     0x90000000
#define SWARM_TIMER_MATCH0   0x90000000
#define SWARM_TIMER_MATCH1   0x90000004
#define SWARM_TIMER_MATCH2   0x90000008
#define SWARM_TIMER_MATCH3   0x9000000c
#define SWARM_TIMER_CNT      0x90000010
#define SWARM_TIMER_STATUS   0x90000014
#define SWARM_TIMER_WDOG     0x90000018
#define SWARM_TIMER_INTEN    0x9000001C

#define SWARM_TIMER_SYS_COUNT 50000

/* interrupt */
#define SWARM_INT_BASE       0x90050000
#define SWARM_INT_IRQ_STATUS 0x90050000
#define SWARM_INT_MASK       0x90050004
#define SWARM_INT_LEVEL      0x90050008
#define SWARM_INT_FIQ_STATUS 0x9005000c

#endif /*__REGS_H__*/

#ifndef __ASM_ARCH_HARDWARE_H
#define __ASM_ARCH_HARDWARE_H
#endif

