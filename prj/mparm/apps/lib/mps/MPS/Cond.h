#ifndef MPS_COND_H
#define MPS_COND_H

//------------------------------------------------------------------------
// Condition Variables implementation for inter-processor synchronization

// Shared memory semaphores have different semantics than scratchpad
// memories semaphores.  Hence we need to provide two set of
// implementations (MPS::ShMem::Cond, and MPS::ScrMem::Cond).
// Please see core/ext_mem.h and core/scratch_mem.h for information on
// the precise semantics.

// Cond are meant to be used for inter-processor synchronization.  If
// intra-processor syncronization is also required then you should use
// SCond (not yet ready).

// They consume 3 contiguous semaphores each (plus 2 shared counters
// in the case of external shared memories).  The user just need to
// provide the first of these.  The condition variable must be
// initialized once before it can be used (see init parameter to the
// constructor).

// This code is heavily based on IceUtil/Cond.h 

// NOTE: timedWaitImpl, timedWaitImpl, and wake should probably be
// outlined, but using inlined template member functions we get better
// compatibility with older compilers.

#include <MPS/Mutex.h>
#include <IceUtil/Time.h>
#include <MPS/Semaphore.h>

namespace MPS {

    template <class T> class Monitor;

    // POSIX semantics condition variables (non-shareable version)
    //
    // This class is not meant to be used directly (see
    // MPS::ShMem::Cond and MPS::ScrMem::Cond)
    
    template <class Semaphore, class Mutex>
    class BaseCond
    {
    public:
	BaseCond(typename Mutex& lock,
		 typename Semaphore& gate,
		 typename Semaphore& blocked)
	  : _internal(lock), _gate(gate), _blocked(blocked),
	    _unblocked(0), _toUnblock(0)
	{
	}
	
	~BaseCond()
	{
	}

	void signal() {
	    wake(false);
	}
	    
	void broadcast() {
	    wake(true);
	}

	template <typename Lock>
	void wait(const Lock& lock) const
	{
	    // precondition: lock acquired
	    waitImpl(lock._mutex);
	}

	template <typename Lock>
	bool timedWait(const Lock& lock, const Time& timeout) const
	{
	    // precondition: lock acquired
	    return timedWaitImpl(lock._mutex, timeout);
	}
	    
    private:
	friend template <typename M, typename C> class Monitor<M,C>;
	    
	BaseCond(const BaseCond&);		// disallow copy ctor
	void operator=(const BaseCond&);	// disallow assignment operator
	    
	template <typename M>
	void waitImpl(const M& mutex) const
	{
	    _gate.wait();
	    _blocked++;
	    _gate.post();

	    typedef typename M::LockState LockState;

	    LockState state;
	    mutex.unlock(state);

	    _queue.wait();
	    _internal.lock();
	    _unblocked++;

	    if(_toUnblock != 0) {
		bool last = --_toUnblock == 0;
		_internal.unlock();
	
		if(last)
		    _gate.post();
		else
		    _queue.post();
	    }
	    else
		_internal.unlock();

	    mutex.lock(state);
	}

	template <typename M>
	bool timedWaitImpl(const M& mutex, const Time& timeout) const
	{
	    _gate.wait();
	    _blocked++;
	    _gate.post();
		
	    typedef typename M::LockState LockState;

	    LockState state;
	    mutex.unlock(state);
		
	    bool rc = _queue.timedWait(timeout);

	    _internal.lock();
	    _unblocked++;

	    if(_toUnblock != 0) {
		bool last = --_toUnblock == 0;
		_internal.unlock();
	
		if(!rc)
		    _queue.wait();	// timed out
	
		if(last)
		    _gate.post();
		else
		    _queue.post();
	    }
	    else
		_internal.unlock();

	    mutex.lock(state);
	    return rc;
	}
	    
	void wake(bool broadcast) {
	    // Lock gate & mutex.
	    _gate.wait();
	    _internal.lock();

	    if(_unblocked != 0) {
		_blocked -= _unblocked;
		_unblocked = 0;
	    }

	    if(_blocked > 0) {
		// Unblock some number of waiters.
		_toUnblock = (broadcast) ? _blocked : 1;
		_internal.unlock();
		_queue.post();
	    }
	    else {
		// Otherwise no blocked waiters, release gate & mutex.
		_gate.post();
		_internal.unlock();
	    }
	}

	Mutex _internal;
	Semaphore _gate;
	Semaphore _queue;
	mutable long _blocked;
	mutable long _unblocked;
	mutable long _toUnblock;
    };


    namespace ShMem {
	
	//----------------------------------------------
	// Condition variables using SHARED MEMORY 

	typedef typename MPS::BaseCond<Semaphore, Mutex> BaseCond;

	// The actual Condition Variable class for SHARED MEMORY
	class Cond : public BaseCond
	{
	public:
	    Cond(unsigned locks, unsigned counters, bool init = false)
		: _gate(locks, counters, init, 1),
		  _blocked(locks+1, counters+1, init, 0),
		  _lock(locks+2),
		  BaseCond(_lock, _gate, _blocked)
	    {
	    }
	    
	private:
	    Semaphore _gate;
	    Semaphore _blocked;
	    Mutex _lock;
	};
	
    }

    namespace ScrMem {

	//----------------------------------------------
	// Condition variables using SCRATCH MEMORY 

	typedef typename MPS::BaseCond<Semaphore, Mutex> BaseCond;

	// The actual Condition Variable class for SCRATCH MEMORY
	class Cond : public BaseCond
	{
	public:
	    Cond(unsigned core, unsigned semaphores, bool init = false)
		: _gate(core, semaphores, init, 1),
		  _blocked(core, semaphores+1, init, 0),
		  _lock(core, semaphores+2, init, 1),
		  BaseCond(_lock, _gate, _blocked)
	    {
	    }
	    
	private:
	    Semaphore _gate;
	    Semaphore _blocked;
	    Mutex _lock;
	};
	
    }
    
}

#endif
