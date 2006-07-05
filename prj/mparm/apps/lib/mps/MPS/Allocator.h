#ifndef MPS_ALLOCATOR_H
#define MPS_ALLOCATOR_H

// Several simple allocators for MPS

namespace MPS {

    // Simple fixed-size allocator

    // A nice property of this allocator is that you do not really
    // need the original allocator to be able to free buffers

    // Size is a template parameter because modulo operations may be
    // highly optimized by the compiler if N is a power of two

    template <class T, class Mutex, int N>
    class FixedSizeAllocator
    {
	struct ItemType {
	    int header;
	    T   data;
	};
	
    public:
	FixedSizeAllocator(volatile void* mem, Mutex& mutex, bool init = false)
	    : _mem(reinterpret_cast<volatile ItemType*>(mem)),
	      _mutex(mutex)
	{
	    if (init) {
		for (int i=0; i<N; ++i)
		    _mem[i].header = 0;
	    }
	}

	volatile T* alloc()
	{
	    int index;
	    return alloc(index);
	}
	
	void free(volatile T* data)
	{
	    volatile int* header = static_cast<volatile int*>(data) - 1;
	    *header = 0;
	}

    protected:
	volatile T* alloc(int& index)
	{
	    typename Mutex::Lock lock(_mutex);
	    for (int i=0; i<N; ++i) {
		if (0 == _mem[i].header) {
		    index = i;
		    return &_mem[i].data;
		}
	    }
	    return reinterpret_cast<T*>(-1); // out of memory
	}
	
    private:
	volatile ItemType* _mem;
	Mutex& _mutex;
    };

    
    // Naive sequential allocator for arbitrary sized chunks of data

    // Allocated blocks are marked with a positive size, free blocks
    // are marked with the most significant bit of the size header set
    // to 1.

    // It tries compaction on every invocation of free and if alloc
    // cannot find enough memory

    template <class Mutex, int N>
    class Allocator
    {
    public:
	Allocator(volatile void* mem, Mutex& mutex, bool init = false)
	    : _head(*static_cast<volatile int*>(mem)),
	      _tail(*(&_head + 1)),
	      _mem(reinterpret_cast<volatile char*>(&_tail + 1)),
	      _mutex(mutex)
	{
	    if (init)
		_head = _tail = 0;
	}

	volatile void* alloc(int size);
	void free(volatile void* data);
    
    private:
	static int align(int v, int boundary) {
	    return (v + (boundary - 1)) & ~(boundary - 1);
	}
    
	bool enoughSpace(int head, int tail, int sz);
	void compact();
	void fillEmptyBlock(int sz);
	volatile void* fillBlock(int sz);
    
    private:
	volatile int& _head;
	volatile int& _tail;
	volatile char* _mem;
	Mutex& _mutex;
    };


    template <class Mutex, int N>
    volatile void*
    Allocator<Mutex,N>::alloc(int size)
    {
	size = align(size, sizeof(int));
	
	typename Mutex::Lock lock(_mutex);

	int head = _head, tail = _tail, sz = size + sizeof(int);

	if (!enoughSpace(head, tail, sz)) {
	    compact();
	    if (!enoughSpace(head, tail, sz))
		return reinterpret_cast<void*>(-1); // out of memory
	}
	return fillBlock(size);
    }

    template <class Mutex, int N>
    void
    Allocator<Mutex,N>::free(volatile void* data)
    {
	volatile unsigned& _sz = *(static_cast<volatile unsigned*>(data) - 1);
	int sz = _sz;
	_sz = sz | 0x80000000;

	typename Mutex::TryLock lock(_mutex);
	if (lock.acquired())
	    compact();
    }

    template <class Mutex, int N>
    bool
    Allocator<Mutex,N>::enoughSpace(int head, int tail, int sz)
    {
	if (head > tail && head - tail < sz) {
	    return false;
	}
	else if (N - tail < sz) {
	    // Try wrapping around...
	    if (head < sz)
		return false;
	    fillEmptyBlock(N - tail - sizeof(int));
	}
	return true;
    }
    
    template <class Mutex, int N>
    void
    Allocator<Mutex,N>::compact()
    {
	for(;;) {
	    if (_tail == _head)			// Reached the end
		break;
	    unsigned tail = _tail;
	    volatile unsigned& _sz = *reinterpret_cast<volatile unsigned*>(&_mem[tail]);
	    unsigned sz = _sz;
	    if (0 == (sz & 0x80000000))		// Positive size, still active
		break;
	    sz = sz & (0x80000000 - 1);
	    tail += sz + sizeof(int);
	    _tail = tail % N;
	}
    }

    template <class Mutex, int N>
    void
    Allocator<Mutex,N>::fillEmptyBlock(int sz)
    {
	volatile unsigned& _sz = *reinterpret_cast<volatile unsigned*>(&_mem[_head]);
	fillBlock(sz);
	_sz = sz | 0x80000000;
    }
    
    template <class Mutex, int N>
    volatile void*
    Allocator<Mutex,N>::fillBlock(int sz)
    {
	volatile unsigned& _sz = *reinterpret_cast<volatile unsigned*>(&_mem[_head]);
	_sz = sz;
	unsigned head = _head + sz + sizeof(int);
	_head = head % N;
	return &_sz + 1;
    }
}
#endif
