#ifndef MPS_REC_MUTEX_H
#define MPS_REC_MUTEX_H

//---------------------------------------------------------------------
// Recursive Mutex implementation for inter-processor synchronization

// Shared memory semaphores have different semantics than scratchpad
// memories semaphores.  Hence we need to provide two set of
// implementations (MPS::ShMem::RecMutex, and MPS::ScrMem::RecMutex).
// Please see core/ext_mem.h and core/scratch_mem.h for information on
// the precise semantics.

#include <MPS/Mutex.h>

namespace MPS {

    template <class MutexState>
    class BaseRecMutex
    {
	friend class Cond;

    public:
	typedef LockT<BaseRecMutex<MutexState> > Lock;
	typedef TryLockT<BaseRecMutex<MutexState> > TryLock;

	BaseRecMutex(MutexState& state)
	    : _state(state), _count(0)
	{
	}
	
	void lock() const {
	    if (_count)
		++_count;
	    else {
		while(!_state.tryLock())
		    _state.yield();
		_count = 1;
	    }
	}

	bool tryLock() const {
	    if (_state.tryLock()) {
		_count = 1;
		return true;	// lock acquired
	    }
	    else
		return false;	// already locked
	}

	void unlock() const {
	    if (!--_count)
		_state.unlock();
	}

	bool willUnlock() const {
	    return _count == 1;
	}

    private:
	BaseRecMutex(const BaseRecMutex&);	// disallow copy ctor
	void operator=(const BaseRecMutex&);	// disallow assignment operator

	struct LockState {
	    int count;
	};

	void unlock(LockState& state) const {
	    state.count = _count;
	    _count = 0;
	    _state.unlock();
	}
	
	void lock(LockState& state) const {
	    while(!_state.tryLock())
		_state.yield();
	    _count = state.count;
	}

	MutexState& _state;
	mutable int _count;
    };
}

# include <MPS/ShMem/RecMutex.h>
# include <MPS/ScrMem/RecMutex.h>

#endif
