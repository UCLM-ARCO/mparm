/* irq_init.c
 *
 *  This file contains the implementation of rtems initialization
 *  related to interrupt handling.
 *
 *  CopyRight (C) 2000 Canon Research Centre France SA.
 *  Emmanuel Raguet, mailto:raguet@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  bsp_irq_init.c,v 1.1 2000/09/13 13:45:05 joel Exp
 *
disabilito tutti gli int e metto il livello a 
*/
#include <bsp.h>
#include <registers.h>


void BSP_rtems_irq_mngt_init() {

    
        *((volatile unsigned long *)SWARM_INT_MASK)=0x0; /* Disable all interrupts */
	*((volatile unsigned long *)SWARM_INT_FIQ_STATUS)=0x0; /* no FIQs only IRQs */
        *((volatile unsigned long *)SWARM_INT_IRQ_STATUS)=0x0; /* Clear all interrupts	*/
  }


inline void ExecuteITHandler()										 
 {													
	unsigned32 stato,i,appoggio;								
	rtems_irq_hdl *table;										
	table =(rtems_irq_hdl *) VECTOR_TABLE;								
	stato = *((volatile unsigned long *)SWARM_INT_IRQ_STATUS);					
	for(i=0;i<BSP_MAX_INT;i++) 									
	{appoggio = stato & (1 << i);									
	 if (appoggio!=0) break;									
	}												
        if (appoggio==0)return;										
        (*(table + i))();
	/*l'int non e' piu' pendente*/									
	stato = *((volatile unsigned long *)SWARM_INT_IRQ_STATUS); 					
	stato &= ~(0x1 << i); 										
	*((volatile unsigned long *)SWARM_INT_IRQ_STATUS) = stato; /*l'int non e' piu' pendente*/															
 } 
