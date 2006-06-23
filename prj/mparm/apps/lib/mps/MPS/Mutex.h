#ifndef MPS_MUTEX_H
#define MPS_MUTEX_H

//------------------------------------------------------------------------
// Non-recursive Mutex implementation for inter-processor synchronization

// Shared memory semaphores have different semantics than scratchpad
// memories semaphores.  Hence we need to provide two set of
// implementations (MPS::ShMem::Mutex, and MPS::ScrMem::Mutex).
// Please see core/ext_mem.h and core/scratch_mem.h for information on
// the precise semantics.

// Mutex are meant to be used for inter-processor synchronization.  If
// intra-processor syncronization is also required then you should use
// SMutex.

#include <MPS/Config.h>
#include <IceUtil/Lock.h>

namespace MPS {

    namespace ShMem {

	//----------------------------------------------
	// Non-recursive mutexes using SHARED MEMORY 
    
	class Cond;	// See Cond.h

	// Non thread-shareable mutex (to be instantiated on each thread)
	class Mutex
	{
	    friend class Cond;

	public:
	    typedef IceUtil::LockT<Mutex> Lock;
	    typedef IceUtil::TryLockT<Mutex> TryLock;

	    Mutex(unsigned id) : _semaphore(semaphore_base[id])
	    {
	    }

	    ~Mutex()
	    {
	    }

	    void lock() const
	    {
		while(_semaphore)
		    sleep(0);	// yields the CPU
	    }

	    bool tryLock() const
	    {
		if (!_semaphore)
		    return true;	// lock acquired
		else
		    return false;	// already locked
	    }

	    void unlock() const
	    {
		_semaphore = 0;
	    }

	    bool willUnlock() const
	    {
		return true;	// non-recursive mutexes will always unlock
	    }

	private:
	    Mutex(const Mutex&);		// disallow copy ctor
	    void operator=(const Mutex&);	// disallow assignment operator

	    struct LockState {};
	    
	    void unlock(LockState& state) const
	    {
		unlock();
	    }
	
	    void lock(LockState& state) const
	    {
		lock();
	    }
	    
	    volatile int& _semaphore;
	};

    }

    namespace ScrMem {

	//-----------------------------------------------
	// Non-recursive mutexes using SCRATCH MEMORY 

	class Cond;	// See Cond.h

	// Non thread-shareable mutex (to be instantiated on each thread)
	class Mutex
	{
	    friend class Cond;

	public:
	    typedef IceUtil::LockT<Mutex> Lock;
	    typedef IceUtil::TryLockT<Mutex> TryLock;

	    Mutex(unsigned core, unsigned id, bool init = false)
		: _semaphore(semaphore_base(core)[id])
	    {
		if (init)
		    _semaphore = 2;	// init to 1 (unlocked)
	    }

	    ~Mutex()
	    {
	    }

	    void lock() const
	    {
		while(!_semaphore)
		    sleep(0);	// yields the CPU
	    }

	    bool tryLock() const
	    {
		if (_semaphore)
		    return true;	// lock acquired
		else
		    return false;	// already locked
	    }

	    void unlock() const
	    {
		_semaphore = 0;		// increment semaphore
	    }

	    bool willUnlock() const
	    {
		return true;	// non-recursive mutexes will always unlock
	    }

	private:
	    Mutex(const Mutex&);		// disallow copy ctor
	    void operator=(const Mutex&);	// disallow assignment operator

	    volatile int& _semaphore;
	};

    }
    
}
#endif
