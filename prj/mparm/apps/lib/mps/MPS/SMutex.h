#ifndef MPS_SMUTEX_H
#define MPS_SMUTEX_H

//------------------------------------------------------------------------
// Non-recursive Mutex implementation for inter-processor and
// intra-processor synchronization

// Shared memory semaphores have different semantics than scratchpad
// memories semaphores.  Hence we need to provide two set of
// implementations (MPS::ShMem::SMutex, and MPS::ScrMem::SMutex).
// Please see core/ext_mem.h and core/scratch_mem.h for information on
// the precise semantics.

// SMutex are meant to be used for both inter-processor and
// intra-processor synchronization.  If only inter-processor
// syncronization is required then you should use Mutex.

#include <MPS/Config.h>
#include <IceUtil/Mutex.h>
#include <IceUtil/Lock.h>

namespace MPS {

    namespace ShMem {

	//----------------------------------------------
	// Non-recursive mutexes using SHARED MEMORY 
    
	class SCond;	// See Cond.h

	// Thread shareable version
	class SMutex : public IceUtil::Mutex
	{
	    friend class SCond;

	public:
	    typedef IceUtil::LockT<SMutex> Lock;
	    typedef IceUtil::TryLockT<SMutex> TryLock;

	    SMutex(unsigned id)
	      : IceUtil::Mutex(), _semaphore(semaphore_base[id])
	    {
	    }

	    ~SMutex()
	    {
	    }

	    void lock() const
	    {
		IceUtil::Mutex::lock();
		while(_semaphore)
		    sleep(0);	// yields the CPU
	    }

	    bool tryLock() const
	    {
		if (!IceUtil::Mutex::tryLock())
		    return false;
		else if (!_semaphore)
		    return true;	// lock acquired
		else {
		    IceUtil::Mutex::unlock();
		    return false;	// already locked
		}
	    }

	    void unlock() const
	    {
		_semaphore = 0;
		IceUtil::Mutex::unlock();
	    }

	    bool willUnlock() const
	    {
		return true;	// non-recursive locked mutexes will
				// always unlock
	    }

	private:
	    typedef IceUtil::Mutex::LockState LockState;

	    SMutex(const SMutex&);		// disallow copy ctor
	    void operator=(const SMutex&);	// disallow assignment operator

	    void unlock(LockState& state) const
	    {
		unlock();
		IceUtil::Mutex::unlock(state);
	    }
    
	    void lock(LockState& state) const
	    {
		IceUtil::Mutex::lock(state);
		lock();
	    }

	    mutable volatile int& _semaphore;
	};

    }

    namespace ScrMem {

	//-----------------------------------------------
	// Non-recursive mutexes using SCRATCH MEMORY 

	class SCond;	// See Cond.h

	// Thread shareable version
	class SMutex : public IceUtil::Mutex
	{
	    friend class SCond;

	public:
	    typedef IceUtil::LockT<SMutex> Lock;
	    typedef IceUtil::TryLockT<SMutex> TryLock;

	    SMutex(unsigned core, unsigned id, bool init = false)
		: IceUtil::Mutex(), _semaphore(semaphore_base(core)[id])
	    {
		if (init)
		    _semaphore = 2;	// init to 1 (unlocked)
	    }

	    ~SMutex()
	    {
	    }

	    void lock() const
	    {
		IceUtil::Mutex::lock();
		while(!_semaphore)
		    sleep(0);	// yields the CPU
	    }

	    bool tryLock() const
	    {
		if (!IceUtil::Mutex::tryLock())
		    return false;
		if (_semaphore)
		    return true;	// lock acquired
		else {
		    IceUtil::Mutex::unlock();
		    return false;	// already locked
		}
	    }

	    void unlock() const
	    {
		_semaphore = 0;		// increment semaphore
		IceUtil::Mutex::unlock();
	    }

	    bool willUnlock() const
	    {
		return true;	// non-recursive locked mutexes will
				// always unlock
	    }

	private:
	    typedef IceUtil::Mutex::LockState LockState;
	    
	    SMutex(const SMutex&);		// disallow copy ctor
	    void operator=(const SMutex&);	// disallow assignment operator

	    void unlock(LockState& state) const
	    {
		unlock();
		IceUtil::Mutex::unlock(state);
	    }
    
	    void lock(LockState& state) const
	    {
		IceUtil::Mutex::lock(state);
		lock();
	    }

	    mutable volatile int& _semaphore;
	};

    }
    
}

#endif
