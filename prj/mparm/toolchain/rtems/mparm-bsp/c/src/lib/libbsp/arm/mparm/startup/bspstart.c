/*-------------------------------------------------------------------------+
| This file contains the ARM BSP startup package. It includes application,
| board, and monitor specific initialization and configuration. The generic CPU
| dependent initialization has been performed before this routine is invoked. 
+--------------------------------------------------------------------------+
|
| Copyright (c) 2000 Canon Research Centre France SA.
| Emmanuel Raguet, mailto:raguet@crf.canon.fr
|
|   The license and distribution terms for this file may be
|   found in found in the file LICENSE in this distribution or at
|   http://www.rtems.com/license/LICENSE.
|
+--------------------------------------------------------------------------*/


#include <bsp.h>
#include <rtems/libcsupport.h>
#include <rtems/libio.h>
#include <rtems/bspIo.h>       /* printk */
#include <registers.h>

/*-------------------------------------------------------------------------+
| Global Variables
+--------------------------------------------------------------------------*/

/*initialized in linkcmds*/
extern void  *_freememstart;
unsigned long rtemsFreeMemStart; /*inizialized in bsp_start_default*/


/* The original BSP configuration table from the application and our copy of it
   with some changes. */

extern rtems_configuration_table  Configuration;

rtems_configuration_table    BSP_Configuration;
rtems_multiprocessing_table  BSP_Multiprocessing;

rtems_cpu_table Cpu_table;                     /* CPU configuration table.    */
char            *rtems_progname;               /* Program name - from main(). */

/*-------------------------------------------------------------------------+
| External Prototypes
+--------------------------------------------------------------------------*/
void rtems_irq_mngt_init(void);
void bsp_libc_init( void *, rtems_unsigned32, int );
void bsp_postdriver_hook(void);


/*-------------------------------------------------------------------------+
|         Function: bsp_pretasking_hook
|      Description: BSP pretasking hook.  Called just before drivers are
|                   initialized. Used to setup libc and install any BSP
|                   extensions. NOTE: Must not use libc (to do io) from here,
|                   since drivers are not yet initialized.
| Global Variables: None.
|        Arguments: None.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
void bsp_pretasking_hook(void)
{
  rtems_unsigned32 heap_start;

  heap_start = rtemsFreeMemStart;
        
  bsp_libc_init((void*)heap_start, HEAP_SIZE, 0);

  /*
   * Init mparm DMA I/O (uses malloc and sprintf)
   */
  dma_driver_initialize();

} /* bsp_pretasking_hook */
 

/*-------------------------------------------------------------------------+
|         Function: bsp_start
|      Description: Called before main is invoked.
| Global Variables: None.
|        Arguments: None.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
void bsp_start_default( void )
{
  int i;
  Cpu_table.pretasking_hook         = bsp_pretasking_hook; /* init libc, etc. */
  Cpu_table.postdriver_hook         = bsp_postdriver_hook;
  Cpu_table.do_zero_of_workspace    = FALSE;

  BSP_Configuration.work_space_start = (void*)&_freememstart;
  rtemsFreeMemStart = ((rtems_unsigned32)&_freememstart + BSP_Configuration.work_space_size);
  BSP_Configuration.microseconds_per_tick = 100000;

  if (BSP_Configuration.User_multiprocessing_table) {
    /* make a copy for possible editing (may be in shared mem)*/
    BSP_Multiprocessing = *BSP_Configuration.User_multiprocessing_table;
    BSP_Configuration.User_multiprocessing_table = &BSP_Multiprocessing;
    BSP_Multiprocessing.node = get_id();
    BSP_Multiprocessing.maximum_nodes = get_tot();
  }

  /*
   * Init rtems exceptions management
   */
  rtems_exception_init_mngt();

  /*
   * Init rtems interrupt management
   */
  rtems_irq_mngt_init();

  /*
   *  The following information is very useful when debugging.
   */
#ifdef PRINTDEBUGINFO
  printk("\n******INITIALIZATION OF RTEMS ON SWARM (Poletti Francesco)******\n\n");
  printk("processor id = 0x%x\n", BSP_Multiprocessing.node);
  printk("number of cores = 0x%x\n", BSP_Multiprocessing.maximum_nodes);
  printk("work_space_start = 0x%08x\n", BSP_Configuration.work_space_start);
  printk("work_space_size = 0x%08x\n", BSP_Configuration.work_space_size);
  printk("irq_stack_size = 0x%x\n", Cpu_table.interrupt_stack_size);
  printk("heap address = 0x%x\n",rtemsFreeMemStart);
  printk("heap_size = 0x%x\n\n", HEAP_SIZE);
  printk("USER DEFINES\n\n");
  printk("microseconds_per_tick = 0x%x\n", BSP_Configuration.microseconds_per_tick);
  printk("ticks_per_timeslice = 0x%x\n", BSP_Configuration.ticks_per_timeslice);
  printk("maximum_devices = 0x%x\n", BSP_Configuration.maximum_devices);
  printk("number_of_device_drivers = 0x%x\n", BSP_Configuration.number_of_device_drivers);
  printk("device_driver_table = 0x%x\n", BSP_Configuration.Device_driver_table);
#endif

} /* bsp_start */


/*
 *  By making this a weak alias for bsp_start_default, a brave soul
 *  can override the actual bsp_start routine used.
 */
void bsp_start (void) __attribute__ ((weak, alias("bsp_start_default")));
