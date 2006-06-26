#ifndef MPS_RW_REC_MUTEX_H
#define MPS_RW_REC_MUTEX_H

//-----------------------------------------------------------------
// Readers/Writers Mutex implementation

// Concurrency primitive that allows many readers & one writer access
// to a data structure. Writers have priority over readers. The
// structure is not strictly fair in that there is no absolute queue
// of waiting writers - that is managed by a condition variable.

// Both Reader & Writer mutexes can be recursively locked. Calling
// upgrade() or timedUpgrade() while holding a read lock promotes the
// reader to a writer lock.

// This file is heavily based on IceUtil/RWRecMutex.h

#include <MPS/Mutex.h>
#include <MPS/Cond.h>

namespace MPS {


    template <class Mutex, class Cond>
    class BaseRWRecMutex
    {
    public:
	typedef RLockT<BaseRWRecMutex<Mutex,Cond> > RLock;
	typedef TryRLockT<BaseRWRecMutex<Mutex,Cond> > TryRLock;
	typedef WLockT<BaseRWRecMutex<Mutex,Cond> > WLock;
	typedef TryWLockT<BaseRWRecMutex<Mutex,Cond> > TryWLock;

	BaseRWRecMutex(volatile int& count,
		   volatile int& waitingWriters,
		   volatile bool& upgrading,
		   Mutex& mutex,
		   Cond& readers, Cond& writers, Cond& upgrader)
	    : _count(count),
	      _waitingWriters(waitingWriters),
	      _upgrading(upgrading),
	      _mutex(mutex), 
	      _readers(readers),
	      _writers(writers),
	      _upgrader(upgrader)
	{
	    if (init) {
		_count = 0;
		_waitingWriters = 0;
		_upgrading = false;
	    }
	}
	
	~BaseRWRecMutex()
	{
	}

	void readLock() const
	{
	    Mutex::Lock lock(_mutex);

	    // Wait while a writer holds the lock or while writers are
	    // waiting to get the lock.
	    while(_count < 0 || _waitingWriters != 0)
		_readers.wait(lock);
	    _count++;
	}

	bool tryReadLock() const
	{
	    Mutex::Lock lock(_mutex);

	    // Would block if a writer holds the lock or if writers
	    // are waiting to get the lock.
	    if(_count < 0 || _waitingWriters != 0)
		return false;
	    _count++;
	    return true;
	}

	bool timedReadLock(const Time& timeout) const
	{
	    Mutex::Lock lock(_mutex);

	    // Wait while a writer holds the lock or while writers are
	    // waiting to get the lock.
	    Time end = Time::now() + timeout;
	    while(_count < 0 || _waitingWriters != 0) {
		Time remainder = end - Time::now();
		if(remainder > Time()) {
		    if(_readers.timedWait(lock, remainder) == false)
			return false;
		}
		else
		    return false;
	    }
	    _count++;
	    return true;
	}

	void writeLock() const
	{
	    Mutex::Lock lock(_mutex);

	    // FIXME: threadcontrol!
	    if(_count < 0 && _writerId == ThreadControl()) {
		--_count;
		return;
	    }

	    // Wait for the lock to become available and increment the
	    // number of waiting writers.
	    while(_count != 0) {
		++_waitingWriters;
		try {
		    _writers.wait(lock);
		}
		catch(...) {
		    --_waitingWriters;
		    throw;
		}
		_waitingWriters--;
	    }

	    // Got the lock, indicate it's held by a writer.
	    _count = -1;
	    // FIXME: thread control!!!
	    _writerId = ThreadControl();
	}

	bool tryWriteLock() const
	{
	    Mutex::Lock lock(_mutex);

	    // If the mutex is already write locked by this writer
	    // then decrement _count, and return.
	    // FIXME: thread control!
	    if(_count < 0 && _writerId == ThreadControl()) {
		--_count;
		return true;
	    }

	    // If there are readers or other writers then the call
	    // would block.
	    if(_count != 0)
		return false;

	    // Got the lock, indicate it's held by a writer.
	    _count = -1;
	    // FIXME: thread control!
	    _writerId = ThreadControl();
	    return true;
	}

	bool timedWriteLock(const Time&) const
	{
	    Mutex::Lock lock(_mutex);

	    // If the mutex is already write locked by this writer
	    // then decrement _count, and return.
	    // FIXME: thread control!
	    if(_count < 0 && _writerId == ThreadControl()) {
		--_count;
		return true;
	    }

	    // Wait for the lock to become available and increment the
	    // number of waiting writers.
	    Time end = Time::now() + timeout;
	    while(_count != 0) {
		Time remainder = end - Time::now();
		if(remainder > Time()) {
		    ++_waitingWriters;
		    try {
			bool result = _writers.timedWait(lock, remainder);
			_waitingWriters--;
			if(result == false) {
			    return false;
			}
		    }
		    catch(...) {
			--_waitingWriters;
			throw;
		    }
		}
		else
		    return false;
	    }

	    // Got the lock, indicate it's held by a writer.
	    _count = -1;
	    // FIXME: thread control!
	    _writerId = ThreadControl();
	    return true;
	}

	void unlock() const
	{
	    bool ww;
	    bool wr;
	    {
		Mutex::Lock lock(_mutex);

		// If _count < 0, the calling thread is a writer that
		// holds the lock, so release the lock.  Otherwise,
		// _count is guaranteed to be > 0, so the calling
		// thread is a reader releasing the lock.
		if(_count < 0) {
		    // Writer called unlock
		    ++_count;

		    // If the write lock wasn't totally released we're done.
		    if(_count != 0)
			return;
		}
		else // Reader called unlock
		    --_count;

		// Writers are waiting (ww) if _waitingWriters > 0.
		// In that case, it's OK to let another writer into
		// the region once there are no more readers (_count
		// == 0).  Otherwise, no writers are waiting but
		// readers may be waiting (wr).
		ww = (_waitingWriters != 0 && _count == 0);
		wr = (_waitingWriters == 0);
	    }

	    // Wake up a waiting writer if there is one. If not, wake
	    // up all readers (just in case -- there may be none).
	    if(ww) {
		if(_upgrading)	// Run untimed upgraders
		    _upgrader.signal();
		else		// Wake a normal writer
		    _writers.signal();
	    }
	    else if(wr)		// Wake readers
		_readers.broadcast();
	}

	// Upgrade the read lock to a writer lock. Note that this
	// method can only be called if the reader lock is not held
	// recursively.
	void upgrade() const
	{
	    Mutex::Lock lock(_mutex);

	    // precondition: _upgrading = false

	    --_count;
   
	    // Wait to acquire the write lock.
	    _upgrading = true;
	    while(_count != 0) {
		++_waitingWriters;
		try {
		    _upgrader.wait(lock);
		}
		catch(...) {
		    _upgrading = false;
		    --_waitingWriters;
		    ++_count;
		    throw;
		}
		--_waitingWriters;	
	    }

	    // Got the lock, indicate it's held by a writer.
	    _count = -1;
	    _writerId = ThreadControl();
	    _upgrading = false;
	}

	// Upgrade the read lock to a writer lock for up to the given
	// timeout Note that this method can only be called if the
	// reader lock is not held recursively.
	bool timedUpgrade(const Time&) const
	{
	    Mutex::Lock lock(_mutex);

	    if(_upgrading)
		return false;

	    --_count;

	    // Wait to acquire the write lock.
	    _upgrading = true;
	    Time end = Time::now() + timeout;
	    while(_count != 0) {
		Time remainder = end - Time::now();
		if(remainder > Time()) {
		    ++_waitingWriters;
		    try {
			bool result = _upgrader.timedWait(lock, remainder);
			--_waitingWriters;
			if(!result) {
			    _upgrading = false;
			    ++_count;
			    return false;
			}
		    }
		    catch(...) {
			_upgrading = false;
			--_waitingWriters;
			++_count;
			throw;
		    }
		}
		else {
		    // The lock isn't acquired if a timeout occurred.
		    ++_count;
		    _upgrading = false;
		    return false;
		}
	    }

	    // Got the lock, indicate it's held by a writer.
	    _count = -1;
	    _writerId = ThreadControl();
	    _upgrading = false;
	    return true;
	}

	// Downgrade a write lock to a read lock.
	void downgrade() const
	{
	    Mutex::Lock lock(_mutex);

	    if(++_count == 0)
		_count = 1;
	}

    private:
	BaseRWRecMutex(const BaseRWRecMutex&);	// disallow copy ctor
	void operator=(const BaseRWRecMutex&);	// disallow assignment operator

	// Number of readers holding the lock. A positive number
	// indicates readers are active. A negative number means that
	// a writer is active.
	mutable volatile int& _count;

	// If there is an active writer this is the ID of the writer thread.
	mutable ThreadControl _writerId;

	// Number of waiting writers.
	mutable volatile int& _waitingWriters;

	// True if an upgrader wants the lock.
	mutable volatile bool& _upgrading;

	// Internal mutex.
	Mutex& _mutex;

	// Two condition variables for waiting readers & writers.
	mutable Cond& _readers;
	mutable Cond& _writers;
	mutable Cond& _upgrader;
    };

}

#endif
