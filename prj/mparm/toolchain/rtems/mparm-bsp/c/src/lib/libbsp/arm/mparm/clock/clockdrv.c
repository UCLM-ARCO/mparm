/*
 * Swarm Clock driver
 *
 * Written by Francesco Poletti 
 *
 *  
 *
 *  clockdrv.c,v 1.1 2002/11/13 17:55:04 joel Exp
*/
#include <rtems.h>
#include <bsp.h>
#include <irq.h>



rtems_isr clock_isr(rtems_vector_number vector);
rtems_isr Clock_isr(rtems_vector_number vector);
static void clock_isr_on(const rtems_irq_connect_data *unused);
static void clock_isr_off(const rtems_irq_connect_data *unused);
static int clock_isr_is_on(const rtems_irq_connect_data *irq);

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

rtems_irq_connect_data clock_isr_data = {BSP_TIMER_0,
                                         (rtems_irq_hdl)Clock_isr,
                                         clock_isr_on,
                                         clock_isr_off,
                                         clock_isr_is_on,
                                         26,
                                         0 };

#define CLOCK_VECTOR 0

#define Clock_driver_support_at_tick()                			   \
  do {   int TimerCnt;							   \
         TimerCnt = tread(SWARM_TIMER_CNT);				   \
	 TimerCnt += SWARM_TIMER_SYS_COUNT; /*defined in register.h*/	   \
	 twrite(SWARM_TIMER_MATCH0 , TimerCnt);				   \
  } while(0)

#define Clock_driver_support_install_isr( _new, _old ) \
  do {clock_isr_data.hdl=_new;                                             \
      BSP_install_rtems_irq_handler(&clock_isr_data);                      \
     } while(0)


/* 
 * Set up the clock hardware
*/
#define Clock_driver_support_initialize_hardware()                            \
  do { unsigned32 TimerCnt;                                                   \
       TimerCnt = tread(SWARM_TIMER_CNT);                                     \
       TimerCnt += SWARM_TIMER_SYS_COUNT; /*defined in register.h*/	      \
       twrite(SWARM_TIMER_MATCH1, TimerCnt);				      \
       clock_isr_data.on(&clock_isr_data);				      \
      } while (0)

#define Clock_driver_support_shutdown_hardware()                        \
  do {                                                                  \
	BSP_remove_rtems_irq_handler(&clock_isr_data);                  \
     } while (0)

static void clock_isr_on(const rtems_irq_connect_data *irq)
{unsigned32 stato;
 son(irq); /*abilito l'int nell'int controller*/ 
 stato = tread(SWARM_TIMER_INTEN);				      \
 stato |= 0x1;							      \
 twrite(SWARM_TIMER_INTEN,stato);				              \
}

static void clock_isr_off(const rtems_irq_connect_data *irq)
{
 unsigned32 stato;
 stato = tread(SWARM_TIMER_INTEN);
 stato &= ~(0x1);
 twrite(SWARM_TIMER_INTEN,stato);
 soff(irq); /*disabilito l'int nell'int controller*/
}

static int clock_isr_is_on(const rtems_irq_connect_data *irq)
{
  return (sison(irq) && tread(SWARM_TIMER_INTEN) && 0x2); 
}


#include "../../../shared/clockdrv_shell.c"
