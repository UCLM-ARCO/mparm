#ifndef MPS_MUTEX_H
#define MPS_MUTEX_H

/*! \file MPS/Mutex.h

  \brief Non-recursive Mutex implementation for inter-processor
  synchronization.

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

    class Cond;

    /*! \brief Common behaviour for all mutexes.
     *
     * A major design principle of MPS is writing things once.  In
     * order to minimize run-time overhead of all Mutex
     * implementations we define a common template class which
     * encapsulates the common behaviour of all Mutexes.  A discussion
     * on this static composition technique can be found in section
     * \ref staticcompo.
     */
    template <class MutexState>
    class BaseMutex
    {
	/*! \brief Condition variables require special behaviour.
	 *
	 * There are several flavours of condition variables depending
	 * on the mutex and the semaphore used in their
	 * implementation.  In the implementation we require the
	 * ability to completely unlock a recursive mutex and then
	 * restore their previous lock status.  To make implementation
	 * of condition variables completely general we provide them
	 * with a few extra private RecMutex methods.  Non-recursive
	 * mutexes provide trivial implementations of these methods in
	 * order to be able to share the same condition variable
	 * implementation.
	 */
	friend class Cond;

    public:

	/*! \brief Simplified interface for mutexes.
	 *
	 * All instances of a mutex instantiate the LockT template in
	 * order to make less error-prone mutex usage (e.g. it
	 * provides exception safety).  See documentation of
	 * MPS::LockT for further information.
	 */
	typedef LockT<BaseMutex<MutexState> > Lock;
	
	/*! \brief Simplified non-blocking interface for mutexes.
	 *
	 * All instances of a mutex instantiate the TryLockT template
	 * in order to make less error-prone mutex usage (e.g. it
	 * provides exception safety).  See documentation of
	 * MPS::TryLockT for further information.
	 */
	typedef TryLockT<BaseMutex<MutexState> > TryLock;

	/*! \brief Constructor for all mutex variants.
	 *
	 * This constructor is used by all mutex variants to
	 * initialize a reference to a state object.  Every state
	 * management details are delegated to the state object.
	 * Actually this constructor should be private and all state
	 * classes should be declared friends.  But then you would be
	 * required to modify this file when extending MPS with
	 * another mutex variant.
	 *
	 * \param state A reference to a state class.  A state class
	 * is used to delegate technology-specific details.  More
	 * details on this technique are described in section \ref
	 * staticcompo.
	 */
	BaseMutex(MutexState& state) : _state(state)
	{
	}

	/*! \brief Locks the mutex.
	 *
	 * It the mutex is already locked it blocks until the \c
	 * unlock method is invoked.  Guarantees mutual exclusion
	 * (hence the name).  It is implemented as loop of \c tryLock
	 * operations until it succeeds.  If the mutex is locked then
	 * it tries to release the CPU using the \c yield method.  For
	 * active-waiting mutexes yield is defined as an empty method.
	 */
	void lock() const
	{
	    while(!_state.tryLock())
		_state.yield();
	}

	/*! \brief Tries to lock the mutex.
	 *
	 * Non-blocking version of the lock operation.
	 *
	 * \returns If the mutex is already locked it returns \c
	 * false.  Otherwise it locks the mutex and returns \c true.
	 */
	bool tryLock() const
	{
	    return _state.tryLock();
	}

	/*! \brief Unlocks the mutex.
	 *
	 * It assumes that the mutex is already locked.  The behaviour
	 * of this method on an already unlocked mutex is undefined.
	 */
	void unlock() const
	{
	    _state.unlock();
	}

	/*! \brief Returns whether a single unlock would unlock the mutex.
	 *
	 * It assumes that the mutex is already locked.  The result of
	 * invoking this method on an already unlocked mutex is
	 * undefined.
	 *
	 * \returns This method returns \c true if an invocation of
	 * the \c unlock method would unlock the mutex and \c false
	 * otherwise.  Note that non-recursive mutexes will always
	 * return true.
	 */
	bool willUnlock() const
	{
	    return _state.willUnlock();
	}

    private:

	/*! \brief Disallows the copy constructor.
	 *
	 * The semantics of copying a mutex is undefined.  To avoid
	 * automatic instantiation of a compiler-provided copy
	 * constructor we declare it private here.
	 */
	BaseMutex(const BaseMutex&);

	/*! \brief Disallows the assignment operator.
	 *
	 * The semantics of assigning a mutex is undefined.  To avoid
	 * automatic instantiation of a compiler-provided assignment
	 * operator we declare it private here.
	 */
	void operator=(const BaseMutex&);

	/*! \brief Dummy class to identify special lock and unlock
	 *  operations.
	 *
	 * Condition variables require special lock and unlock
	 * operations.  They are identified by an extra argument of
	 * this class that is able to keep track of the mutex status.
	 * Non-recursive mutexes do not need to store anything.
	 */
	struct LockState {};
	    
	/*! \brief Special unlock operation for condition variables.
	 *
	 * It assumes that the mutex is already locked.  The results
	 * of invoking this method on an already unlocked mutex is
	 * undefined.
	 *
	 * This method is not public.  It is meant to be used by
	 * condition variables.  This special version of unlock stores
	 * the full state of the mutex in the structure referenced by
	 * the first argument.  Note that a non-recursive mutex does
	 * not need to store anything but we follow the traits imposed
	 * by our condition variable implementation (see file \ref
	 * Cond.h).
	 *
	 * \param state A reference to a structure able to store the
	 * state of the mutex.  Note that for non-recursive mutexes it
	 * is just a dummy variable.
	 */
	void unlock(LockState& state) const
	{
	    unlock();
	}
	
	/*! \brief Special lock operation for condition variables.
	 *
	 * It assumes that the mutex is already unlocked and the
	 * argument is the result of invoking the unlock(LockState&)
	 * operation.  The results of invoking this method on an
	 * already locked mutex or passing a not-initialized state
	 * variable are undefined.
	 *
	 * This method is not public.  It is meant to be used by
	 * condition variables.  This special version of unlock
	 * restore the full state of the mutex stored in the structure
	 * referenced by the first argument.  Note that a
	 * non-recursive mutex does not need to store anything but we
	 * follow the traits imposed by our condition variable
	 * implementation (see file \ref Cond.h).
	 *
	 * \param state A reference to a structure holding the
	 * previous lock state of the mutex.  Note that for
	 * non-recursive mutexes it is just a dummy variable.
	 */
	void lock(LockState& state) const
	{
	    lock();
	}

    private:

	/*! \brief Reference to a specific delegate for
	 *  technology-specific details.
	 *
	 * A reference to an object implementing the minimum set of
	 * technology-specific details.  For further details on this
	 * static composition technique see section \ref staticcompo.
	 */
	MutexState& _state;
    };

    /*! \brief State class for fake mutexes.
     *
     * Dummy mutexes are fake locks.  They are used on containers to
     * avoid having a specialized version without locking.
     */
    class DummyMutexState
    {
    public:

	/*! \brief Implementation details of BaseMutex<T>::tryLock.
	 *
	 * See BaseMutex<T>::tryLock for more information.  A
	 * DummyMutex always success because it does not actually lock
	 * anything.
	 */
	bool tryLock() const { return true; }
	
	/*! \brief Implementation details of BaseMutex<T>::unlock.
	 *
	 * See BaseMutex<T>::unlock for more information.  A
	 * DummyMutex does not neet to unlock since it does not
	 * actually lock anything.
	 */
	void unlock() const { }

	/*! \brief Implementation details of BaseMutex<T>::willUnlock.
	 *
	 * See BaseMutex<T>::willUnlock for more information.  A
	 * DummyMutex always success because it does not actually lock
	 * anything.
	 */
	bool willUnlock() const { return true; }

	/*! \brief Yields the CPU.
	 *
	 * See BaseMutex<T>::lock for more information.  A DummyMutex
	 * does not need to actually yield the CPU since it will
	 * always success on \c lock invocations.
	 */
	void yield() const { }
    };

    /*! \brief Base class for dummy mutexes.
     *
     * Whenever you define a new mutex you should start by declaring
     * an instance of BaseMutex template to become the base class for
     * your mutex.
     */
    typedef MPS::BaseMutex<DummyMutexState> BaseDummyMutex;

    /*! \brief Actual definition of dummy mutexes.
     *
     * Usually a specific mutex just provides convenience constructors
     * and delegate everything to the parent class.
     */
    class DummyMutex : public BaseDummyMutex
    {
    public:

	/*! \brief Specific constructor for dummy mutexes.
	 *
	 * A dummy mutex does not need any storage, but you need to
	 * use the BaseMutex<T> constructor anyway.  Therefore there
	 * must be an instance of a state class as a private member to
	 * be passed to the parent constructor.
	 */
	DummyMutex() : BaseDummyMutex(_state)
	{
	}

    private:
	/*! \brief Disallows the copy constructor.
	 *
	 * The semantics of copying a mutex is undefined.  To avoid
	 * automatic instantiation of a compiler-provided copy
	 * constructor we declare it private here.
	 */
	DummyMutex(const DummyMutex&);

	/*! \brief Disallows the assignment operator.
	 *
	 * The semantics of assigning a mutex is undefined.  To avoid
	 * automatic instantiation of a compiler-provided assignment
	 * operator we declare it private here.
	 */
	void operator=(const DummyMutex&);

	/*! \brief State class instance for the dummy mutex.
	 *
	 * A state class holds all the required state for this
	 * specific mutex.  Even if there is nothing to store you must
	 * define it.  See section \ref staticcompo for more details
	 * on this strategy of static composition.
	 *
	 * Note that in this case the compiler optimizations will
	 * completely remove the state member and all of the function
	 * member invocations.  This is due to the inexistence of a
	 * virtual table and to the aggressive inlining.
	 */
	DummyMutexState _state;
    };

}

# include <MPS/ShMem/Mutex.h>
# include <MPS/ScrMem/Mutex.h>

#endif
