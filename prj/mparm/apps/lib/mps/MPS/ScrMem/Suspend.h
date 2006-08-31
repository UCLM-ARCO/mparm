#ifndef MPS_SUSPEND_H
#define MPS_SUSPEND_H

# ifdef __rtems__
	// Suspension semantics for semaphores and mutexes (platform
	// specific)

#  include <bsp.h>

namespace MPS {
    
    namespace ScrMem {

	// Mostly taken from ss_semaphore.cpp

	const unsigned MaxLocalTasksWaiting = 32;
	
	class Suspend {
	public:
	    Suspend()
	    {
		if (!_initialized) {
		    // FIXME: As of 2006-07-04 sison is wrong
		    // in the RTEMS source code
		    static rtems_irq_connect_data irq = {
			int21, &interrupt_handler,
			son, soff, sison, 0, 0
		    };
		    BSP_install_rtems_irq_handler(&irq);
		    irq.on(&irq);
		    _initialized = true;
		}
	    }
	    
	    void yield()
	    {
		if (_suspended >= MaxLocalTasksWaiting)
		    return;

		rtems_interrupt_level level;
		rtems_interrupt_disable(level);

		rtems_id id;
		rtems_task_ident(RTEMS_SELF, get_proc_id(), &id);
		_taskId[_suspended++] = id;
		rtems_task_suspend(id);
		
		rtems_interrupt_enable(level);
	    }

	private:
	    static void interrupt_handler()
	    {
		rtems_interrupt_level level;
		rtems_interrupt_disable(level);
		for(unsigned i = 0; i < _suspended; i++) {
		    rtems_status_code status;
		    status = rtems_task_resume(_taskId[i]);
		}
		_suspended = 0;
		rtems_interrupt_enable(level);
	    }
	    static bool _initialized;
	    static unsigned _suspended;
	    static rtems_id _taskId[MaxLocalTasksWaiting];
	};

    }

}
# endif
	
#endif
