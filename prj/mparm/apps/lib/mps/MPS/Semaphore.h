#ifndef MPS_COND_H
#define MPS_COND_H

//------------------------------------------------------------------------
// Semaphore implementation for inter-processor synchronization

// Shared memory semaphores have different semantics than scratchpad
// memories semaphores.  Hence we need to provide two set of
// implementations (MPS::ShMem::Semaphores, and
// MPS::ScrMem::Semaphores).  Please see core/ext_mem.h and
// core/scratch_mem.h for information on the precise semantics.

// Semaphores are too primitive for most applications.  Deadlocks and
// race conditions are easy to get and hard to find.  You should avoid
// them if possible.  Look at Mutex, RecMutex, Cond, Monitor for
// higher level synchronization abstractions, and refer to the
// documentation for hints on proper usage.

namespace MPS {

    namespace ShMem {

	//-----------------------------------------------------------
	// POSIX style semaphores for SHARED MEMORY

	// The semantics of current shared semaphores does only allow
	// implementing mutexes. Therefore we need an additional
	// shared memory counter.
	
	class Semaphore
	{
	public:
	    Semaphore(unsigned lock, unsigned counter, bool init = false,
		      long value = 0)
		: _lock(semaphore_base[lock]), _counter(memory_base[counter])
	    {
		if (init) {
		    _lock = 0;
		    _counter = value;
		}
	    }
	    
	    ~Semaphore()
	    {
	    }

	    void wait() const
	    {
		for(;;) {
		    Mutex::Lock lock(_mutex);
		    int value = _counter;
		    if (value) {
			_counter = value - 1;
			break;
		    }
		    sleep(0);	// yields the CPU
		}
	    }

	    bool timedWait(const Time& t) const
	    {
		Time begin = Time::now();
		for(;;) {
		    Mutex::Lock lock(_mutex);
		    int value = _counter;
		    if (value) {
			_counter = value - 1;
			return true;
		    }
		    sleep(0);			// yields the CPU
		    
		    if (Time::now() - begin > t)
			return false;		// timeout
		}
	    }

	    void post(int inc = 1) const
	    {
		Mutex::Lock lock(_mutex);
		_counter = _counter + inc;
	    }

	private:
	    Mutex _mutex;
	    mutable volatile int& _counter;
	};

    }

    namespace ScrMem {

	//----------------------------------------------
	// POSIX style semaphores for SCRATCH MEMORY

	// The semantics of scratch semaphores fits quite nicely in
	// the POSIX style implementation.  Therefore they are far
	// more efficient than SHARED MEMORY semaphores.

	class Semaphore
	{
	public:
	    Semaphore(unsigned core, unsigned id,
		      bool init = false, long value = 0)
		: _semaphore(semaphore_base(core)[id])
	    {
		if (init)
		    _semaphore = value + 1;
	    }
	    
	    ~Semaphore()
	    {
	    }

	    void wait() const
	    {
		while (!_semaphore)
		    sleep(0);			// yields the CPU
	    }

	    bool timedWait(const Time& t) const
	    {
		Time begin = Time::now();
		while (!_semaphore) {
		    sleep(0);			// yields the CPU
		    if (Time::now() - begin > t)
			return false;		// timeout
		}
		return true;
	    }

	    void post(int inc = 1) const
	    {
		_semaphore = 0;
	    }

	private:
	    Mutex _mutex;
	    mutable volatile int& _counter;
	};
	
    }
    
}

#endif
