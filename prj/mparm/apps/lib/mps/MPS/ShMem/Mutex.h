#ifndef MPS_SHMEM_MUTEX_H
#define MPS_SHMEM_MUTEX_H

namespace MPS {

    namespace ShMem {

	class MutexState
	{
	public:
	    MutexState(unsigned id) : _semaphore(semaphore_base[id])
	    {
	    }

	    bool tryLock() const
	    {
		return (!_semaphore);
	    }

	    void unlock() const
	    {
		_semaphore = 0;
	    }

	    void yield() const
	    {
		sleep(0);
	    }

	private:
	    MutexState(const MutexState&);	// disallow copy ctor
	    void operator=(const MutexState&);	// disallow assignment operator

	    volatile int& _semaphore;
	};

	
	class SpinMutexState: public MutexState
	{
	public:
	    void yield() const
	    {
	    }
	}
	
	//----------------------------------------------
	// Non-recursive mutexes using SHARED MEMORY 
    
	typedef typename MPS::Mutex<MutexState> BaseMutex;

	class Mutex : public BaseMutex
	{
	public:
	    Mutex(unsigned id)
		: _state(semaphore_base[id]), BaseMutex(_state)
	    {
	    }

	private:
	    Mutex(const Mutex&);		// disallow copy ctor
	    void operator=(const Mutex&);	// disallow assignment operator

	    MutexState& _state;
	};

	//----------------------------------------------
	// Spin mutexes do not yield the CPU 
    
	typedef typename MPS::Mutex<SpinMutexState> BaseSpinMutex;

	class SpinMutex : public BaseSpinMutex
	{
	public:
	    SpinMutex(unsigned id)
		: _state(semaphore_base[id]), BaseSpinMutex(_state)
	    {
	    }

	private:
	    SpinMutex(const SpinMutex&);	// disallow copy ctor
	    void operator=(const SpinMutex&);	// disallow assignment operator

	    SpinMutexState& _state;
	};

    }
    
}
#endif
