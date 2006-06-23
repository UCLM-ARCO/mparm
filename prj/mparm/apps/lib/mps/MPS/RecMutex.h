#ifndef MPS_REC_MUTEX_H
#define MPS_REC_MUTEX_H

//---------------------------------------------------------------------
// Recursive Mutex implementation for inter-processor synchronization

// Shared memory semaphores have different semantics than scratchpad
// memories semaphores.  Hence we need to provide two set of
// implementations (MPS::ShMem::RecMutex, and MPS::ScrMem::RecMutex).
// Please see core/ext_mem.h and core/scratch_mem.h for information on
// the precise semantics.

// RecMutex are meant to be used for inter-processor synchronization.
// If intra-processor syncronization is also required then you should
// use SRecMutex.

#include <MPS/Config.h>
#include <IceUtil/Lock.h>

namespace MPS {

    namespace ShMem {

	//----------------------------------------------
	// Recursive mutexes using SHARED MEMORY 
    
	class Cond;	// See Cond.h

	// Non thread-shareable mutex (to be instantiated on each thread)
	class RecMutex
	{
	    friend class Cond;

	public:
	    typedef IceUtil::LockT<RecMutex> Lock;
	    typedef IceUtil::TryLockT<RecMutex> TryLock;

	    RecMutex(unsigned id) : _semaphore(semaphore_base[id]), _count(0)
	    {
	    }
	
	    ~RecMutex() {
	    }

	    void lock() const {
		if (_count)
		    ++_count;
		else {
		    while(_semaphore)
			sleep(0);	// yields the CPU
		    _count = 1;
		}
	    }

	    bool tryLock() const {
		if (!_semaphore) {
		    _count = 1;
		    return true;	// lock acquired
		}
		else
		    return false;	// already locked
	    }

	    void unlock() const {
		if (!--_count)
		    _semaphore = 0;
	    }

	    bool willUnlock() const {
		return _count == 1;
	    }

	private:
	    RecMutex(const RecMutex&);		// disallow copy ctor
	    void operator=(const RecMutex&);	// disallow assignment operator

	    struct LockState {
		int count;
	    };

	    void unlock(LockState& state) const {
		state.count = _count;
		_count = 0;
		_semaphore = 0;
	    }
	
	    void lock(LockState& state) const {
		while(_semaphore)
		    sleep(0);	// yields the CPU
		_count = state.count;
	    }

	    mutable volatile int& _semaphore;
	    mutable int _count;
	};

    }

    namespace ScrMem {

	//----------------------------------------------
	// Recursive mutexes using SCRATCH MEMORY 
    
	class Cond;	// See Cond.h

	// Non thread-shareable mutex (to be instantiated on each thread)
	class RecMutex
	{
	    friend class Cond;

	public:
	    typedef IceUtil::LockT<RecMutex> Lock;
	    typedef IceUtil::TryLockT<RecMutex> TryLock;

	    RecMutex(unsigned core, unsigned id, bool init = false)
		: _semaphore(semaphore_base(core)[id]), _count(0)
	    {
		if (init)
		    _semaphore = 2;	// init to 1 (unlocked)
	    }
	
	    ~RecMutex() {
	    }

	    void lock() const {
		if (_count)
		    ++_count;
		else {
		    while(!_semaphore)
			sleep(0);	// yields the CPU
		    _count = 1;
		}
	    }

	    bool tryLock() const {
		if (_semaphore) {
		    _count = 1;
		    return true;	// lock acquired
		}
		else
		    return false;	// already locked
	    }

	    void unlock() const {
		if (!--_count)
		    _semaphore = 0;
	    }

	    bool willUnlock() const {
		return _count == 1;
	    }

	private:
	    RecMutex(const RecMutex&);		// disallow copy ctor
	    void operator=(const RecMutex&);	// disallow assignment operator

	    struct LockState {
		int count;
	    };

	    void unlock(LockState& state) const {
		state.count = _count;
		_count = 0;
		_semaphore = 0;
	    }
	
	    void lock(LockState& state) const {
		while(_semaphore)
		    sleep(0);	// yields the CPU
		_count = state.count;
	    }

	    mutable volatile int& _semaphore;
	    mutable int _count;
	};

    }

}
#endif
