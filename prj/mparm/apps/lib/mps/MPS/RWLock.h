#ifndef MPS_RW_LOCK_H
#define MPS_RW_LOCK_H

#include <MPS/Mutex.h>
#include <MPS/Cond.h>

namespace MPS {

    //---------------------------------------------------
    // Lock template for readers
    template <typename T>
    class RLockT
    {
    public:
	RLockT(const T& mutex) : _mutex(mutex)
	{
	    _mutex.readLock();
	    _acquired = true;
	}

	~RLockT()
	{
	    if (_acquired)
		_mutex.unlock();
	}

	void acquire() const
	{
	    // precondition: _acquired = false
	    _mutex.readLock();
	    _acquired = true;
	}

	bool tryAcquire() const
	{
	    // precondition: _acquired = false
	    _acquired = _mutex.tryReadLock();
	    return _acquired;
	}

	bool timedAcquire(const Time& timeout) const
	{
	    // precondition: _acquired = false
	    _acquired = _mutex.timedReadLock(timeout);
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

	void upgrade() const
	{
	    _mutex.upgrade();
	}

	bool timedUpgrade(const Time& timeout) const
	{
	    return _mutex.timedUpgrade(timeout);
	}

	void downgrade() const
	{
	    _mutex.downgrade();
	}

    protected:
	RLockT(const T& mutex, bool) : _mutex(mutex)
	{
	    _acquired = _mutex.tryReadLock();
	}


	RLockT(const T& mutex, const Time& timeout) : _mutex(mutex)
	{
	    _acquired = _mutex.timedReadLock(timeout);
	}

    private:
	RLockT(const RLockT&);			// disallow copy ctor
	RLockT& operator=(const RLockT&);	// disallow assignment operator

	const T& _mutex;
	mutable bool _acquired;
    };

    //---------------------------------------------------
    // TryLock template for readers
    template <typename T>
    class TryRLockT : public RLockT<T>
    {
    public:
	TryRLockT(const T& mutex) : RLockT<T>(mutex, true)
	{
	}

	TryRLockT(const T& mutex, const Time& timeout)
	    : RLockT<T>(mutex, timeout)
	{
	}
	
    };

    //---------------------------------------------------
    // Lock template for writers
    template <typename T>
    class WLockT
    {
    public:
	WLockT(const T& mutex) : _mutex(mutex)
	{
	    _mutex.writeLock();
	    _acquired = true;
	}

	~WLockT()
	{
	    if (_acquired)
		_mutex.unlock();
	}

	void acquire() const
	{
	    // precondition: _acquired = false
	    _mutex.writeLock();
	    _acquired = true;
	}

	bool tryAcquire() const
	{
	    // precondition: _acquired = false
	    _acquired = _mutex.tryWriteLock();
	    return _acquired;
	}

	bool timedAcquire(const Time& timeout) const
	{
	    // precondition: _acquired = false
	    _acquired = _mutex.timedWriteLock(timeout);
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
	WLockT(const T& mutex, bool) : _mutex(mutex)
	{
	    _acquired = _mutex.tryWriteLock();
	}

	WLockT(const T& mutex, const Time& timeout) : _mutex(mutex)
	{
	    _acquired = _mutex.timedWriteLock(timeout);
	}

    private:
	WLockT(const WLockT&);			// disallow copy ctor
	WLockT& operator=(const WLockT&);	// disallow assignment operator

	const T& _mutex;
	mutable bool _acquired;
    };

    //---------------------------------------------------
    // TryLock template for writers
    template <typename T>
    class TryWLockT : public WLockT<T>
    {
    public:
	TryWLockT(const T& mutex) : WLockT<T>(mutex, true)
	{
	}

	TryWLockT(const T& mutex, const Time& timeout) :
	    WLockT<T>(mutex, timeout)
	{
	}
	
    };

}

#endif
