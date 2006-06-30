#ifndef MPS_MUTEX_H
#define MPS_MUTEX_H

//------------------------------------------------------------------------
// Non-recursive Mutex implementation for inter-processor synchronization

// Shared memory semaphores have different semantics than scratchpad
// memories semaphores.  Hence we need to provide two set of
// implementations (MPS::ShMem::Mutex, and MPS::ScrMem::Mutex).
// Please see core/ext_mem.h and core/scratch_mem.h for information on
// the precise semantics.

#include <MPS/Config.h>
#include <MPS/Lock.h>

namespace MPS {

    class Cond;	// See Cond.h

    template <class MutexState>
    class BaseMutex
    {
	friend class Cond;

    public:
	typedef LockT<BaseMutex<MutexState> > Lock;
	typedef TryLockT<BaseMutex<MutexState> > TryLock;

	BaseMutex(MutexState& state) : _state(state)
	{
	}

	void lock() const
	{
	    while(!_state.tryLock())
		_state.yield();
	}

	bool tryLock() const
	{
	    return _state.tryLock();
	}

	void unlock() const
	{
	    _state.unlock();
	}

	bool willUnlock() const
	{
	    return _state.willUnlock();
	}

    private:
	BaseMutex(const BaseMutex&);		// disallow copy ctor
	void operator=(const BaseMutex&);	// disallow assignment operator

	struct LockState {};
	    
	void unlock(LockState& state) const
	{
	    unlock();
	}
	
	void lock(LockState& state) const
	{
	    lock();
	}

    private:
	MutexState& _state;
    };

    // Dummy mutexes are fake locks. They are used on containers to
    // avoid having a specialized version without locking.
    
    class DummyMutexState
    {
    public:
	bool tryLock() const { return true; }
	void unlock() const { }
	bool willUnlock() const { return true; }
	void yield() const { }
    };

    
    typedef MPS::BaseMutex<DummyMutexState> BaseDummyMutex;

    class DummyMutex : public BaseDummyMutex
    {
    public:
	DummyMutex() : BaseDummyMutex(_state)
	{
	}

    private:
	DummyMutex(const DummyMutex&);		// disallow copy ctor
	void operator=(const DummyMutex&);	// disallow assignment operator

	DummyMutexState _state;
    };

}

# include <MPS/ShMem/Mutex.h>
# include <MPS/ScrMem/Mutex.h>

#endif
