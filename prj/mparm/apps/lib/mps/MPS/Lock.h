#ifndef MPS_LOCK_H
#define MPS_LOCK_H

//-----------------------------------------------------------------
// Lock templates

// LockT and TryLockT are the preferred construct to
// lock/tryLock/unlock simple and recursive mutexes. You typically
// allocate them on the stack to hold a lock on a mutex.

// LockT and TryLockT are not recursive: you cannot acquire several
// times in a row a lock with the same Lock or TryLock object.

// This code is almost identical to 

namespace MPS {

    class ShMem::Cond;
    class ScrMem::Cond;

    template <typename T>
    class LockT
    {
    public:
	LockT(const T& mutex) : _mutex(mutex)
	{
	    _mutex.lock();
	    _acquired = true;
	}

	~LockT()
	{
	    if (_acquired)
		_mutex.unlock();
	}
    
	void acquire() const
	{
	    // precondition: _acquired = false
	    _mutex.lock();
	    _acquired = true;
	}


	bool tryAcquire() const
	{
	    // precondition: _acquired = false
	    _acquired = _mutex.tryLock();
	    return _acquired;
	}

	void release() const
	{
	    // precondition: _acquired = true
	    _mutex.unlock();
	    _acquired = false;
	}

	bool acquired() const
	{
	    return _acquired;
	}
   
    protected:
	LockT(const T& mutex, bool) : _mutex(mutex)
	{
	    _acquired = _mutex.tryLock();
	}

    private:
	LockT(const LockT&);		// disallow copy ctor
	LockT& operator=(const LockT&);	// disallow assignment operator

	const T& _mutex;
	mutable bool _acquired;

	friend class ShMem::Cond;
	friend class ScrMem::Cond;
    };

    template <typename T>
    class TryLockT : public LockT<T>
    {
    public:
	TryLockT(const T& mutex) : LockT<T>(mutex, true)
	{}
    };

}

#endif
