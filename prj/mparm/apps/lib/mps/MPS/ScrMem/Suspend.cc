#include <simctrl.h>
#include <MPS/ScrMem/Suspend.h>

#ifdef __rtems__

namespace MPS {

    namespace ScrMem {
	
	bool Suspend::_initialized = false;
	unsigned Suspend::_suspended = 0;
	rtems_id Suspend::_taskId[MPS::ScrMem::MaxLocalTasksWaiting];

    }

}

#endif
