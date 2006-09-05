#ifndef MPS_QUEUE_H
#define MPS_QUEUE_H

/*! \file MPS/Queue.h

  \brief Generic queue implementation based on MPS::RingBuffer.

  MPS Queues may be used for both variable-size or fixed-size
  type-safe message-oriented communication between a set of producers
  and a set of consumers.  It provides a template-based mechanism to
  define the required locking semantics on both ends of the queue at
  compile time.  In addition it offers extensive configuration
  features to optimize the layout in particular applications.

  Queues allow reference semantics for ring buffers.  This is useful
  for the implementation of zero-copy communication.

  Queues encapsulate an MPS::Allocator and an MPS::RingBuffer.  The
  MPS::RingBuffer transmit pointers.  Both, the RingBuffer and the
  Allocator will use contiguous memory areas. Locking may be
  configured independently on each end.

  There are two constructors available. The first simpler one just
  allocates the RingBuffer and the Allocator contiguously. The
  internal RingBuffer holds a \c head pointing to the position where
  consumers would get a pointer to a new message and a \c tail
  pointing to the place where producers would write a pointer to a new
  message.  Both head and tail are protected with mutexes to allow
  multiple consumers and producers respectively.  If you do not
  require multiple consumers or producers you may define the
  corresponding mutex as a MPS::DummyMutex which provides a fake mutex
  behaviour.  Static composition and aggressive inlining cooperate to
  remove unneeded code at compile time.

  Both \c head and \c tail may also be configured on private memories
  if there is a single reader or a single writer respectively, but
  this would not perform as well as you would imagine because tail and
  head pointers cannot be mapped to local registers (they are declared
  volatile). Usually they will be configured on the scratch memory of
  the most frequent reader and writer respectively.

  Each end (consumer and producer) may be configured independently
  (e.g. one end may use a LocalSemaphore and another a
  RemoteSemaphore).

*/

#include <MPS/RingBuffer.h>
#include <MPS/Allocator.h>

namespace MPS {

    namespace Queue {

	/*! \brief Producer side of the Queue.
	 *
	 * A Queue is implemented with two different classes: a
	 * Producer and a Consumer.  A producer takes care of the
	 * synchronized access to the queue for writers.
	 *
	 * \param T Type for messages. You may always produce messages
	 * of any size.  You do not need to use the type-safe
	 * interface.  If type-safety is not required you may use
	 * whatever type you want (e.g. char) or use the untyped
	 * version UntypedProducer.  You may also get type-safe
	 * reference transmission even for variable-size messages if
	 * there is a common base class for all messages. T should
	 * then be declared to be that base class.
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
	 * \param MemMutex Type of mutex to protect concurrent access
	 * to the allocator.
	 *
	 * \param N Size of the Queue (in units of type \a T).  This is
	 * a template parameter because modulo operations may be
	 * highly optimized by the compiler if N is a power of two.
	 * The minimum size of a buffer for this implementation is
	 * one.
	 */
	template <class T, class Mutex, class PSemaphore, class CSemaphore, class MemMutex, int N>
	class Producer {

	    /*! \brief Type of the internal ring buffer.
	     *
	     * The internal ring buffer stores pointers to messages
	     * allocated in the internal allocator.  If type-safety is
	     * not required or desired just use \c char as the value
	     * for T template parameter.
	     */
	    typedef MPS::RingBuffer::Producer<volatile T*, Mutex, PSemaphore, CSemaphore, N> ProducerBuffer;
	    
	    /*! \brief Type of the internal allocator.
	     *
	     * The internal allocator holds at most N instances of
	     * type T.  Note that there is no need to just allocate
	     * messages of type T.
	     */
	    typedef MPS::Allocator<MemMutex, N * (sizeof(T) + sizeof(int))> Allocator;
	    
	public:

	    /*! \brief Simplified constructor for the producer side of
	     *  a Queue.
	     *
	     * This simplified constructor assumes that ring buffer
	     * and allocator are located in contiguous memory regions.
	     * This is useful for small queues or extremely large
	     * ones.  Otherwise you should probably allocate the ring
	     * buffer on the scratch memory and the allocator in the
	     * shared memory.
	     *
	     * Multiple instances of this class may refer to the same
	     * queue but you should be careful to initialize it just
	     * once (see \a init parameter).  You should also take
	     * care not to use the buffer before it is properly
	     * initialized.
	     *
	     * \param mem Storage area.
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
	     * \param memMutex Guarantees mutual exclusion when trying
	     * to access the internal allocator.
	     *
	     * \param init Whether this side of the buffer should be
	     * initialized.  This is not enough to be able to write
	     * messages in the buffer.  One end of the buffer (and
	     * only one) must invoke the \c activate method to
	     * initialize the semaphores.
	     */
	    Producer(volatile void* mem,
		     volatile int& tail,
		     Mutex& tailMutex,
		     PSemaphore& semP,
		     CSemaphore& semC,
		     MemMutex& memMutex,
		     bool init = false)
		: _buffer(static_cast<volatile T* volatile *>(mem), tail, tailMutex, semP, semC, init),
		  _allocator(static_cast<volatile T* volatile *>(mem) + N , memMutex, init)
	    {
	    }
	    
	    /*! \brief Constructor for the producer side of a Queue.
	     *
	     * This constructor allows independent definition of the
	     * internal ring buffer data area and the allocator data
	     * area.  This is useful to allocate the ring buffer on
	     * the scratch memory (very fast) and the allocator in the
	     * shared memory (larger size).
	     *
	     * Multiple instances of this class may refer to the same
	     * queue but you should be careful to initialize it just
	     * once (see \a init parameter).  You should also take
	     * care not to use the buffer before it is properly
	     * initialized.
	     *
	     * \param buf Ring buffer storage area.
	     *
	     * \param mem Storage area.
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
	     * \param memMutex Guarantees mutual exclusion when trying
	     * to access the internal allocator.
	     *
	     * \param init Whether this side of the buffer should be
	     * initialized.  This is not enough to be able to write
	     * messages in the buffer.  One end of the buffer (and
	     * only one) must invoke the \c activate method to
	     * initialize the semaphores.
	     */
	    Producer(volatile void* buf,
		     volatile void* mem,
		     volatile int& tail,
		     Mutex& tailMutex,
		     PSemaphore& semP,
		     CSemaphore& semC,
		     MemMutex& memMutex,
		     bool init = false)
		: _buffer(static_cast<volatile T* volatile *>(buf), tail, tailMutex, semP, semC, init),
		  _allocator(mem, memMutex, init)
	    {
	    }

	    /*! \brief Allocates a message of default size and type.
	     *
	     * This instance of the \c alloc method allocates a chunk
	     * of data whose size is exactly that of the \a T type.
	     * It is delegated to the internal allocator.
	     *
	     * \returns Returns a pointer to the allocated message.
	     */
	    volatile T* alloc() { return static_cast<volatile T*>(_allocator.alloc(sizeof(T))); }
	    
	    /*! \brief Allocates a message of a given size.
	     *
	     * This instance of the \c alloc method allocates a
	     * message of arbitrary size.  It is delegated to the
	     * internal allocator.
	     *
	     * \returns Returns a pointer to the allocated message.
	     */
	    void* alloc(int size) { return _allocator.alloc(size); }

	    /*! \brief Adds another message to the buffer.
	     *
	     * It writes a new message to the tail position of the
	     * buffer and updates the tail pointer.  The code does not
	     * require that thee message is allocated using the \c
	     * alloc method.  Ownership of this message is transfered
	     * to the receiver.  Therefore the receiver is responsible
	     * for freeing this message when no longer needed.
	     */
	    void  put(volatile T* data) { _buffer.put(data); }

	    /*! \brief Activates the queue.
	     *
	     * Initializes the internal ring buffer.  See
	     * MPS::RingBuffer::Producer::activate for more
	     * information.
	     */
	    void activate() { _buffer.activate(); }

	private:

	    /*! \brief Internal ring buffer for pointer transmission.
	     *
	     * This buffer is used for passing references to messages
	     * to the consumers.
	     */
	    ProducerBuffer _buffer;

	    /*! \brief Internal allocator.
	     *
	     * It can be used to allocate messages on the producer
	     * side before they are sent to the consumers.
	     */
	    Allocator _allocator;
	};

	
	/*! \brief Producer side of an untyped Queue.
	 *
	 * An untyped Queue is implemented with two different classes:
	 * an UntypedProducer and an UntypedConsumer.  An untyped
	 * producer takes care of the synchronized access to the queue
	 * for writers.
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
	 * \param MemMutex Type of mutex to protect concurrent access
	 * to the allocator.
	 *
	 * \param N Size of the Queue (maximum number of outstanding
	 * messages).  This is the size of the internal ring buffer.
	 * This is a template parameter because modulo operations may
	 * be highly optimized by the compiler if N is a power of two.
	 * The minimum size of a buffer for this implementation is
	 * one.
	 *
	 * \param M Size of the internal allocator in bytes.
	 */
	template <class Mutex, class PSemaphore, class CSemaphore, class MemMutex, int N, int M>
	class UntypedProducer {

	    /*! \brief Type of the internal ring buffer.
	     *
	     * The internal ring buffer stores untyped pointers to
	     * messages allocated in the internal allocator.
	     */
	    typedef MPS::RingBuffer::Producer<volatile void*, Mutex, PSemaphore, CSemaphore, N> ProducerBuffer;

	    /*! \brief Type of the internal allocator.
	     *
	     * The internal allocator holds at most M bytes and
	     * concurrent access is protected with a MemMutex.
	     */
	    typedef MPS::Allocator<MemMutex, M> Allocator;

	public:

	    /*! \brief Simplified constructor for the producer side of
	     *  an untyped Queue.
	     *
	     * This simplified constructor assumes that ring buffer
	     * and allocator are located in contiguous memory regions.
	     * This is useful for small queues or extremely large
	     * ones.  Otherwise you should probably allocate the ring
	     * buffer on the scratch memory and the allocator in the
	     * shared memory.
	     *
	     * Multiple instances of this class may refer to the same
	     * queue but you should be careful to initialize it just
	     * once (see \a init parameter).  You should also take
	     * care not to use the buffer before it is properly
	     * initialized.
	     *
	     * \param mem Storage area.
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
	     * \param memMutex Guarantees mutual exclusion when trying
	     * to access the internal allocator.
	     *
	     * \param init Whether this side of the buffer should be
	     * initialized.  This is not enough to be able to write
	     * messages in the buffer.  One end of the buffer (and
	     * only one) must invoke the \c activate method to
	     * initialize the semaphores.
	     */
	    UntypedProducer(volatile void* mem,
			    volatile int& tail,
			    Mutex& tailMutex,
			    PSemaphore& semP,
			    CSemaphore& semC,
			    MemMutex& memMutex,
			    bool init = false)
		: _buffer(static_cast<volatile void* volatile *>(mem), tail, tailMutex, semP, semC, init),
		  _allocator(static_cast<volatile void* volatile *>(mem) + N , memMutex)
	    {
	    }
	    
	    /*! \brief Constructor for the producer side of an untyped
	     *  Queue.
	     *
	     * This constructor allows independent definition of the
	     * internal ring buffer data area and the allocator data
	     * area.  This is useful to allocate the ring buffer on
	     * the scratch memory (very fast) and the allocator in the
	     * shared memory (larger size).
	     *
	     * Multiple instances of this class may refer to the same
	     * queue but you should be careful to initialize it just
	     * once (see \a init parameter).  You should also take
	     * care not to use the buffer before it is properly
	     * initialized.
	     *
	     * \param buf Ring buffer storage area.
	     *
	     * \param mem Storage area.
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
	     * \param memMutex Guarantees mutual exclusion when trying
	     * to access the internal allocator.
	     *
	     * \param init Whether this side of the buffer should be
	     * initialized.  This is not enough to be able to write
	     * messages in the buffer.  One end of the buffer (and
	     * only one) must invoke the \c activate method to
	     * initialize the semaphores.
	     */
	    UntypedProducer(volatile void* buf,		// RingBuffer area
			    volatile void* mem,		// Shared mem
			    volatile int& tail,		// Tail of RingBuffer
			    Mutex& tailMutex,		// Mutex for tail manipulation
			    PSemaphore& semP,		// Number of elements in buffer
			    CSemaphore& semC,		// Number of empty slots in buffer
			    MemMutex& memMutex,		// Mutex for shared mem
			    bool init = false)
		: _buffer(static_cast<volatile void* volatile *>(buf), tail, tailMutex, semP, semC, init),
		  _allocator(mem , memMutex)
	    {
	    }
	    
	    /*! \brief Allocates a message of a given size.
	     *
	     * This instance of the \c alloc method allocates a
	     * message of arbitrary size.  It is delegated to the
	     * internal allocator.
	     *
	     * \returns Returns a pointer to the allocated message.
	     */
	    void* alloc(int size) { return _allocator.alloc(size); }
	    
	    /*! \brief Adds another message to the buffer.
	     *
	     * It writes a new message to the tail position of the
	     * buffer and updates the tail pointer.  The code does not
	     * require that thee message is allocated using the \c
	     * alloc method.  Ownership of this message is transfered
	     * to the receiver.  Therefore the receiver is responsible
	     * for freeing this message when no longer needed.
	     */
	    void  put(void* data) { _buffer.put(data); }

	    /*! \brief Activates the queue.
	     *
	     * Initializes the internal ring buffer.  See
	     * MPS::RingBuffer::Producer::activate for more
	     * information.
	     */
	    void  activate() { _buffer.activate(); }

	private:

	    /*! \brief Internal ring buffer for pointer transmission.
	     *
	     * This buffer is used for passing references to messages
	     * to the consumers.
	     */
	    ProducerBuffer _buffer;

	    /*! \brief Internal allocator.
	     *
	     * It can be used to allocate messages on the producer
	     * side before they are sent to the consumers.
	     */
	    Allocator _allocator;
	};

	
	/*! \brief Consumer side of the Queue.
	 *
	 * A Queue is implemented with two different classes: a
	 * Producer and a Consumer.  A consumer takes care of the
	 * synchronized access to the queue for readers.
	 *
	 * \param T Type for messages. You may always consume messages
	 * of any size.  You do not need to use the type-safe
	 * interface.  If type-safety is not required you may use
	 * whatever type you want (e.g. char) or use the untyped
	 * version UntypedConsumer.  You may also get type-safe
	 * reference transmission even for variable-size messages if
	 * there is a common base class for all messages. T should
	 * then be declared to be that base class.
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
	 * \param MemMutex Type of mutex to protect concurrent access
	 * to the allocator.
	 *
	 * \param N Size of the Queue (in units of type \a T).  This is
	 * a template parameter because modulo operations may be
	 * highly optimized by the compiler if N is a power of two.
	 * The minimum size of a buffer for this implementation is
	 * one.
	 */
	template <class T, class Mutex, class PSemaphore, class CSemaphore, class MemMutex, int N>
	class Consumer {
	    
	    /*! \brief Type of the internal ring buffer.
	     *
	     * The internal ring buffer stores pointers to messages
	     * allocated in the internal allocator.  If type-safety is
	     * not required or desired just use \c char as the value
	     * for T template parameter.
	     */
	    typedef MPS::RingBuffer::Consumer<volatile T*, Mutex, PSemaphore, CSemaphore, N> ConsumerBuffer;

	    /*! \brief Type of the internal allocator.
	     *
	     * The internal allocator holds at most N instances of
	     * type T.  Note that there is no need to just allocate
	     * messages of type T.
	     */
	    typedef MPS::Allocator<MemMutex, N * (sizeof(T) + sizeof(int))> Allocator;

	public:

	    /*! \brief Simplified constructor for the consumer side of
	     *  a Queue.
	     *
	     * This simplified constructor assumes that ring buffer
	     * and allocator are located in contiguous memory regions.
	     * This is useful for small queues or extremely large
	     * ones.  Otherwise you should probably allocate the ring
	     * buffer on the scratch memory and the allocator in the
	     * shared memory.
	     *
	     * Multiple instances of this class may refer to the same
	     * queue but you should be careful to initialize it just
	     * once (see \a init parameter).  You should also take
	     * care not to use the buffer before it is properly
	     * initialized.
	     *
	     * \param mem Storage area.
	     *
	     * \param head Index to the position where next message
	     * will be get.  Although we frequently talk about a head
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
	     * and more efficient.  A consumer would automatically
	     * block on an empty buffer.
	     *
	     * \param semC Consumed item counter.  Number of free
	     * positions in the buffer.  It is implemented as a
	     * semaphore to make producer implementation easier and
	     * more efficient.
	     *
	     * \param memMutex Guarantees mutual exclusion when trying
	     * to access the internal allocator.
	     *
	     * \param init Whether this side of the buffer should be
	     * initialized.  This is not enough to be able to get
	     * messages from the buffer.  One end of the buffer (and
	     * only one) must invoke the \c activate method to
	     * initialize the semaphores.
	     */
	    Consumer(volatile void* mem,
		     volatile int& head,
		     Mutex& headMutex,
		     PSemaphore& semP,
		     CSemaphore& semC,
		     MemMutex& memMutex,
		     bool init = false)
		: _buffer(static_cast<volatile T* volatile *>(mem), head, headMutex, semP, semC, init),
		  _allocator(static_cast<volatile T* volatile *>(mem) + N , memMutex)
	    {
	    }
	    
	    /*! \brief Constructor for the consumer side of a Queue.
	     *
	     * This constructor allows independent definition of the
	     * internal ring buffer data area and the allocator data
	     * area.  This is useful to allocate the ring buffer on
	     * the scratch memory (very fast) and the allocator in the
	     * shared memory (larger size).
	     *
	     * Multiple instances of this class may refer to the same
	     * queue but you should be careful to initialize it just
	     * once (see \a init parameter).  You should also take
	     * care not to use the buffer before it is properly
	     * initialized.
	     *
	     * \param buf Ring buffer storage area.
	     *
	     * \param mem Storage area.
	     *
	     * \param head Index to the position where next message
	     * will be get.  Although we frequently talk about a head
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
	     * and more efficient.  A consumer would automatically
	     * block on an empty buffer.
	     *
	     * \param semC Consumed item counter.  Number of free
	     * positions in the buffer.  It is implemented as a
	     * semaphore to make producer implementation easier and
	     * more efficient.
	     *
	     * \param memMutex Guarantees mutual exclusion when trying
	     * to access the internal allocator.
	     *
	     * \param init Whether this side of the buffer should be
	     * initialized.  This is not enough to be able to write
	     * messages in the buffer.  One end of the buffer (and
	     * only one) must invoke the \c activate method to
	     * initialize the semaphores.
	     */
	    Consumer(volatile void* buf,
		     volatile void* mem,
		     volatile int& head,
		     Mutex& headMutex,
		     PSemaphore& semP,
		     CSemaphore& semC,
		     MemMutex& memMutex,
		     bool init = false)
		: _buffer(static_cast<volatile T* volatile *>(buf), head, headMutex, semP, semC, init),
		  _allocator(mem, memMutex)
	    {
	    }

	    /*! \brief Frees a previously allocated message.
	     *
	     * This method assumes that the pointer was allocated by a
	     * call to \c alloc even if it was passed through the
	     * internal ring buffer to another processor.
	     *
	     * \param addr Address previously returned by a former
	     * call to \c alloc either on the current processor or any
	     * other processor.
	     */
	    void free(volatile T* addr) { _allocator.free(addr); }
	    
	    /*! \brief Activates the queue.
	     *
	     * Initializes the internal ring buffer.  See
	     * MPS::RingBuffer::Producer::activate for more
	     * information.
	     */
	    void activate() { _buffer.activate(); }
	    
	    /*! \brief Get first available message in the queue.
	     *
	     * It reads a new message from the head position of the
	     * internal ring buffer.
	     */
	    volatile T* get() { return _buffer.get(); }

	private:
	    
	    /*! \brief Internal ring buffer for pointer reception.
	     *
	     * This buffer is used for receiving references to
	     * messages from producers.
	     */
	    ConsumerBuffer _buffer;
	    
	    /*! \brief Internal allocator.
	     *
	     * It can be used to free messages on the consumer
	     * side when they are received from producers.
	     */
	    Allocator _allocator;
	};

	
	/*! \brief Consumer side of an untyped Queue.
	 *
	 * An untyped Queue is implemented with two different classes:
	 * an UntypedProducer and an UntypedConsumer.  An untyped
	 * consumer takes care of the synchronized access to the queue
	 * for readers.
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
	 * \param MemMutex Type of mutex to protect concurrent access
	 * to the allocator.
	 *
	 * \param N Size of the Queue (maximum number of outstanding
	 * messages).  This is the size of the internal ring buffer.
	 * This is a template parameter because modulo operations may
	 * be highly optimized by the compiler if N is a power of two.
	 * The minimum size of a buffer for this implementation is
	 * one.
	 *
	 * \param M Size of the internal allocator in bytes.
	 */
	template <class Mutex, class PSemaphore, class CSemaphore, class MemMutex, int N, int M>
	class UntypedConsumer {

	    /*! \brief Type of the internal ring buffer.
	     *
	     * The internal ring buffer stores untyped pointers to
	     * messages allocated in the internal allocator.
	     */
	    typedef MPS::RingBuffer::Consumer<volatile void*, Mutex, PSemaphore, CSemaphore, N> ConsumerBuffer;

	    /*! \brief Type of the internal allocator.
	     *
	     * The internal allocator holds at most M bytes and
	     * concurrent access is protected with a MemMutex.
	     */
	    typedef MPS::Allocator<MemMutex, M> Allocator;

	public:

	    /*! \brief Simplified constructor for the consumer side of
	     *  an untyped Queue.
	     *
	     * This simplified constructor assumes that ring buffer
	     * and allocator are located in contiguous memory regions.
	     * This is useful for small queues or extremely large
	     * ones.  Otherwise you should probably allocate the ring
	     * buffer on the scratch memory and the allocator in the
	     * shared memory.
	     *
	     * Multiple instances of this class may refer to the same
	     * queue but you should be careful to initialize it just
	     * once (see \a init parameter).  You should also take
	     * care not to use the buffer before it is properly
	     * initialized.
	     *
	     * \param mem Storage area.
	     *
	     * \param head Index to the position where next message
	     * will be get.  Although we frequently talk about a head
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
	     * and more efficient.  A consumer would automatically
	     * block on an empty buffer.
	     *
	     * \param semC Consumed item counter.  Number of free
	     * positions in the buffer.  It is implemented as a
	     * semaphore to make producer implementation easier and
	     * more efficient.
	     *
	     * \param memMutex Guarantees mutual exclusion when trying
	     * to access the internal allocator.
	     *
	     * \param init Whether this side of the buffer should be
	     * initialized.  This is not enough to be able to write
	     * messages in the buffer.  One end of the buffer (and
	     * only one) must invoke the \c activate method to
	     * initialize the semaphores.
	     */
	    UntypedConsumer(volatile void* mem,
			    volatile int& head,
			    Mutex& headMutex,
			    PSemaphore& semP,
			    CSemaphore& semC,
			    MemMutex& memMutex,
			    bool init = false)
		: _buffer(static_cast<volatile void* volatile *>(mem), head, headMutex, semP, semC, init),
		  _allocator(static_cast<volatile void* volatile *>(mem) + N , memMutex)
	    {
	    }
	    
	    /*! \brief Constructor for the consumer side of an untyped
	     *  Queue.
	     *
	     * This constructor allows independent definition of the
	     * internal ring buffer data area and the allocator data
	     * area.  This is useful to allocate the ring buffer on
	     * the scratch memory (very fast) and the allocator in the
	     * shared memory (larger size).
	     *
	     * Multiple instances of this class may refer to the same
	     * queue but you should be careful to initialize it just
	     * once (see \a init parameter).  You should also take
	     * care not to use the buffer before it is properly
	     * initialized.
	     *
	     * \param buf Ring buffer storage area.
	     *
	     * \param mem Storage area.
	     *
	     * \param head Index to the position where next message
	     * will be get.  Although we frequently talk about a head
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
	     * and more efficient.  A consumer would automatically
	     * block on an empty buffer.
	     *
	     * \param semC Consumed item counter.  Number of free
	     * positions in the buffer.  It is implemented as a
	     * semaphore to make producer implementation easier and
	     * more efficient.
	     *
	     * \param memMutex Guarantees mutual exclusion when trying
	     * to access the internal allocator.
	     *
	     * \param init Whether this side of the buffer should be
	     * initialized.  This is not enough to be able to write
	     * messages in the buffer.  One end of the buffer (and
	     * only one) must invoke the \c activate method to
	     * initialize the semaphores.
	     */
	    UntypedConsumer(volatile void* buf,		// RingBuffer area
			    volatile void* mem,		// Shared mem
			    volatile int& head,		// Head of RingBuffer
			    Mutex& headMutex,		// Mutex for head manipulation
			    PSemaphore& semP,		// Number of elements in buffer
			    CSemaphore& semC,		// Number of empty slots in buffer
			    MemMutex& memMutex,		// Mutex for shared mem
			    bool init = false)
		: _buffer(static_cast<volatile void* volatile *>(buf), head, headMutex, semP, semC, init),
		  _allocator(mem, memMutex)
	    {
	    }
	    
	    /*! \brief Frees a previously allocated message.
	     *
	     * This method assumes that the pointer was allocated by a
	     * call to \c alloc even if it was passed through the
	     * internal ring buffer to another processor.
	     *
	     * \param addr Address previously returned by a former
	     * call to \c alloc either on the current processor or any
	     * other processor.
	     */
	    void free(volatile void* addr) { _allocator.free(addr); }

	    /*! \brief Activates the queue.
	     *
	     * Initializes the internal ring buffer.  See
	     * MPS::RingBuffer::Producer::activate for more
	     * information.
	     */
	    void activate() { _buffer.activate(); }

	    /*! \brief Get first available message in the queue.
	     *
	     * It reads a new message from the head position of the
	     * internal ring buffer.
	     */
	    volatile void* get() { return _buffer.get(); }

	private:
	    /*! \brief Internal ring buffer for pointer reception.
	     *
	     * This buffer is used for receiving references to
	     * messages from producers.
	     */
	    ConsumerBuffer _buffer;

	    /*! \brief Internal allocator.
	     *
	     * It can be used to free messages on the consumer
	     * side when they are received from producers.
	     */
	    Allocator _allocator;
	};
	
    }
  
}

#endif
