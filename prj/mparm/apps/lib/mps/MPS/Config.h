#ifndef MPS_CONFIG_H
#define MPS_CONFIG_H

/*! \file MPS/Config.h
  \brief Configuration of MPSoC platform for MPS.

  This file includes all the configuration details such as memory map
  of the MPARM simulator.  It is based on constants defined in the
  source code of the MPARM simulator.
*/

#include "mpsim_config.h"

namespace MPS {

    namespace ShMem {

	/*! \brief Base address of shared memory semaphores.
	 *
	 * This is the base address of binary hardware semaphores
	 * available in MPARM base platform.
	 */
	volatile int* const semaphore_base
	    = reinterpret_cast<volatile int* const>(SEMAPHORE_BASE);
	
	/*! \brief Base address of shared memory.
	 *
	 * This is the base address of the shared memory available in
	 * MPARM base platform.
	 */
	volatile int* const memory_base
	    = reinterpret_cast<volatile int* const>(SHARED_BASE);
	
    }
    
    namespace ScrMem {

	/*! \brief Base address of scratch semaphores.
	 *
	 * This is the base address of hardware semaphores available
	 * in the CORESLAVE peripheral of MPARM platform.
	 *
	 * \param core Identifier of the processor.
	 *
	 * \returns Returns a pointer to the first semaphore in the
	 * given processor.
	 */
	inline volatile int* const semaphore_base(int core)
	{
	    return reinterpret_cast<volatile int* const>
		(CORESLAVE_BASE
		 + CORESLAVE_SPACING * (core - 1)
		 + BUILTIN_DEFAULT_SCRATCH_SIZE);
	}
	
	/*! \brief Base address of scratch memory.
	 *
	 * This is the base address of the scratchpad memory available
	 * in each processor of the MPARM platform.
	 *
	 * \param core Identifier of the processor.
	 *
	 * \returns Returns a pointer to the first word in the given
	 * processor.
	 */
	inline volatile int* const memory_base(int core)
	{
	    return reinterpret_cast<volatile int* const>
		(CORESLAVE_BASE
		 + CORESLAVE_SPACING * (core - 1));
	}
	
    }

}

#endif
