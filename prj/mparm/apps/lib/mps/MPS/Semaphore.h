#ifndef MPS_SEMAPHORE_H
#define MPS_SEMAPHORE_H

/*! \file MPS/Semaphore.h

  \brief Semaphore implementation for inter-processor synchronization.

  MPS semaphores implement standard Dikjstra semaphore API for
  counting semaphores.  The naming convention is taken from ZeroC ICE
  semaphores which are modeled after the POSIX semaphores.  They
  provide a \c post operation that increments the semaphore counter
  and a \c wait operation that decrements the counter.  A \c wait on a
  zero valued semaphore would block until it gets a non-zero value.
  
  MPARM shared memory semaphores have different semantics than
  scratchpad memories semaphores.  Hence we need to provide two set of
  implementations (MPS::ShMem::Semaphore, and MPS::ScrMem::Semaphore).
  Please either consult the MPSIM documentation or examine \c
  core/ext_mem.h and \c core/scratch_mem.h in the simulator source
  code for information on the precise semantics of the underlying
  hardware support.

  Semaphores are too primitive for most applications.  Deadlocks and
  race conditions are easy to get and hard to find.  You should avoid
  them if possible.  Look at Mutex, RecMutex, Cond, Monitor for higher
  level synchronization abstractions, and refer to the documentation
  for hints on proper usage.
  
*/

# include <MPS/Config.h>
# include <IceUtil/Time.h>

namespace MPS {

    // Share Time implementation with IceUtil
    typedef IceUtil::Time Time;
    
    /*! \brief Common behaviour for all semaphores.
     *
     * A major design principle of MPS is writing things once.  In
     * order to minimize run-time overhead of all Semaphore
     * implementations we define a common template class which
     * encapsulates the common behaviour of all Semaphores.  A
     * discussion on this static composition technique can be found in
     * section \ref staticcompo.
     */
    template <class SemaphoreState>
    class BaseSemaphore
    {
    public:
	
	/*! \brief Constructor for all semaphore variants.
	 *
	 * This constructor is used by all semaphore variants to
	 * initialize a reference to a state object.  Every state
	 * management details are delegated to the state object.
	 * Actually this constructor should be private and all state
	 * classes should be declared friends.  But then you would be
	 * required to modify this file when extending MPS with
	 * another semaphore variant.
	 *
	 * \param state A reference to a state class.  A state class
	 * is used to delegate technology-specific details.  More
	 * details on this technique are described in section \ref
	 * staticcompo.
	 */
	BaseSemaphore(SemaphoreState& state) : _state(state)
	{
	}
	    
	/*! \brief Initializes the semaphore to a known value.
	 *
	 * The semaphore is set to an initial value.  If the value is
	 * non-zero then processes blocked on a \c wait will be
	 * signalled.
	 *
	 * \param value Integer value for the semaphore.  There is
	 * nothing wrong with negative numbers.
	 */
	void init(int value) const
	{
	    _state.initialize(value);
	}

	/*! \brief Decrements the semaphore counter (blocking).
	 *
	 * If the semaphore value is zero then it blocks until it
	 * becomes non-zero.  Otherwise it decrements the semaphore
	 * value.
	 */
	void wait() const
	{
	    for(;;) {
		if (_state.dec())
		    break;
		_state.yield();
	    }
	}
	
	/*! \brief Decrements the semaphore counter (non-blocking).
	 *
	 * If the semaphore value is zero then it waits until it
	 * becomes non-zero or reaches a time limit.  Otherwise it
	 * decrements the semaphore value.
	 *
	 * \param t Time limit.  If system time reaches this value
	 * before the semaphore can be decremented this method will
	 * return \c false.
	 *
	 * \returns Returns \c true on success and \c false on
	 * timeout.
	 */
	bool timedWait(const Time& t) const
	{
	    Time begin = Time::now();
	    for(;;) {
		if (_state.dec())
		    return true;
		_state.yield();
		    
		if (Time::now() - begin > t)
		    return false;		// timeout
	    }
	}

	/*! \brief Increments the semaphore counter.
	 *
	 * Increments the value of the semaphore by a certain amount.
	 *
	 * \param inc Amount by which the value should be incremented.
	 * This operation is not guaranteed to be atomic.  It may be
	 * implemented as a sequence of atomic increments.
	 *
	 */
	void post(int inc = 1) const
	{
	    _state.inc(inc);
	}

    private:

	/*! \brief Disallows the copy constructor.
	 *
	 * The semantics of copying a semaphore is undefined.  To
	 * avoid automatic instantiation of a compiler-provided copy
	 * constructor we declare it private here.
	 */
	BaseSemaphore(const BaseSemaphore&);
	
	/*! \brief Disallows the assignment operator.
	 *
	 * The semantics of assigning a semaphore is undefined.  To
	 * avoid automatic instantiation of a compiler-provided
	 * assignment operator we declare it private here.
	 */
	void operator=(const BaseSemaphore&);

    private:
	
	/*! \brief Reference to a specific delegate for
	 *  technology-specific details.
	 *
	 * A reference to an object implementing the minimum set of
	 * technology-specific details.  For further details on this
	 * static composition technique see section \ref staticcompo.
	 */
	SemaphoreState& _state;
    };
    
}

# include <MPS/ShMem/Semaphore.h>
# include <MPS/ScrMem/Semaphore.h>

#endif
