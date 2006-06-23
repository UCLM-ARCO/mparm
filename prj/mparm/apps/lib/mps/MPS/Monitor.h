#ifndef MPS_MONITOR_H
#define MPS_MONITOR_H

//------------------------------------------------------------------------
// Monitor implementation for inter-processor synchronization

// These monitors implement the Mesa monitor semantics. That is any
// calls to notify() or notifyAll() are delayed until the monitor is
// unlocked.

// This implementation is mostly taken from IceUtil/Monitor.h

#include <IceUtil/Lock.h>
#include <MPS/Cond.h>

namespace MPS {

    // Mesa semantics monitors (non-shareable version)
    //
    // This class is not meant to be used directly (see
    // MPS::ShMem::Monitor and MPS::ScrMem::Monitor)

    template <class Mutex, class Cond>
    class BaseMonitor
    {
    public:

	typedef LockT<BaseMonitor<Mutex, Cond> > Lock;
	typedef TryLockT<BaseMonitor<Mutex, Cond> > TryLock;

	BaseMonitor(Cond& cond, Mutex& mutex,
		    volatile int& nnotify, bool init = false)
	    : _cond(cond), _mutex(mutex), _nnotify(nnotify)
	{
	    if (init)
		_nnotify = 0;
	}
	
	~BaseMonitor()
	{
	}

	void lock() const
	{
	    _mutex.lock();
	    if(_mutex.willUnlock())
		_nnotify = 0;
	}
	
	void unlock() const
	{
	    if(_mutex.willUnlock()) {
		if(_nnotify != 0)
		    notifyImpl(_nnotify);
	    }
	    _mutex.unlock();
	}
	
	bool tryLock() const
	{
	    bool result = _mutex.tryLock();
	    if(result && _mutex.willUnlock())
		_nnotify = 0;
	    return result;
	}

	void wait() const
	{
	    if(_nnotify != 0)
		notifyImpl(_nnotify);
	    try {
		_cond.waitImpl(_mutex);
	    }
	    catch(...) {
		_nnotify = 0;
		throw;
	    }
	    _nnotify = 0;
	}
	
	bool timedWait(const Time&) const
	{
	    if(_nnotify != 0)
		notifyImpl(_nnotify);
	    
	    bool rc;
	    try {
		rc = _cond.timedWaitImpl(_mutex, timeout);
	    }
	    catch(...) {
		_nnotify = 0;
		throw;
	    }

	    _nnotify = 0;
	    return rc;
	}
	
	void notify()
	{
	    if(_nnotify != -1)
		++_nnotify;
	}
	
	void notifyAll()
	{
	    _nnotify = -1;			// -1 indicates broadcast
	}

    private:

	BaseMonitor(const BaseMonitor&);	// disallow copy ctor
	void operator=(const BaseMonitor&);	// disallow assignment operator

	void notifyImpl(int nnotify) const
	{
	    if(nnotify == -1) {
		_cond.broadcast();
		return;
	    }
	    else {
		while(nnotify > 0) {
		    _cond.signal();
		    --nnotify;
		}
	    }
	}

	mutable Cond& _cond;
	Mutex& _mutex;
	volatile int& _nnotify;
    };

    namespace ShMem {

	//----------------------------------------------
	// Monitor using SHARED MEMORY 

	// It requires 4 semaphores and a 3 shared counters
	template <class M>
	class Monitor : public MPS::BaseMonitor<M, Cond>
	{
	    typedef typename MPS::BaseMonitor<M, Cond> BaseMonitor;

	public:
	    Monitor(unsigned locks, unsigned counters, bool init = false)
		: _cond(locks, counters, init),
		  _mutex(locks+3),
		  BaseMonitor(_cond, _mutex, memory_base[counter+2])
	    {
	    }
	    
	private:
	    Cond _cond;
	    Mutex _mutex;
	};
	
    }

    namespace ScrMem {

	//----------------------------------------------
	// Monitors using SCRATCH MEMORY 

	// It requires 4 semaphores and a shared counter
	template <class M>
	class Monitor : public MPS::BaseMonitor<M, Cond>
	{
	    typedef typename MPS::BaseMonitor<M, Cond> BaseMonitor;

	public:
	    Monitor(unsigned core, unsigned semaphores,
		    unsigned counter, bool init = false)
		: _cond(core, semaphores, init),
		  _mutex(core, semaphores+3, init, 1),
		  BaseMonitor(_cond, _mutex, memory_base(core)[counter])
	    {
	    }
	    
	private:
	    Cond _cond;
	    Mutex _mutex;
	};

    }
}

#endif
