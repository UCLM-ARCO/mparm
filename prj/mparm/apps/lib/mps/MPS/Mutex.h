#ifndef MPS_MUTEX_H
#define MPS_MUTEX_H

/*! \file Mutex.h

  \brief Non-recursive Mutex implementation for inter-processor synchronization.

  MPS mutexes are modeled after the C++ Mutex implementation included
  in ZeroC ICE (http://www.zeroc.com/) which closely resembles the
  POSIX semantics.  Mutexes provide \c lock and an \c unlock
  operations to guarantee mututal exclusion when accessing a critical
  region or shared resource.

  MPS mutexes differ from ZeroC ICE in a number of ways:

  \li ZeroC ICE selects a single underlying implementation primitive
      for each target platform which is provided by the operating
      system.  MPS does not require any underlying OS but instead it
      uses the hardware synchronization primitives available in the
      MPARM platform.  MPS does not try to guess which particular
      synchronization primitive is best suited for the target
      application but instead provides a whole range of Mutexes based
      on every hardware synchronization primitives available.
  
  \li ZeroC ICE Mutexes provide strict POSIX semantics.  A mutex is
      constructed in a thread and then shared with any spawned thread.
      In contrast MPS mutexes may be instantiated multiple times and
      still refer to the same mutex object.  A mutex is uniquely
      identified by the parameters passed to the constructor and those
      parameters depend on the underlying implementation technique.
      The last parameter to any Mutex constructor is a boolean value
      which default to false.  If explicitly set to true then that
      specific Mutex instance would take care of proper inicialization
      of the underlying hardware to a known unlocked state.  A
      fool-proof application should ensure that a single instance
      initializing the underlying hardware is created prior to any
      other non-initializing Mutex instances refering to the same
      object.

  \li ZeroC ICE Mutexes enforce, as POSIX mutexes also do, a simple
      set of rules to guarantee the right behaviour: the same thread
      which locks a mutex is also responsible for releasing it, and a
      thread which did not lock the mutex is not allowed to release
      it.  By contrast, MPS mutexes do not impose any constraints on
      the sequencing of lock and unlock operations from different
      threads or processors.  This means, for example, that an MPS
      mutex may be used as a binary semaphore to signal a boolean
      condition to another processor.  You should think twice before
      doing so though.  MPS API is deliberately similar to ZeroC ICE
      API and to POSIX API in order to ease the translation of
      standard multi-threaded applications to the MPARM platform and
      viceversa.  If you don't stick to the POSIX rules you will loose
      the ability to debug or prototype your application on a POSIX
      environment.  Sticking to POSIX rules through the use of \ref
      MPS::Lock instances also makes software more robust when
      exceptional conditions arise.

  MPARM shared memory semaphores have different semantics than
  scratchpad memories semaphores.  Hence we need to provide two set of
  implementations (MPS::ShMem::Mutex, and MPS::ScrMem::Mutex).  Please
  either consult the MPSIM documentation or examine \c core/ext_mem.h
  and \c core/scratch_mem.h in the simulator source code for
  information on the precise semantics of the underlying hardware
  support.

*/

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
