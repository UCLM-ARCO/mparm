#ifndef MPS_QUEUE_H
#define MPS_QUEUE_H

namespace MPS {

    namespace Queue {

	template <class Mutex, class Semaphore, class MemMutex>
	class UntypedProducer {
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
	    MPS::RingBuffer::Producer _buffer;
	    Allocator _allocator;
	};

	
	template <class Mutex, class Semaphore, class MemMutex>
	class UntypedConsumer {
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
	    MPS::RingBuffer::Consumer _buffer;
	    Allocator _allocator;
	};
	
    }
  
}

#endif
