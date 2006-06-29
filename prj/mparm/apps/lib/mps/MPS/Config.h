#ifndef MPS_CONFIG_H
#define MPS_CONFIG_H

#include "mpsim_config.h"

namespace MPS {

    namespace ShMem {

	// Base address of ShMem hardware semaphores
	volatile int* const semaphore_base
	    = reinterpret_cast<volatile int* const>(SEMAPHORE_BASE);
	
	// Base address of ShMem memory
	volatile int* const memory_base
	    = reinterpret_cast<volatile int* const>(SHARED_BASE);
	
    }
    
    namespace ScrMem {

	// Base address of ScrMem hardware semaphores
	inline volatile int* const semaphore_base(int core)
	{
	    return reinterpret_cast<volatile int* const>
		(CORESLAVE_BASE
		 + CORESLAVE_SPACING * (core - 1)
		 + BUILTIN_DEFAULT_SCRATCH_SIZE);
	}
	
	// Base address of ScrMem memory
	inline volatile int* const memory_base(int core)
	{
	    return reinterpret_cast<volatile int* const>
		(CORESLAVE_BASE
		 + CORESLAVE_SPACING * (core - 1));
	}
	
    }

}

#endif
