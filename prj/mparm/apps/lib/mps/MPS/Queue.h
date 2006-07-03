#ifndef MPS_QUEUE_H
#define MPS_QUEUE_H

// Queues allow reference semantics for RingBuffers.  This is useful
// for the implementation of zero-copy communication.

// Queues encapsulate an Allocator and a RingBuffer.  The RingBuffer
// does transmit pointers.

// Both, RingBuffer and Allocator will use contiguous memory
// areas. Locking may be configured independently on each end.

// There are two constructors available. The first simpler one just
// allocates the RingBuffer and the Allocator contiguously

#include <MPS/RingBuffer.h>
#include <MPS/Allocator.h>

namespace MPS {

    namespace Queue {

	template <class T, class Mutex, class PSemaphore, class CSemaphore, class MemMutex, int N>
	class Producer {
	    typedef MPS::RingBuffer::Producer<volatile T*, Mutex, PSemaphore, CSemaphore, N> ProducerBuffer;
	    typedef MPS::Allocator<MemMutex, N * (sizeof(T) + sizeof(int))> Allocator;
	    
	public:
	    Producer(volatile void* mem,	// Shared mem
		     volatile int& tail,	// Tail of RingBuffer
		     Mutex& tailMutex,		// Mutex for tail manipulation
		     PSemaphore& semP,		// Number of elements in buffer
		     CSemaphore& semC,		// Number of empty slots in buffer
		     MemMutex& memMutex,	// Mutex for shared mem
		     bool init = false)
		: _buffer(static_cast<volatile T* volatile *>(mem), tail, tailMutex, semP, semC, init),
		  _allocator(static_cast<volatile T* volatile *>(mem) + N , memMutex, init)
	    {
	    }
	    
	    Producer(volatile void* buf,	// RingBuffer area
		     volatile void* mem,	// Shared mem
		     volatile int& tail,	// Tail of RingBuffer
		     Mutex& tailMutex,		// Mutex for tail manipulation
		     PSemaphore& semP,		// Number of elements in buffer
		     CSemaphore& semC,		// Number of empty slots in buffer
		     MemMutex& memMutex,	// Mutex for shared mem
		     bool init = false)
		: _buffer(static_cast<volatile T* volatile *>(buf), tail, tailMutex, semP, semC, init),
		  _allocator(mem, memMutex, init)
	    {
	    }
	    
	    volatile T* alloc() { return static_cast<volatile T*>(_allocator.alloc(sizeof(T))); }
	    void* alloc(int size) { return _allocator.alloc(size); }
	    void  put(volatile T* data) { _buffer.put(data); }
	    void activate() { _buffer.activate(); }

	private:
	    ProducerBuffer _buffer;
	    Allocator _allocator;
	};

	
	template <class Mutex, class PSemaphore, class CSemaphore, class MemMutex, int N, int M>
	class UntypedProducer {
	    typedef MPS::RingBuffer::Producer<volatile void*, Mutex, PSemaphore, CSemaphore, N> ProducerBuffer;
	    typedef MPS::Allocator<MemMutex, M> Allocator;

	public:
	    UntypedProducer(volatile void* mem,		// Shared mem
			    volatile int& tail,		// Tail of RingBuffer
			    Mutex& tailMutex,		// Mutex for tail manipulation
			    PSemaphore& semP,		// Number of elements in buffer
			    CSemaphore& semC,		// Number of empty slots in buffer
			    MemMutex& memMutex,		// Mutex for shared mem
			    bool init = false)
		: _buffer(static_cast<volatile void* volatile *>(mem), tail, tailMutex, semP, semC, init),
		  _allocator(static_cast<volatile void* volatile *>(mem) + N , memMutex)
	    {
	    }
	    
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
	    
	    void* alloc(int size) { return _allocator.alloc(size); }
	    void  put(void* data) { _buffer.put(data); }
	    void  activate() { _buffer.activate(); }

	private:
	    ProducerBuffer _buffer;
	    Allocator _allocator;
	};

	
	template <class T, class Mutex, class PSemaphore, class CSemaphore, class MemMutex, int N>
	class Consumer {
	    typedef MPS::RingBuffer::Consumer<volatile T*, Mutex, PSemaphore, CSemaphore, N> ConsumerBuffer;
	    typedef MPS::Allocator<MemMutex, N * (sizeof(T) + sizeof(int))> Allocator;

	public:
	    Consumer(volatile void* mem,	// Shared mem
		     volatile int& head,	// Head of RingBuffer
		     Mutex& headMutex,		// Mutex for head manipulation
		     PSemaphore& semP,		// Number of elements in buffer
		     CSemaphore& semC,		// Number of empty slots in buffer
		     MemMutex& memMutex,	// Mutex for shared mem
		     bool init = false)
		: _buffer(static_cast<volatile T* volatile *>(mem), head, headMutex, semP, semC, init),
		  _allocator(static_cast<volatile T* volatile *>(mem) + N , memMutex)
	    {
	    }
	    
	    Consumer(volatile void* buf,	// RingBuffer area
		     volatile void* mem,	// Shared mem
		     volatile int& head,	// Head of RingBuffer
		     Mutex& headMutex,		// Mutex for head manipulation
		     PSemaphore& semP,		// Number of elements in buffer
		     CSemaphore& semC,		// Number of empty slots in buffer
		     MemMutex& memMutex,	// Mutex for shared mem
		     bool init = false)
		: _buffer(static_cast<volatile T* volatile *>(buf), head, headMutex, semP, semC, init),
		  _allocator(mem, memMutex)
	    {
	    }

	    void free(volatile T* addr) { _allocator.free(addr); }
	    void activate() { _buffer.activate(); }
	    volatile T* get() { return _buffer.get(); }

	private:
	    ConsumerBuffer _buffer;
	    Allocator _allocator;
	};

	
	template <class Mutex, class PSemaphore, class CSemaphore, class MemMutex, int N, int M>
	class UntypedConsumer {
	    typedef MPS::RingBuffer::Consumer<volatile void*, Mutex, PSemaphore, CSemaphore, N> ConsumerBuffer;
	    typedef MPS::Allocator<MemMutex, M> Allocator;

	public:
	    UntypedConsumer(volatile void* mem,		// Shared mem
			    volatile int& head,		// Head of RingBuffer
			    Mutex& headMutex,		// Mutex for head manipulation
			    PSemaphore& semP,		// Number of elements in buffer
			    CSemaphore& semC,		// Number of empty slots in buffer
			    MemMutex& memMutex,		// Mutex for shared mem
			    bool init = false)
		: _buffer(static_cast<volatile void* volatile *>(mem), head, headMutex, semP, semC, init),
		  _allocator(static_cast<volatile void* volatile *>(mem) + N , memMutex)
	    {
	    }
	    
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
	    
	    void free(volatile void* addr) { _allocator.free(addr); }
	    void activate() { _buffer.activate(); }
	    volatile void* get() { return _buffer.get(); }

	private:
	    ConsumerBuffer _buffer;
	    Allocator _allocator;
	};
	
    }
  
}

#endif
