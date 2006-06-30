#ifndef MPS_QUEUE_H
#define MPS_QUEUE_H

// Queues allow reference semantics for RingBuffers.  This is useful
// for the implementation of zero-copy communication.

// Queues encapsulate an Allocator and a RingBuffer.  The RingBuffer
// does transmit pointers.

// Both, RingBuffer and Allocator will use contiguous memory
// areas. Locking may be configured independently on each end.

namespace MPS {

    namespace Queue {

	template <class T, class Mutex, class Semaphore, class MemMutex, int N>
	class Producer {
	    typedef MPS::RingBuffer::Producer<T*, Mutex, Semaphore, N> ProducerBuffer;
	    typedef MPS::Allocator<MemMutex, N * sizeof(T)> Allocator;
	    
	public:
	    Producer(void* mem,			// Shared mem
		     volatile unsigned& tail,	// Tail of RingBuffer
		     Mutex& tailMutex,		// Mutex for tail manipulation
		     Semaphore& sem,		// Number of elements in buffer
		     MemMutex& memMutex,		// Mutex for shared mem
		     bool init = false)
		: _buffer(static_cast<T**>(mem), tail, tailMutex, sem, init),
		  _allocator(static_cast<T**>(mem) + N , memMutex)
	    {
	    }
	    
	    T*    alloc() { return static_cast<T*>(_allocator.alloc(sizeof(T))); }
	    void* alloc(int size) { return _allocator.alloc(size); }
	    void  put(T* data) { _buffer.put(data); }

	private:
	    ProducerBuffer _buffer;
	    Allocator _allocator;
	};

	
	template <class Mutex, class Semaphore, class MemMutex, int N, int M>
	class UntypedProducer {
	    typedef MPS::RingBuffer::Producer<void*, Mutex, Semaphore, N> ProducerBuffer;
	    typedef MPS::Allocator<MemMutex, M> Allocator;

	public:
	    UntypedProducer(void* mem,			// Shared mem
			    volatile unsigned& tail,	// Tail of RingBuffer
			    Mutex& tailMutex,		// Mutex for tail manipulation
			    Semaphore& sem,		// Number of elements in buffer
			    MemMutex& memMutex,		// Mutex for shared mem
			    bool init = false)
		: _buffer(static_cast<void**>(mem), tail, tailMutex, sem, init),
		  _allocator(static_cast<void**>(mem) + N , memMutex)
	    {
	    }
	    
	    void* alloc(int size) { return _allocator.alloc(size); }
	    void  put(void* data) { _buffer.put(data); }

	private:
	    ProducerBuffer _buffer;
	    Allocator _allocator;
	};

	
	template <class T, class Mutex, class Semaphore, class MemMutex, int N>
	class Consumer {
	    typedef MPS::RingBuffer::Consumer<T*, Mutex, Semaphore, N> ConsumerBuffer;
	    typedef MPS::Allocator<MemMutex, N * sizeof(T)> Allocator;

	public:
	    Consumer(void* mem,			// Shared mem
		     volatile unsigned& head,	// Head of RingBuffer
		     Mutex& headMutex,		// Mutex for head manipulation
		     Semaphore& sem,		// Number of elements in buffer
		     MemMutex& memMutex)	// Mutex for shared mem
		: _buffer(static_cast<T**>(mem), head, headMutex, sem),
		  _allocator(static_cast<T**>(mem) + N , memMutex)
	    {
	    }
	    
	    void  free(T* addr) { _allocator.free(addr); }
	    T* get() { return _buffer.get(); }

	private:
	    ConsumerBuffer _buffer;
	    Allocator _allocator;
	};

	
	template <class Mutex, class Semaphore, class MemMutex, int N, int M>
	class UntypedConsumer {
	    typedef MPS::RingBuffer::Consumer<void*, Mutex, Semaphore, N> ConsumerBuffer;
	    typedef MPS::Allocator<MemMutex, M> Allocator;

	public:
	    UntypedConsumer(void* mem,			// Shared mem
			    volatile unsigned& head,	// Head of RingBuffer
			    Mutex& headMutex,		// Mutex for head manipulation
			    Semaphore& sem,		// Number of elements in buffer
			    MemMutex& memMutex)		// Mutex for shared mem
		: _buffer(static_cast<void**>(mem), head, headMutex, sem),
		  _allocator(static_cast<void**>(mem) + N , memMutex)
	    {
	    }
	    
	    void  free(void* addr) { _allocator.free(addr); }
	    void* get() { return _buffer.get(); }

	private:
	    ConsumerBuffer _buffer;
	    Allocator _allocator;
	};
	
    }
  
}

#endif
