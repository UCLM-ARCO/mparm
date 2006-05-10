/*
 *  start.S :	  RTEMS entry point
 *
 *  Copyright (C) 2000 Canon Research Centre France SA.
 *  Emmanuel Raguet, mailto:raguet@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
    Con modifiche per simulatore swarm
    in particolare ho tolto il carimento di dati nella ram , 
    ho spostato gli stack di abort irq fiq svc in fondo alla ram,
    ho aggiunto una sezione resetvector che punta alla _start
 */
/*-----------------------------------------------------------------------------
 * Definitions
 ----------------------------------------------------------------------------*/
.equ PID_RAM_Limit, 0xBffff0
/* stack size definition */
.equ FIQ_StackSize,     0x400		/* FIQ stack size */                   
.equ IRQ_StackSize,     0x10000		/* IRQ stack size */         
.equ SVC_StackSize,     0x10000		/* SVC stack size */         
.equ ABORT_StackSize,   0x1000		/* ABORT stack size */ 
.equ UNDEF_StackSize,   0x1000        	/* UNDEF stack size */        

/* stack size address */
.equ Stack_Limit,     PID_RAM_Limit           
.equ SVC_Stack,       Stack_Limit
.equ ABORT_Stack,     Stack_Limit - SVC_StackSize     
.equ UNDEF_Stack,     ABORT_Stack - ABORT_StackSize	
.equ IRQ_Stack,       UNDEF_Stack - UNDEF_StackSize   
.equ FIQ_Stack,       IRQ_Stack - IRQ_StackSize  	
.equ END_FIQ,	      FIQ_Stack - FIQ_StackSize
	
/* Some standard definitions...*/

.equ Mode_USR,        	     0x10
.equ Mode_FIQ,        	     0x11
.equ Mode_IRQ,        	     0x12
.equ Mode_SVC,        	     0x13
.equ Mode_ABT,        	     0x17
.equ Mode_ABORT,             0x17
.equ Mode_UNDEF,      	     0x1B
.equ Mode_SYS,        	     0x1F /*only available on ARM Arch. v4*/

.equ I_Bit,           	     0x80
.equ F_Bit,           	     0x40


	.text
	.globl	_start
	.globl  __exit_simul
	
__end_loop:	b	__end_loop

	 
_start:

/*
 * Here is the code to initialize the low-level BSP environment
 * (Chip Select, PLL, ....?)
*/ 

/* copy execution address into r2 */

/*
  Copy data from RAM to SCRATCH
        LDR     r0, =_initscratch      
	LDR     r1, =0x70000000        
        LDR     r2, =_endscratch
*/
	
/* loop whilst r1 < r2 */

/*
  copy:
        CMP     r1, r2		      
        LDRLO   r3, [r0], #4
        STRLO   r3, [r1], #4
        BLO     copy
*/


/* zero the bss
        LDR     r1, =_bss_end_       
        LDR     r0, =_bss_start_     
zi_init:	
        MOV     r2, #0
        CMP     r0, r1		      
        STRLOT   r2, [r0], #4
        BLO     zi_init 

*/
	
/* Load basic ARM7 interrupt table */
VectorInit:	
	MOV	R8, #0
	ADR	R9, Vector_Init_Block
	LDMIA	R9!, {R0-R7}	/* Copy the Vectors (8 words) */
	STMIA	R8!, {R0-R7}
	LDMIA	R9!, {R0-R7}	/* Copy the .long'ed addresses (8 words) */
	STMIA	R8!, {R0-R7}

	B	init2

/*******************************************************
 standard exception vectors table
 *** Must be located at address 0 
********************************************************/	 

Vector_Init_Block:	
	LDR	PC, Reset_Addr
	LDR	PC, Undefined_Addr
	LDR	PC, SWI_Addr
	LDR	PC, Prefetch_Addr
	LDR	PC, Abort_Addr
	NOP
	LDR	PC, IRQ_Addr
	LDR	PC, FIQ_Addr

	.globl Reset_Addr
Reset_Addr:	.long   _start 
Undefined_Addr:	.long	Undefined_Handler
SWI_Addr:	.long	SWI_Handler
Prefetch_Addr:	.long	Prefetch_Handler
Abort_Addr:	.long	Abort_Handler
		.long	0	
IRQ_Addr:	.long	IRQ_Handler
FIQ_Addr:	.long	FIQ_Handler
	
/* The following handlers do not do anything useful */
	.globl Undefined_Handler
Undefined_Handler:	
        B       Undefined_Handler
	.globl SWI_Handler
SWI_Handler:	
        B       SWI_Handler 
	.globl Prefetch_Handler
Prefetch_Handler:	
        B       Prefetch_Handler
	.globl Abort_Handler
Abort_Handler:	
        B       Abort_Handler
	.globl IRQ_Handler
IRQ_Handler:	
        B       IRQ_Handler
	.globl FIQ_Handler
FIQ_Handler:	
        B       FIQ_Handler

init2 :	
/* --- Initialise stack pointer registers
   Set up the ABORT stack pointer last and stay in SVC mode */
    MOV     r0, #(Mode_ABORT | I_Bit | F_Bit)   /* No interrupts */
    MSR     cpsr, r0
    LDR     sp, =ABORT_Stack

/* Enter IRQ mode and set up the IRQ stack pointer */
    MOV     r0, #Mode_IRQ | I_Bit | F_Bit     /* No interrupts */
    MSR     cpsr, r0
    LDR     sp, =IRQ_Stack

/* Enter FIQ mode and set up the FIQ stack pointer */
    MOV     r0, #Mode_FIQ | I_Bit | F_Bit     /* No interrupts */
    MSR     cpsr, r0
    LDR     sp, =FIQ_Stack

/* Set up the SVC stack pointer last and stay in SVC mode */
    MOV     r0, #Mode_SVC | I_Bit | F_Bit     /* No interrupts */
    MSR     cpsr, r0
    LDR     sp, =SVC_Stack

/* --- Now we enter the C code */

    B	boot_card

    ldmia	sp!, {r4-r12, lr}
      mov	sp, r12
      mov	pc, lr


.section ".resetvector",#alloc,#execinstr
		.type stext,#function
		b	_start









