/* 
 *  File per il timer del simulatore swarm scritto da Poletti Francesco
 */

#include <bsp.h>
#include <rtems/libio.h>

#define OVERHEAD 0 /*Overhead time which is subtracted to the read*/

void ton(const rtems_irq_connect_data *irq);
void toff(const rtems_irq_connect_data *irq);
int tison(const rtems_irq_connect_data *irq);
static void timer_interrupt(void);

static rtems_irq_connect_data timer_isr_data = {BSP_TIMER_0,
  						timer_interrupt,
  						ton,
  						toff,
  						tison,
  						26,
  						0
					       };



rtems_boolean Timer_driver_Find_average_overhead;
int contatore=0,first=0;

static inline unsigned32
tread(int reg)
{
  register unsigned32 val;

  val = *((volatile unsigned long *)reg);
  
  return val;
}

static inline void      
twrite(int reg, unsigned32 val)
{

  *((volatile unsigned long *)reg) = val;

}

 static void timer_interrupt()
{
  int  TimerCnt;
     	
	contatore++;
	#ifdef QUIET
	printk("\nIn timer_interrupt contatore:%x",contatore);
        #endif
	
	TimerCnt = tread(SWARM_TIMER_CNT);
	TimerCnt += SWARM_TIMER_SYS_COUNT; /*defined in register.h*/
        
	#ifdef QUIET
        printk("\ntimer_interrupt:%x", TimerCnt);
        #endif	
	
	twrite(SWARM_TIMER_MATCH0 , TimerCnt);
         #ifdef QUIET
	 printk("\nLeaving timer_interrupt");
	 #endif
 
}

void ton (const rtems_irq_connect_data* irq)
{unsigned32 stato;
 son(irq); /*abilito l'int nell'int controller*/
 stato = tread(SWARM_TIMER_INTEN);
 stato |= 0x1;
 twrite(SWARM_TIMER_INTEN,stato);
}
void toff (const rtems_irq_connect_data* irq)
{unsigned32 stato;
 stato = tread(SWARM_TIMER_INTEN);
 stato &= ~(0x1);
 twrite(SWARM_TIMER_INTEN,stato);
 soff(irq); /*disabilito l'int nell'int controller*/
}
int tison (const rtems_irq_connect_data* irq)
{return (sison(irq) && tread(SWARM_TIMER_INTEN) && 0x1);
}

void Timer_initialize()
{ int TimerCnt;
  rtems_status_code status;
  status = BSP_install_rtems_irq_handler(&timer_isr_data);
  if (!status)
  {
    printk("Error installing TIMER interrupt handler!\n");
    rtems_fatal_error_occurred(status);
  }  
  TimerCnt = tread(SWARM_TIMER_CNT);
  contatore = 0;
  #ifdef QUIET
  printk("\nTimer_initialize timercnt:%x" , TimerCnt );
  #endif
  TimerCnt += SWARM_TIMER_SYS_COUNT; /*defined in register.h*/
  twrite(SWARM_TIMER_MATCH0, TimerCnt);
  #ifdef QUIET
  printk("\nTimer_initialize timercnt:%x  intlevel:%x\n" , TimerCnt , timer_isr_data.irqLevel);
  #endif
  timer_isr_data.on(&timer_isr_data);
}

int Read_timer()
{ BSP_remove_rtems_irq_handler(&timer_isr_data);
  if (Timer_driver_Find_average_overhead)
    contatore -= OVERHEAD ;
  return (contatore);
}

rtems_status_code Empty_function( void )
{
  return RTEMS_SUCCESSFUL;
}

void Set_find_average_overhead(
  rtems_boolean find_flag
)
{
  Timer_driver_Find_average_overhead = find_flag;
}


