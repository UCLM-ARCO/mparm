#ifndef MPS_SHMEM_SEMAPHORE_H
#define MPS_SHMEM_SEMAPHORE_H

# include <MPS/Mutex.h>

namespace MPS {

    namespace ShMem {

	// The State classes are not meant to be used directly. They
	// are just a convenience for easier adaptation of HW
	// semantics. Please see ${SWARMDIR}/core/ext_mem.h for
	// information on the current semantics.
	
	class SemaphoreState {
	public:
	    SemaphoreState(unsigned lock,     // HW lock to be used as a mutex
			   unsigned counter,  // Shared counter
			   bool init = false, // Set an initial value?
			   unsigned value = 0)
		: _mutex(semaphore_base[lock]),
		_counter(memory_base[counter])
	    {
		if (init) {
		    _mutex.unlock();
		    _counter = value;
		}
	    }
	    
	    bool dec()
	    {
		Mutex::Lock lock(_mutex);
		int value = _counter;
		if (value) {
		    _counter = value - 1;
		    return true;
		}
		return false;
	    }
	    
	    void inc(int add = 1)
	    {
		Mutex::Lock lock(_mutex);
		_counter = _counter + add;
	    }

	    void yield()
	    {
		sleep(0);
	    }
	    
	private:
	    Mutex _mutex;
	    mutable volatile int& _counter;
	};

	class SpinSemaphoreState: public SemaphoreState {
	public:
	    SpinSemaphoreState(unsigned lock,
			       unsigned counter,
			       bool init = false,
			       unsigned value = 0)
		: SemaphoreState(lock, counter, init, value)
	    {
	    }
	    
	    void yield() { }	// Just overrides yield to avoid ctx switch
	};

	
	//-----------------------------------------------------------
	// POSIX style semaphores for SHARED MEMORY

	// The semantics of current shared semaphores does only allow
	// implementing mutexes. Therefore we need an additional
	// shared memory counter.
	
	typedef MPS::BaseSemaphore<SemaphoreState> BaseSemaphore;

	class Semaphore: public BaseSemaphore
	{
	public:
	    Semaphore(unsigned lock, unsigned counter, bool init = false,
		      unsigned value = 0)
		: BaseSemaphore(_state), _state(lock, counter, init, value)
	    {
	    }
	private:
	    SemaphoreState _state;
	};

	//-----------------------------------------------------------
	// Spin semaphores do not yield the CPU while waiting for a
	// locked semaphore
	
	typedef MPS::BaseSemaphore<SpinSemaphoreState> BaseSpinSemaphore;

	class SpinSemaphore: public BaseSpinSemaphore
	{
	public:
	    SpinSemaphore(unsigned lock, unsigned counter, bool init = false,
			  unsigned value = 0)
		: BaseSpinSemaphore(_state), _state(lock, counter, init, value)
	    {
	    }
	private:
	    SpinSemaphoreState _state;
	};

    }

}

#endif
