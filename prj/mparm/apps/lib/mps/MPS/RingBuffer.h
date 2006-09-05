#ifndef MPS_RING_BUFFER_H
#define MPS_RING_BUFFER_H

/*! \file MPS/RingBuffer.h

  \brief Generic ring buffer implementation.

  RingBuffers may be used for fixed-size type-safe message-oriented
  communication between a set of producers and a set of consumers.  It
  provides a template-based mechanism to define the required locking
  semantics on both ends of the buffer at compile time.  In addition
  it offers extensive configuration features to optimize the layout in
  particular applications.

  A RingBuffer holds a \c head pointing to the position where
  consumers would get a new message and a \c tail pointing to the
  place where producers would write a new message.  Both pointers are
  protected with mutexes to allow multiple consumers and producers
  respectively.  If you do not require multiple consumers or producers
  you may define the corresponding mutex as a MPS::DummyMutex which
  provides a fake mutex behaviour.  Static composition and aggressive
  inlining cooperate to remove unneeded code at compile time.

  Both \c head and \c tail may also be configured on private memories
  if there is a single reader or a single writer respectively, but
  this would not perform as well as you would expect because tail and
  head pointers cannot be mapped to local registers (they are declared
  volatile). Usually they will be configured on the scratch memory of
  the most frequent reader and writer respectively.

  Each end (consumer and producer) may be configured independently
  (e.g. one end may use a LocalSemaphore and another a
  RemoteSemaphore).

*/

/*! \todo We may use partial template specialization to allow
  pointers in local registers when no locking is needed. */

namespace MPS {

    namespace RingBuffer {

	/*! \brief Producer side of the RingBuffer.
	 *
	 * A RingBuffer is implemented with two different classes: a
	 * Producer and a Consumer.  A producer takes care of the
	 * synchronized access to the buffer for writers.
	 *
	 * \param T Type for messages. If you want variable sized
	 * messages then you should look into MPS::Queue.
	 *
	 * \param Mutex Type of mutex to protect concurrent access to
	 * the producer side (to the tail pointer).
	 *
	 * \param PSemaphore Type of semaphore used to keep the
	 * produced item counter.
	 *
	 * \param CSemaphore Type of semaphore used to keep the
	 * available positions in the buffer (consumed item counter).
	 *
	 * \param N Size of the RingBuffer (in messages).  This is a
	 * template parameter because modulo operations may be highly
	 * optimized by the compiler if N is a power of two.  The
	 * minimum size of a buffer for this implementation is one.
	 */
	template <class T, class Mutex, class PSemaphore, class CSemaphore, int N>
	class Producer {
	public:

	    /*! \brief Constructor for the producer side of the
	     *  RingBuffer.
	     *
	     * Multiple instances of this class may refer to the same
	     * buffer but you should be careful to initialize it just
	     * once (see \a init parameter).  You should also take
	     * care not to use the buffer before it is properly
	     * initialized.
	     *
	     * \param data Storage area.
	     *
	     * \param tail Index to the position where next message
	     * will be put.  Although we frequently talk about a tail
	     * pointer it is actually implemented as an array index.
	     * This is also convenient for sharing the tail pointer
	     * with other processors which map the storage area on
	     * different addresses.
	     *
	     * \param tailMutex Guarantees mutual exclusion when
	     * trying to access the \a tail pointer.
	     *
	     * \param semP Produced item counter.  It is implemented
	     * as a semaphore to make consumer implementation easier
	     * and more efficient.
	     *
	     * \param semC Consumed item counter.  Number of free
	     * positions in the buffer.  It is implemented as a
	     * semaphore to make producer implementation easier and
	     * more efficient.  A producer would automatically block
	     * on a full buffer.
	     *
	     * \param init Whether this side of the buffer should be
	     * initialized.  This is not enough to be able to write
	     * messages in the buffer.  One end of the buffer (and
	     * only one) must invoke the \c activate method to
	     * initialize the semaphores.
	     */
	    Producer(volatile T* data,
		     volatile int& tail,
		     Mutex& tailMutex,
		     PSemaphore& semP,
		     CSemaphore& semC,
		     bool init = false)
		: _data(data),
		  _tail(tail), _tailMutex(tailMutex),
		  _semP(semP), _semC(semC)
	    {
		if (init)
		    _tail = 0;
	    }

	    /*! \brief Activates the buffer.
	     *
	     * Initializes the semaphores to an initial (buffer empty)
	     * condition.  We set \c _semP to zero and _semC to the
	     * maximum value.
	     */
	    void activate() {
		_semC.init(N);
		_semP.init(0);
		_tail = 0;
	    }
	    
	    /*! \brief Adds another message to the buffer.
	     *
	     * It writes a new message to the tail position of the
	     * buffer and updates the tail pointer.
	     */
	    void put(const T& t)
	    {
		_semC.wait();
		typename Mutex::Lock lock(_tailMutex);

		int tail = _tail;
		_tail = (tail + 1) % N;
		const_cast<T&>(_data[tail]) = t;
		_semP.post();
	    }
    
	private:
	    
	    /*! \brief Data storage area.
	     *
	     * Pointer to message area.  It should be large enough to
	     * hold \c N items of type \c T.  It may be located on any
	     * addressable memory.
	     */
	    volatile T* _data;
	    
	    /*! \brief Index where next message will be put.
	     *
	     * Although we frequently talk about a tail pointer it is
	     * actually implemented as an array index into the \c
	     * _data storage area.  This is also convenient for
	     * sharing the tail pointer with other processors which
	     * map the storage area on different addresses.
	     */
	    volatile int& _tail;

	    /*! \brief Guarantees mutual exclusion when trying to
	     * access the _tail pointer.
	     *
	     * If you do not require a mutex (e.g. just a single
	     * producer) then you may declare \c Mutex to be \c
	     * MPS::DummyMutex).
	     */
	    Mutex& _tailMutex;
	    
	    /*! \brief Produced item counter.
	     *
	     * It is implemented as a semaphore to make consumer
	     * implementation easier and more efficient.
	     */
	    PSemaphore& _semP;

	    /*! \brief Consumed item counter.
	     *
	     * Number of free positions in the buffer.  It is
	     * implemented as a semaphore to make producer
	     * implementation easier and more efficient.  A producer
	     * would automatically block on a full buffer.
	     */
	    CSemaphore& _semC;
	};


	/*! \brief Consumer side of the RingBuffer.
	 *
	 * A RingBuffer is implemented with two different classes: a
	 * Producer and a Consumer.  A Consumer takes care of the
	 * synchronized access to the buffer for readers.
	 *
	 * \param T Type for messages. If you want variable sized
	 * messages then you should look into MPS::Queue.
	 *
	 * \param Mutex Type of mutex to protect concurrent access to
	 * the consumer side (to the head pointer).
	 *
	 * \param PSemaphore Type of semaphore used to keep the
	 * produced item counter.
	 *
	 * \param CSemaphore Type of semaphore used to keep the
	 * available positions in the buffer (consumed item counter).
	 *
	 * \param N Size of the RingBuffer (in messages).  This is a
	 * template parameter because modulo operations may be highly
	 * optimized by the compiler if N is a power of two.  The
	 * minimum size of a buffer for this implementation is one.
	 */
	template <class T, class Mutex, class PSemaphore, class CSemaphore, int N>
	class Consumer {
	public:

	    /*! \brief Constructor for the consumer side of the
	     *  RingBuffer.
	     *
	     * Multiple instances of this class may refer to the same
	     * buffer but you should be careful to initialize it just
	     * once (see \a init parameter).  You should also take
	     * care not to use the buffer before it is properly
	     * initialized.
	     *
	     * \param data Storage area.
	     *
	     * \param head Index to the position of first available
	     * message.  Although we frequently talk about a head
	     * pointer it is actually implemented as an array index.
	     * This is also convenient for sharing the head pointer
	     * with other processors which map the storage area on
	     * different addresses.
	     *
	     * \param headMutex Guarantees mutual exclusion when
	     * trying to access the \a head pointer.
	     *
	     * \param semP Produced item counter.  It is implemented
	     * as a semaphore to make consumer implementation easier
	     * and more efficient.
	     *
	     * \param semC Consumed item counter.  Number of free
	     * positions in the buffer.  It is implemented as a
	     * semaphore to make producer implementation easier and
	     * more efficient.  A producer would automatically block
	     * on a full buffer.
	     *
	     * \param init Whether this side of the buffer should be
	     * initialized.  This is not enough to be able to write
	     * messages in the buffer.  One end of the buffer (and
	     * only one) must invoke the \c activate method to
	     * initialize the semaphores.
	     */
	    Consumer(volatile T* data,
		     volatile int& head,
		     Mutex& headMutex,
		     PSemaphore& semP,
		     CSemaphore& semC,
		     bool init = false)
		: _data(data), _head(head), _headMutex(headMutex), _semP(semP), _semC(semC)
	    {
		if (init)
		    _head = 0;
	    }

	    /*! \brief Inhibit further puts.
	     *
	     * The value of the consumed item counter is set to zero.
	     * Therefore all producers would block on a put to this
	     * buffer.
	     */
	    void deactivate() {
		_semC.init(0);
	    }

	    /*! \brief Activates the buffer.
	     *
	     * Initializes the semaphores to an initial (buffer empty)
	     * condition.  We set \c _semP to zero and _semC to the
	     * maximum value.
	     */
	    void activate() {
		_semC.init(N);
		_semP.init(0);
		_head = 0;
	    }

	private:

	    /*! \brief Post a semaphore when destructor is invoked.
	     *
	     * This intermediate class wraps a semaphore post in order
	     * to allow post-return invocation of the \c post
	     * semaphore method.  This is needed in the \c get method
	     * implementation.
	     */
	    class PostOnDestructor {
		CSemaphore& _sem;
	    public:
		PostOnDestructor(CSemaphore& sem): _sem(sem) {}
		~PostOnDestructor() { _sem.post(); }
	    };
	    
	public:
	    
	    /*! \brief Get first available message in the buffer.
	     *
	     * It reads a new message from the head position of the
	     * buffer and updates the head pointer.
	     */
	    T get()
	    {
		_semP.wait();
		typename Mutex::Lock lock(_headMutex);

		int head = _head;
		_head = (head + 1) % N;
		PostOnDestructor consume(_semC);
		return const_cast<const T&>(_data[head]);
	    }
        
	private:

	    /*! \brief Data storage area.
	     *
	     * Pointer to message area.  It should be large enough to
	     * hold \c N items of type \c T.  It may be located on any
	     * addressable memory.
	     */
	    volatile T* _data;
	    
	    /*! \brief Index where first available message is located.
	     *
	     * Although we frequently talk about a head pointer it is
	     * actually implemented as an array index into the \c
	     * _data storage area.  This is also convenient for
	     * sharing the head pointer with other processors which
	     * map the storage area on different addresses.
	     */
	    volatile int& _head;

	    /*! \brief Guarantees mutual exclusion when trying to
	     * access the _head pointer.
	     *
	     * If you do not require a mutex (e.g. just a single
	     * consumer) then you may declare \c Mutex to be \c
	     * MPS::DummyMutex).
	     */
	    Mutex& _headMutex;

	    /*! \brief Produced item counter.
	     *
	     * It is implemented as a semaphore to make consumer
	     * implementation easier and more efficient.
	     */
	    PSemaphore& _semP;

	    /*! \brief Consumed item counter.
	     *
	     * Number of free positions in the buffer.  It is
	     * implemented as a semaphore to make producer
	     * implementation easier and more efficient.  A producer
	     * would automatically block on a full buffer.
	     */
	    CSemaphore& _semC;
	};
	
    }    

}
#endif
