#ifndef MPS_REC_MUTEX_H
#define MPS_REC_MUTEX_H

/*! \file MPS/RecMutex.h

  \brief Recursive Mutex implementation for inter-processor
  synchronization.

  Recursive Mutexes provide \c lock and an \c unlock operations to
  guarantee mututal exclusion when accessing a critical region or
  shared resource.  In contrast to simple mutexes, recursive mutexes
  allow multiple lock operations from a single process and requires
  the same number of unlock operations to become unlocked.

  MPS mutexes are modeled after the C++ Mutex implementation included
  in ZeroC ICE (http://www.zeroc.com/) which closely resembles the
  POSIX semantics.  See documentation of file Mutex.h for a detailed
  description of the differences between MPS and ZeroC ICE mutexes.
  Besides, MPS recursive mutexes are not thread-safe.  If you want to
  synchronize multiple threads just make a new instance of the mutex
  on each thread.
  
  MPARM shared memory semaphores have different semantics than
  scratchpad memories semaphores.  Hence we need to provide two set of
  implementations (MPS::ShMem::RecMutex, and MPS::ScrMem::RecMutex).
  Please either consult the MPSIM documentation or examine \c
  core/ext_mem.h and \c core/scratch_mem.h in the simulator source
  code for information on the precise semantics of the underlying
  hardware support.
*/

#include <MPS/Mutex.h>

namespace MPS {

    /*! \brief Common behaviour for all recursive mutexes.
     *
     * A major design principle of MPS is writing things once.  In
     * order to minimize run-time overhead of all RecMutex
     * implementations we define a common template class which
     * encapsulates the common behaviour of all RecMutexes.  A
     * discussion on this static composition technique can be found in
     * section \ref staticcompo.
     */
    template <class MutexState>
    class BaseRecMutex
    {
	/*! \brief Condition variables require special behaviour.
	 *
	 * There are several flavours of condition variables depending
	 * on the mutex and the semaphore used in their
	 * implementation.  In the implementation we require the
	 * ability to completely unlock a recursive mutex and then
	 * restore their previous lock status.  To make implementation
	 * of condition variables completely general we provide them
	 * with a few extra private RecMutex methods.
	 */
	friend class Cond;

    public:

	/*! \brief Simplified interface for recursive mutexes.
	 *
	 * All instances of a recursive mutex instantiate the LockT
	 * template in order to make less error-prone recursive mutex
	 * usage (e.g. it provides exception safety).  See
	 * documentation of MPS::LockT for further information.
	 */
	typedef LockT<BaseRecMutex<MutexState> > Lock;
	
	/*! \brief Simplified non-blocking interface for recursive
	 *  mutexes.
	 *
	 * All instances of a recursive mutex instantiate the TryLockT
	 * template in order to make less error-prone recursive mutex
	 * usage (e.g. it provides exception safety).  See
	 * documentation of MPS::TryLockT for further information.
	 */
	typedef TryLockT<BaseRecMutex<MutexState> > TryLock;

	/*! \brief Constructor for all recursive mutex variants.
	 *
	 * This constructor is used by all recursive mutex variants to
	 * initialize a reference to a state object and the recursion
	 * depth.  Every state management details are delegated to the
	 * state object.  Actually this constructor should be private
	 * and all state classes should be declared friends.  But then
	 * you would be required to modify this file when extending
	 * MPS with another mutex variant.
	 *
	 * \param state A reference to a state class.  A state class
	 * is used to delegate technology-specific details.  More
	 * details on this technique are described in section \ref
	 * staticcompo.
	 */
	BaseRecMutex(MutexState& state)
	    : _state(state), _count(0)
	{
	}
	
	/*! \brief Locks the mutex.
	 *
	 * It the same instance of the mutex is already locked it just
	 * increments the recursion count.  If the mutex was locked
	 * from another instance then it blocks until it is unlocked.
	 * Guarantees mutual exclusion (hence the name).  It always
	 * checks for a per-instance recursion counter first and then
	 * itenters a loop of \c tryLock operations until it succeeds.
	 * If the mutex is locked then it tries to release the CPU
	 * using the \c yield method.  For active-waiting mutexes
	 * yield should be defined as an empty method.
	 */
	void lock() const {
	    if (_count)
		++_count;
	    else {
		while(!_state.tryLock())
		    _state.yield();
		_count = 1;
	    }
	}

	/*! \brief Tries to lock the mutex.
	 *
	 * Non-blocking version of the lock operation.
	 *
	 * \returns If the mutex is already locked it returns \c
	 * false.  Otherwise it locks the mutex and returns \c true.
	 */
	bool tryLock() const {
	    if (_state.tryLock()) {
		_count = 1;
		return true;	// lock acquired
	    }
	    else
		return false;	// already locked
	}

	/*! \brief Decrements the recursion counter.
	 *
	 * If the internal recursion counter goes to zero then the
	 * mutex is unlocked.  It assumes that the mutex is already
	 * locked.  The behaviour of this method on an already
	 * unlocked mutex is undefined.
	 */
	void unlock() const {
	    if (!--_count)
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
	 * otherwise.  Note that recursive mutexes will return true if
	 * the recursion counter is exactly one.
	 */
	bool willUnlock() const {
	    return _count == 1;
	}

    private:

	/*! \brief Disallows the copy constructor.
	 *
	 * The semantics of copying a mutex is undefined.  To avoid
	 * automatic instantiation of a compiler-provided copy
	 * constructor we declare it private here.
	 */
	BaseRecMutex(const BaseRecMutex&);
	
	/*! \brief Disallows the assignment operator.
	 *
	 * The semantics of assigning a mutex is undefined.  To avoid
	 * automatic instantiation of a compiler-provided assignment
	 * operator we declare it private here.
	 */
	void operator=(const BaseRecMutex&);

	/*! \brief Class to identify special lock and unlock
	 *  operations.
	 *
	 * Condition variables require special lock and unlock
	 * operations.  They are identified by an extra argument of
	 * this class that is able to keep track of the mutex status.
	 * Recursive mutexes just need to store the recursion count.
	 */
	struct LockState {
	    int count;
	};

	/*! \brief Special unlock operation for condition variables.
	 *
	 * It assumes that the mutex is already locked.  The results
	 * of invoking this method on an already unlocked mutex is
	 * undefined.
	 *
	 * This method is not public.  It is meant to be used by
	 * condition variables.  This special version of unlock stores
	 * the full state of the mutex in the structure referenced by
	 * the first argument.
	 *
	 * \param state A reference to a structure able to store the
	 * state of the mutex.
	 */
	void unlock(LockState& state) const {
	    state.count = _count;
	    _count = 0;
	    _state.unlock();
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
	 * referenced by the first argument.
	 *
	 * \param state A reference to a structure holding the
	 * previous lock state of the mutex.
	 */
	void lock(LockState& state) const {
	    while(!_state.tryLock())
		_state.yield();
	    _count = state.count;
	}

	/*! \brief Reference to a specific delegate for
	 *  technology-specific details.
	 *
	 * A reference to an object implementing the minimum set of
	 * technology-specific details.  For further details on this
	 * static composition technique see section \ref staticcompo.
	 */
	MutexState& _state;
	
	/*! \brief Recursion counter.
	 *
	 * Each instance of a recursive mutex will hold a private
	 * counter.  This counter should never be shared.  Therefore
	 * you should never share the same instance of a recursive
	 * mutex between two threads.
	 */
	mutable int _count;
    };
}

# include <MPS/ShMem/RecMutex.h>
# include <MPS/ScrMem/RecMutex.h>

#endif
