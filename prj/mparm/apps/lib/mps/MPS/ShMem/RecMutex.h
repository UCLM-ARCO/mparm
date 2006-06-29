#ifndef MPS_SHMEM_REC_MUTEX_H
#define MPS_SHMEM_REC_MUTEX_H

namespace MPS {

    namespace ShMem {
	
	//----------------------------------------------
	// Recursive mutexes using SHARED MEMORY 
    
	typedef MPS::BaseRecMutex<MutexState> BaseRecMutex;

	class RecMutex : public BaseRecMutex
	{
	public:
	    RecMutex(unsigned id) : BaseRecMutex(_state), _state(id)
	    {
	    }

	private:
	    RecMutex(const RecMutex&);		// disallow copy ctor
	    void operator=(const RecMutex&);	// disallow assignment operator

	    MutexState _state;
	};

	//----------------------------------------------
	// Spin mutexes do not yield the CPU 
    
	typedef MPS::BaseRecMutex<SpinMutexState> BaseSpinRecMutex;

	class SpinRecMutex : public BaseSpinRecMutex
	{
	public:
	    SpinRecMutex(unsigned id) : BaseSpinRecMutex(_state), _state(id)
	    {
	    }

	private:
	    SpinRecMutex(const SpinRecMutex&);	 // disallow copy ctor
	    void operator=(const SpinRecMutex&); // disallow a. operator

	    SpinMutexState _state;
	};

    }

}
#endif
