#ifndef MPS_ALLOCATOR_H
#define MPS_ALLOCATOR_H

/*! \file MPS/Allocator.h

  \brief A set of simple allocators for MPS.

  MPS provides a small set of very simple allocators specially suited
  for the MPS application domain.
 
*/
namespace MPS {

    /*! \brief Simple fixed-size allocator.
     *
     * It is able to allocate single-size elements.  A nice property
     * of this allocator is that you do not really need the original
     * allocator to be able to free buffers.
     *
     * \param T Type for allocated items.
     *
     * \param Mutex Type of mutex to protect concurrent access to the
     * allocator data area.
     *
     * \param N Size of the allocator data area in items of type T.
     * Actually it also needs 4*N words of additional storage for
     * memory management purposes (see description of
     * MPS::FixedSizeAllocator::ItemType. It is a template parameter
     * because modulo operations may be highly optimized by the
     * compiler if N is a power of two.
     */
    template <class T, class Mutex, int N>
    class FixedSizeAllocator
    {
	/*! \brief Private type for chunk allocation.
	 *
	 * It complements the item data with a small header.  The
	 * header is just used as a boolean indicating whether this
	 * item is already freed (value 0) or not.
	 */
	struct ItemType {
	    int header;
	    T   data;
	};
	
    public:

	/*! \brief Fixed-size allocator constructor.
	 *
	 * Multiple instances of this class may refer to the same
	 * allocator.  One and only one instance should initialize the
	 * allocator (see description of \a init parameter below).
	 *
	 * \param mem Storage area.  Should be able to hold N
	 * instances of T plus N instances of the header (see
	 * description of MPS::FixedSizeAllocator::ItemType).
	 *
	 * \param mutex Guarantees mutual exclusion when trying to
	 * access the allocator.
	 *
	 * \param init Whether this instance of the allocator should
	 * be initialized.  One instance of the allocator (and only
	 * one) must set this parameter to true.  It defaults to
	 * false.
	 */
	FixedSizeAllocator(volatile void* mem, Mutex& mutex, bool init = false)
	    : _mem(reinterpret_cast<volatile ItemType*>(mem)),
	      _mutex(mutex)
	{
	    if (init) {
		for (int i=0; i<N; ++i)
		    _mem[i].header = 0;
	    }
	}

	/*! \brief Allocates an instance of T.
	 *
	 * This method allocates a chunk of data whose size is exactly
	 * that of the \a T type.  The allocated chunk points to an
	 * uninitialized area.
	 *
	 * It is implemented as a simple wrapper of the alloc(int&)
	 * method.
	 *
	 * \returns Returns a pointer to the allocated data.
	 */
	volatile T* alloc()
	{
	    int index;
	    return alloc(index);
	}
	
	/*! \brief Frees a previously allocated chunk of data.
	 *
	 * This method assumes that the pointer was allocated by a
	 * call to \c alloc.  It would work even for chunks allocated
	 * on different processors since the storage area is shared.
	 *
	 * \param data Address previously returned by a former call to
	 * \c alloc either on the current processor or any other
	 * processor.
	 */
	void free(volatile T* data)
	{
	    volatile int* header = static_cast<volatile int*>(data) - 1;
	    *header = 0;
	}

    protected:

	/*! \brief Allocates an instance of T and returns the position
	 *  in the internal array.
	 *
	 * This method allocates a chunk of data whose size is exactly
	 * that of the \a T type and also returns the position inside
	 * the internal array.  The allocated chunk points to an
	 * uninitialized area.
	 *
	 * This method is not public since it exposes implementation
	 * details of FixedSizeAllocator.  It is useful for
	 * specialized versions of this allocator (e.g. see
	 * MPS::Channel::TransceiverAllocator).
	 *
	 * \param index Reference pointing to a variable where the
	 * allocated position will be returned.
	 *
	 * \returns Returns a pointer to the allocated data.
	 */
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

	/*! \brief Internal data storage area.
	 *
	 * It should be able to hold N instances of T plus N instances
	 * of the header (see description of
	 * MPS::FixedSizeAllocator::ItemType).
	 */
	volatile ItemType* _mem;

	/*! \brief Mutex used to synchronize concurrent accesses to
	 *  the allocator.
	 *
	 * Guarantees mutual exclusion when trying to access the
	 * allocator.
	 */
	Mutex& _mutex;
    };

    
    /*! \brief Simple sequential allocator.
     *
     * Naive sequential allocator for arbitrary sized chunks of data.
     * This allocator keeps a head and a tail pointer to an internal
     * storage area.  New chunks are allocated at the head.  Free
     * chunks are just marked to be freed and then made available
     * using a FIFO order by updating the tail pointer.  Therefore it
     * is optimized for FIFO ordered alloc/free operations, commonly
     * found in communications software.
     *
     * Allocated blocks are marked with an integer header indicating a
     * positive size. Free blocks are marked with the most significant
     * bit of the size header set to 1.
     *
     * It tries compaction on every invocation of \a free and also if
     * \a alloc cannot find enough memory.
     *
     * \param Mutex Type of mutex to protect concurrent access to the
     * allocator data area.
     *
     * \param N Size of the allocator data area in bytes.  It is a
     * template parameter because modulo operations may be highly
     * optimized by the compiler if N is a power of two.
     */
    template <class Mutex, int N>
    class Allocator
    {
    public:

	/*! \brief Sequential allocator constructor.
	 *
	 * Multiple instances of this class may refer to the same
	 * allocator.  One and only one instance should initialize the
	 * allocator (see description of \a init parameter below).
	 *
	 * \param mem Storage area.  Should be able to hold N
	 * bytes of data.
	 *
	 * \param mutex Guarantees mutual exclusion when trying to
	 * access the allocator.
	 *
	 * \param init Whether this instance of the allocator should
	 * be initialized.  One instance of the allocator (and only
	 * one) must set this parameter to true.  It defaults to
	 * false.
	 */
	Allocator(volatile void* mem, Mutex& mutex, bool init = false)
	    : _head(*static_cast<volatile int*>(mem)),
	      _tail(*(&_head + 1)),
	      _mem(reinterpret_cast<volatile char*>(&_tail + 1)),
	      _mutex(mutex)
	{
	    if (init)
		_head = _tail = 0;
	}

	/*! \brief Allocates a chunk of data.
	 *
	 * This method allocates a chunk of data of \a size bytes.
	 * The allocated chunk points to an uninitialized area.
	 *
	 * \returns Returns a pointer to the allocated data.
	 */
	volatile void* alloc(int size);

	/*! \brief Frees a previously allocated chunk of data.
	 *
	 * This method assumes that the pointer was allocated by a
	 * call to \c alloc.  It would work even for chunks allocated
	 * on different processors since the storage area is shared.
	 *
	 * \param data Address previously returned by a former call to
	 * \c alloc either on the current processor or any other
	 * processor.
	 */
	void free(volatile void* data);
    
    private:

	/*! \brief Alignment helper function.
	 *
	 * It returns the value passed as the first parameter
	 * incremented as needed to guarantee proper alignment to the
	 * boundary given as the second argument.
	 *
	 * \param v Value to be aligned.
	 *
	 * \param boundary Size of the alignment boundaries (1 for
	 * byte alignment, 2 for 16-bit aligment, 4 for 4-byte word
	 * alignment, and so on).
	 *
	 * \returns The returned value is greater or equal to \a v and
	 * aligns properly to the specified boundary.
	 */
	static int align(int v, int boundary) {
	    return (v + (boundary - 1)) & ~(boundary - 1);
	}

	/*! \brief Checks whether there is enough room for a chunk of data.
	 *
	 * Checks whether there is enough room at the tail of the
	 * storage area to allocate a chunk of \a sz bytes.  We need
	 * both \a head and \a tail in order to know the available
	 * free memory.  It is used to determine whether memory
	 * compaction would be needed or not.
	 *
	 * \param head Position of the first allocated chunk of data.
	 *
	 * \param tail Position of the first available chunk of data.
	 *
	 * \returns Returns \c true if there is enough memory
	 * available and \c false otherwise.
	 */
	bool enoughSpace(int head, int tail, int sz);

	/*! \brief Compacts free chunks of memory at either end.
	 *
	 * When a chunk is freed it is first marked with a 1 as the
	 * most significant bit of the size header.  Then a \c compact
	 * operation should be performed.
	 *
	 * This method examines whether there is a free chunk at the
	 * \a tail.  If one is found \a tail is updated accordingly.
	 * The operation is repeated until no more free chunks are
	 * found.
	 */
	void compact();

	/*! \brief Fills a size header.
	 *
	 * Writes the size of the block in the header word and updates
	 * the \a head pointer.
	 *
	 * \returns It returns a pointer to the newly allocated block.
	 */
	volatile void* fillBlock(int sz);

	/*! \brief Fills a header for an empty block.
	 *
	 * Writes the size of the block in the header word and sets
	 * the most significant bit to 1.  We need empty blocks at the
	 * end of the storage area if there is not enough free memory
	 * for an \c alloc operation.
	 */
	void fillEmptyBlock(int sz);
    
    private:

	/*! \brief Index to the head of the allocated area.
	 *
	 * New chunks of data would be allocated contiguosly at \a
	 * head.
	 */
	volatile int& _head;
	
	/*! \brief Index to the tail of the allocated area.
	 *
	 * Free chunks are made available from the \a tail.  Therefore
	 * this is optimized for FIFO ordered alloc/free operations.
	 */
	volatile int& _tail;

	/*! \brief Internal data storage area.
	 *
	 * It should be able to hold N bytes of data.
	 */
	volatile char* _mem;

	/*! \brief Mutex used to synchronize concurrent accesses to
	 *  the allocator.
	 *
	 * Guarantees mutual exclusion when trying to access the
	 * allocator.
	 */
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
