#ifndef MPS_ALLOCATOR_H
#define MPS_ALLOCATOR_H

// Naive sequential allocator for arbitrary sized chunks of data

// Size is a template parameter because modulo operations may be
// highly optimized by the compiler if N is a power of two

// Allocated blocks are marked with a positive size, free blocks are
// marked with the most significant bit of the size header set to 1.

// Compaction occurs on every invocation of free.

namespace MPS {
    
    template <class Mutex, int N>
    class Allocator
    {
    public:
	Allocator(void* mem, Mutex& mutex, bool init = false)
	    : _head(*static_cast<volatile int*>(mem)),
	      _tail(&_head + 1),
	      _mem(static_cast<char*>(&_tail + 1)),
	      _mutex(mutex)
	{
	    if (init)
		_head = _tail = 0;
	}

	void* alloc(int size);
	void free(void* data);
    
    private:
	static int align(int v, int boundary) {
	    return (v + (boundary - 1)) & ~(boundary - 1);
	}
    
	void compact();
	void fillEmptyBlock(int sz);
	void* fillBlock(int sz);
    
    private:
	volatile int& _head;
	volatile int& _tail;
	char* _mem;
	Mutex _mutex;
    };


    template <class Mutex, int N>
    void*
    Allocator<Mutex,N>::alloc(int size)
    {
	size = align(size, sizeof(int));
	
	Mutex::Lock lock(_mutex);

	int head = _head, tail = _tail, sz = size + sizeof(int);
	
	// Check if there is room
	if (head > tail && head - tail < sz) {
	    throw "Out of memory";
	}
	else if (N - tail < sz) {
	    // Try wrapping around...
	    if (head < sz) {
		throw "Out of memory";
	    }
	    fillEmptyBlock(N - tail - sizeof(int));
	}
	return fillBlock(size);
    }

    template <class Mutex, int N>
    void
    Allocator<Mutex,N>::free(void* data);
    {
	Mutex::Lock lock(_mutex);

	volatile unsigned& _sz = *(static_cast<volatile unsigned*>(data) - 1);
	int sz = _sz;
	_sz = sz | 0x80000000;
	compact();
    }

    template <class Mutex, int N>
    void
    Allocator<Mutex,N>::compact()
    {
	for(;;) {
	    unsigned tail = _tail;
	    volatile unsigned& _sz = static_cast<volatile int&>(_mem[tail]);
	    unsigned sz = _sz;
	    if (0 == (sz & 0x80000000))		// Positive size, still active
		break;
	    sz = sz & (0x80000000 - 1);
	    tail += sz + sizeof(int);
	    _tail = tail % N;
	    if (_tail == _head)			// Reached the end
		break;
	}
    }

    template <class Mutex, int N>
    void
    Allocator<Mutex,N>::fillEmptyBlock(int sz)
    {
	volatile unsigned& _sz = static_cast<volatile int&>(_mem[_head]);
	fillBlock(sz);
	_sz = sz | 0x80000000;
    }
    
    template <class Mutex, int N>
    void*
    Allocator<Mutex,N>::fillBlock(int sz)
    {
	volatile unsigned& _sz = static_cast<volatile int&>(_mem[_head]);
	_sz = sz;
	unsigned head = _head + sz + sizeof(int);
	_head = head % N;
	return &_sz + 1;
    }
}
#endif
