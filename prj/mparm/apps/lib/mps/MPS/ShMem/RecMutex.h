#ifndef MPS_SHMEM_REC_MUTEX_H
#define MPS_SHMEM_REC_MUTEX_H

namespace MPS {

    namespace ShMem {
	
	//----------------------------------------------
	// Recursive mutexes using SHARED MEMORY 
    
	typedef typename MPS::BaseRecMutex<MutexState> BaseRecMutex;

	class RecMutex : public BaseRecMutex
	{
	public:
	    RecMutex(unsigned id)
		: _state(semaphore_base[id]), BaseRecMutex(_state)
	    {
	    }

	private:
	    RecMutex(const RecMutex&);		// disallow copy ctor
	    void operator=(const RecMutex&);	// disallow assignment operator

	    RecMutexState& _state;
	};

	//----------------------------------------------
	// Spin mutexes do not yield the CPU 
    
	typedef typename MPS::BaseRecMutex<SpinMutexState> BaseSpinRecMutex;

	class SpinRecMutex : public BaseSpinRecMutex
	{
	public:
	    SpinRecMutex(unsigned id)
		: _state(semaphore_base[id]), BaseSpinRecMutex(_state)
	    {
	    }

	private:
	    SpinRecMutex(const SpinRecMutex&);	 // disallow copy ctor
	    void operator=(const SpinRecMutex&); // disallow a. operator

	    SpinRecMutexState& _state;
	};

    }

}
#endif
