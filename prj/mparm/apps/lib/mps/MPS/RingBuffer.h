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

	template <class T, class Mutex, class PSemaphore, class CSemaphore, int N>
	class Producer {
	public:
	    Producer(volatile T* data,		// Storage area
		     volatile int& tail,	// Tail index
		     Mutex& tailMutex,		// Mutex for tail manipulation
		     PSemaphore& semP,		// Number of elements in buffer
		     CSemaphore& semC,		// Number of empty slots in buffer
		     bool init = false)
		: _data(data),
		  _tail(tail), _tailMutex(tailMutex),
		  _semP(semP), _semC(semC)
	    {
		if (init)
		    _tail = 0;
	    }

	    void activate() {
		_semC.init(N - 1); // we can optimize Consumer::get by
				   // just leaving 1 free slot
		_semP.init(0);
		_tail = 0;
	    }
	    
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
	    volatile T* _data;
	    volatile int& _tail;
	    Mutex& _tailMutex;
	    PSemaphore& _semP;
	    CSemaphore& _semC;
	};

	//--------------------------------------------------------------------
	// Consumer side of the RingBuffer

	template <class T, class Mutex, class PSemaphore, class CSemaphore, int N>
	class Consumer {
	public:
	    Consumer(volatile T* data,		// Storage area
		     volatile int& head,	// Head index
		     Mutex& headMutex,		// Mutex for head manipulation
		     PSemaphore& semP,		// Number of elements in buffer
		     CSemaphore& semC,		// Number of empty slots in buffer
		     bool init = false)
		: _data(data), _head(head), _headMutex(headMutex), _semP(semP), _semC(semC)
	    {
		if (init)
		    _head = 0;
	    }

	    void deactivate() {
		_semC.init(0);     // inhibit further puts
	    }

	    void activate() {
		_semC.init(N - 1); // we can optimize Consumer::get by
				   // just leaving 1 free slot
		_semP.init(0);
		_head = 0;
	    }
	    
	    T get()
	    {
		_semP.wait();
		typename Mutex::Lock lock(_headMutex);

		int head = _head;
		_head = (head + 1) % N;

		_semC.post();	// No problem to post now, the last
				// slot will never be touched
		return const_cast<const T&>(_data[head]);
	    }
        
	private:
	    volatile T* _data;
	    volatile int& _head;
	    Mutex& _headMutex;
	    PSemaphore& _semP;
	    CSemaphore& _semC;
	};
	
    }    

}
#endif
