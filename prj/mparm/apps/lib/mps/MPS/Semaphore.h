#ifndef MPS_SEMAPHORE_H
#define MPS_SEMAPHORE_H

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

# include <MPS/Config.h>
# include <IceUtil/Time.h>

namespace MPS {

    // Share Time implementation with IceUtil
    typedef typename IceUtil::Time Time;
    
    // Common semaphore behaviour
    template <class SemaphoreState>
    class BaseSemaphore
    {
    public:
	BaseSemaphore(SemaphoreState& state) : _state(state)
	{
	}
	    
	~Semaphore()
	{
	}

	void wait() const
	{
	    for(;;) {
		if (_state.dec())
		    break;
		_state.yield();
	    }
	}
	
	bool timedWait(const Time& t) const
	{
	    Time begin = Time::now();
	    for(;;) {
		if (_state.dec())
		    return true;
		_state.yield();
		    
		if (Time::now() - begin > t)
		    return false;		// timeout
	    }
	}

	void post(int inc = 1) const
	{
	    _state.inc(inc);
	}

    private:
	BaseSemaphore(const BaseSemaphore&);	// disallow copy ctor
	void operator=(const BaseSemaphore&);	// disallow assignment operator

    private:
	SemaphoreState& _state;
    };
    
}

# include <MPS/ShMem/Semaphore.h>
# include <MPS/ScrMem/Semaphore.h>

#endif
