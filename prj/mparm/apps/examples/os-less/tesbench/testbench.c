#include "mpsim.h"

int 
main()
{
  int argc;
  char **argv, **envp;
  char thischar;
  int cycles, loop;
  unsigned long long int time, cycle;
  unsigned long int retval;
  unsigned short int curfreq;

#if 0
  pr("", 0x0, PR_NEWL);
  pr("", 0x0, PR_NEWL);
#endif
  
  pr("---Test: getting CPU ID and total---", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);

  pr("This is processor (ID from 1 onwards):", get_proc_id(), PR_CPU_ID | PR_STRING | PR_DEC | PR_NEWL);
  pr("Number of system processors:",           get_proc_num(),      PR_CPU_ID | PR_STRING | PR_DEC | PR_NEWL);
  pr("---DONE---", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);

  pr("", 0x0, PR_NEWL);
  pr("", 0x0, PR_NEWL);
  
  pr("---Test: marking end of boot---", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  /* If statistics were collected since bootstrap, boot ends here, so let the
     simulator decide what to do with collection */
  pr("Just before boot", 0x0, PR_CPU_ID | PR_STRING | PR_TSTAMP | PR_NEWL);
  end_boot();
  pr("Just after boot ", 0x0, PR_CPU_ID | PR_STRING | PR_TSTAMP | PR_NEWL);
  pr("---DONE---", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  
  pr("", 0x0, PR_NEWL);
  pr("", 0x0, PR_NEWL);

  pr("---Test: printing debug messages in various formats---", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  pr("Test print decimal (10 should appear):         ", 10,                     PR_CPU_ID | PR_STRING | PR_DEC  | PR_NEWL);
  pr("Test print hex     (0x0000000a should appear): ", 10,                     PR_CPU_ID | PR_STRING | PR_HEX  | PR_NEWL);
  pr("Test print char    (z should appear):          ", (unsigned long int)'z', PR_CPU_ID | PR_STRING | PR_CHAR | PR_NEWL);
  pr("---DONE---", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  
  pr("", 0x0, PR_NEWL);
  pr("", 0x0, PR_NEWL);
  
  pr("---Test: getting argc/argv---", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  argc = get_argc();
  argv = (char **)get_argv();
  pr("argc value", (unsigned long int)argc, PR_CPU_ID | PR_STRING | PR_DEC | PR_NEWL);
  for (cycles = 0; cycles < argc; cycles ++)
    for (loop = 0; argv[cycles][loop] != '\0'; loop ++)
      pr("", (unsigned long int)argv[cycles][loop], PR_CHAR | PR_NEWL);
  pr("---DONE---", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
      
  pr("", 0x0, PR_NEWL);
  pr("", 0x0, PR_NEWL);
  
  pr("---Test: getting simulation time and cycle---", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  time = get_time();
  cycle = get_cycle();
  pr("time low",   (unsigned long int)(time & 0x00000000FFFFFFFF), PR_CPU_ID | PR_STRING | PR_DEC | PR_NEWL);
  pr("time high",  (unsigned long int)(time >> 32), PR_CPU_ID | PR_STRING | PR_DEC | PR_NEWL);
  pr("cycle low",  (unsigned long int)(cycle & 0x00000000FFFFFFFF), PR_CPU_ID | PR_STRING | PR_DEC | PR_NEWL);
  pr("cycle high", (unsigned long int)(cycle >> 32), PR_CPU_ID | PR_STRING | PR_DEC | PR_NEWL);
  pr("---DONE---", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  
  pr("", 0x0, PR_NEWL);
  pr("", 0x0, PR_NEWL);

#if 0
  pr("---Test: accessing files---", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  if (get_proc_id() == 1)
  {
    retval = access_file("test.txt", FILE_READ);
    if (retval == 0)
      pr("ERROR in opening test.txt for read!", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
    else
      pr("Success in opening test.txt for read!", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  
    retval = access_file("out.txt", FILE_WRITE);
    if (retval == 0)
      pr("ERROR in opening out.txt for write!", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
    else
      pr("Success in opening out.txt for write!", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  }
  pr("---DONE---", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);

  pr("", 0x0, PR_NEWL);
  pr("", 0x0, PR_NEWL);
#endif
  
  pr("---Test: accessing semaphores---", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  if (get_proc_id() == 1)
  {
    // Init to free
    for (loop = 0; loop < 10; loop ++)
      MPSIM_SEM[loop] = 0;
    // They should be free
    for (loop = 0; loop < 10; loop ++)
      if (MPSIM_SEM[loop] == 1)
        pr("ERROR in semaphore access!", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
    // But now they should be locked
    for (loop = 0; loop < 10; loop ++)
      if (MPSIM_SEM[loop] == 0)
        pr("ERROR in semaphore access!", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  }
  pr("---DONE---", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  
  pr("", 0x0, PR_NEWL);
  pr("", 0x0, PR_NEWL);
  
  pr("---Test: accessing shared memory---", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  if (get_proc_id() == 1)
  {
    for (loop = 0; loop < 100; loop ++)
      MPSIM_SHARED[loop] = 0xab;
    for (loop = 0; loop < 100; loop ++)
      if (MPSIM_SHARED[loop] != 0xab)
        pr("ERROR in shared memory access!", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  }
  pr("---DONE---", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  
  pr("", 0x0, PR_NEWL);
  pr("", 0x0, PR_NEWL);
  
  pr("---Test: starting and stopping statistics collection---", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  pr("Just before statistics start", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  start_metric();
  pr("Statistics running", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  stop_metric();
  pr("Just after statistics stop", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  pr("---DONE---", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  
  pr("", 0x0, PR_NEWL);
  pr("", 0x0, PR_NEWL);
  
#ifdef INT_AVAIL
  pr("---Test: sending two interrupts to this very core---", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  pr("WARNING: this will fail if no interrupt device is present (-I switch to the simulator).", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  pr("In this case, disable this test by removing INT_AVAIL from the Makefile options", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  pr("Just before sending first interrupt", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  send_interrupt(get_proc_id());
  pr("Just before sending second interrupt", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  send_interrupt(get_proc_id());
  pr("---DONE---", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  
  pr("", 0x0, PR_NEWL);
  pr("", 0x0, PR_NEWL);
#endif

#if 0
  pr("---Test: scaling down the core frequency to 1/2 and up again---", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  pr("WARNING: this will fail if no frequency scaling device is present (-f switch to the simulator).", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  pr("In this case, disable this test by removing FREQ_AVAIL from the Makefile options", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  pr("Just before checking current frequency", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  curfreq = get_this_core_frequency();
  pr("Current frequency divider is: ", curfreq, PR_CPU_ID | PR_STRING | PR_DEC | PR_NEWL);
  pr("Just before scaling frequency down", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  scale_this_core_frequency(2);
  curfreq = get_this_core_frequency();
  pr("Current frequency divider is: ", curfreq, PR_CPU_ID | PR_STRING | PR_DEC | PR_NEWL);
  pr("Just before scaling frequency up again", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  scale_this_core_frequency(1);
  curfreq = get_this_core_frequency();
  pr("Current frequency divider is: ", curfreq, PR_CPU_ID | PR_STRING | PR_DEC | PR_NEWL);
  pr("---DONE---", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  
  pr("", 0x0, PR_NEWL);
  pr("", 0x0, PR_NEWL);

  // FIXME get_proc_num() is very fragile
  pr("---Test: scaling down the bus frequency to 1/2 and up again---", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  pr("WARNING: this will fail if no frequency scaling device is present (-f switch to the simulator).", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  pr("In this case, disable this test by removing FREQ_AVAIL from the Makefile options", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  pr("Just before checking current frequency", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  curfreq = get_device_frequency(get_proc_num());
  pr("Current frequency divider is: ", curfreq, PR_CPU_ID | PR_STRING | PR_DEC | PR_NEWL);
  pr("Just before scaling frequency down", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  scale_device_frequency(2, get_proc_num());
  curfreq = get_device_frequency(get_proc_num());
  pr("Current frequency divider is: ", curfreq, PR_CPU_ID | PR_STRING | PR_DEC | PR_NEWL);
  pr("Just before scaling frequency up again", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  scale_device_frequency(1, get_proc_num());
  curfreq = get_device_frequency(get_proc_num());
  pr("Current frequency divider is: ", curfreq, PR_CPU_ID | PR_STRING | PR_DEC | PR_NEWL);
  pr("---DONE---", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  
  pr("", 0x0, PR_NEWL);
  pr("", 0x0, PR_NEWL);
#endif

  pr("---SHUTTING DOWN---", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
  stop_simulation();
  pr("ERROR in shutdown!", 0x0, PR_CPU_ID | PR_STRING | PR_NEWL);
}

void
_exit(int status)
{
  pr("_exit", status, PR_CPU_ID | PR_STRING | PR_DEC | PR_NEWL);
  stop_simulation();
  while(1);
}

void
interrupt_function()
{
  static int irq_counter = 0;
  
  pr("interrupt_function", irq_counter,
     PR_CPU_ID | PR_STRING | PR_DEC | PR_TSTAMP | PR_NEWL);
  *((volatile int*)0x90050000) &= ~0x800000;
  irq_counter ++;
}
