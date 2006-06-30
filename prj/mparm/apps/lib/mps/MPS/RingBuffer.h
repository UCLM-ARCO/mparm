#ifndef MPS_RING_BUFFER_H
#define MPS_RING_BUFFER_H

// Generic RingBuffer implementation

// HeadMutex should be MPS::DummyMutex if there is a single reader.
// TailMutex should be MPS::DummyMutex if there is a single writer.

// head and tail may also be configured on private memories if there
// is a single reader or a single writer respectively.  Otherwise it
// may be configured on the scratch memory of the most frequent reader
// and writer respectively.

// Each end may be configured independently (e.g. one end may use a
// LocalSemaphore and another a RemoteSemaphore).

// N is a template parameter because modulo operations may be
// highly optimized by the compiler if N is a power of two

namespace MPS {

    namespace RingBuffer {

	//--------------------------------------------------------------------
	// Producer side of the RingBuffer

	template <class T, class Mutex, class Semaphore, int N>
	class Producer {
	public:
	    typedef typename T ValueType;
	    typedef typename Mutex Mutex;
	    typedef typename Semaphore Semaphore;

	    const int BufferSize = N;

	public:
	    Producer(volatile T* data,		// Storage area
		     volatile int& tail,	// Tail index
		     Mutex& tailMutex,		// Mutex for tail manipulation
		     Semaphore& sem,		// Number of elements in buffer
		     bool init = false)
		: _data(data), _tail(tail), _tailMutex(tailMutex), _sem(sem)
	    {
		if (init)
		    _tail = 0;
	    }
	
	    void put(const T& t)
	    {
		Mutex::Lock lock(_tailMutex);

		int tail = _tail;
		_tail = (tail + 1) % N;
		_data[tail] = t;
		_sem.post();
	    }
    
	private:
	    volatile T* _data;
	    volatile int& _tail;
	    Mutex& _tailMutex;
	    Semaphore& _sem;
	};

	//--------------------------------------------------------------------
	// Consumer side of the RingBuffer

	template <class T, class Mutex, class Semaphore, int N>
	class Consumer {
	public:
	    typedef typename T ValueType;
	    typedef typename Mutex Mutex;
	    typedef typename Semaphore Semaphore;

	    const int BufferSize = N;
	
	public:
	    Consumer(volatile T* data,		// Storage area
		     volatile int& head,	// Head index
		     Mutex& headMutex,		// Mutex for head manipulation
		     Semaphore& sem,		// Number of elements in buffer
		     bool init = false)
		: _data(data), _head(head), _headMutex(headMutex), _sem(sem)
	    {
		if (init)
		    _tail = 0;
	    }
	
	    T get()
	    {
		_sem.wait();
		Mutex::Lock lock(_headMutex);

		int head = _head;
		_head = (head + 1) % N;
		return _data[head];
	    }
        
	private:
	    volatile T* _data;
	    volatile int& _head;
	    Mutex& _headMutex;
	    Semaphore& _sem;
	};
	
    }    

}
#endif
