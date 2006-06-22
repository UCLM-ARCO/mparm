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

#include <MPS/Mutex.h>
#include <IceUtil/Time.h>

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
	friend template <typename M> class Monitor<M>;
	    
	BaseCond(const BaseCond&);		// disallow copy ctor
	void operator=(const BaseCond&);	// disallow assignment operator
	    
	template <typename M>
	void waitImpl(const M& mutex) const
	{
	    _gate.wait();
	    _blocked++;
	    _gate.post();
		
	    mutex.unlock();

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

	    mutex.lock();
	}

	template <typename M>
	bool timedWaitImpl(const M& mutex, const Time& timeout) const
	{
	    _gate.wait();
	    _blocked++;
	    _gate.post();
		
	    mutex.unlock();
		
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

	    mutex.lock();
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

	// The semantics of current shared semaphores does only allow
	// implementing mutexes. Therefore we need an additional
	// shared memory counter.
	
	// POSIX style semaphores for SHARED MEMORY
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

	typedef typename MPS::BaseCond<Semaphore, Mutex> BaseCond;

	// The actual Condition Variable class for SHARED MEMORY
	class Cond : public BaseCond {
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

	// The semantics of scratch semaphores fits quite nicely in
	// the POSIX style implementation.  Therefore they are far
	// more efficient than SHARED MEMORY semaphores.

	// POSIX style semaphores for SCRATCH MEMORY
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

	typedef typename MPS::BaseCond<Semaphore, Mutex> BaseCond;

	// The actual Condition Variable class for SCRATCH MEMORY
	class Cond : public BaseCond {
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
