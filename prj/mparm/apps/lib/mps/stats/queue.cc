#include "TestCase.hh"
#include <MPS/Queue.h>
#include <MPS/Semaphore.h>

// Meassurements of Queue throughput

// 1. Single reader and a single consumer locking is unnecesary
// 2. Semaphores are SpinSemaphores
// 3. Size, even for data types, is always a power of two

// Note that the real power of queues is for multiple producers and
// consumers.  Each end can copy data in parallel with just minimal
// locking.

const int BufferSize = 32;

template <int DataSize = 4, int Iterations = 16384>
class MQueue : public TestCase {

    struct DataType {
	int data[DataSize/sizeof(int)];
    };
    
    typedef MPS::Queue::Producer<
	DataType,
	MPS::DummyMutex,
	MPS::ScrMem::SpinSemaphore,
	MPS::ScrMem::SpinSemaphore,
	MPS::ScrMem::SpinMutex,
	BufferSize> Producer;

    typedef MPS::Queue::Consumer<
	DataType,
	MPS::DummyMutex,
	MPS::ScrMem::SpinSemaphore,
	MPS::ScrMem::SpinSemaphore,
	MPS::ScrMem::SpinMutex,
	BufferSize> Consumer;

public:
    MQueue(const char* id) : TestCase(id)
    {
    }
    
    bool run1();
    bool run2();

private:
};

template <int DataSize, int Iterations>
bool
MQueue<DataSize, Iterations>::run1()
{
    trace("Producer begins");

    // Storage area (should be get with an allocator)
    volatile void* data = &MPS::ScrMem::memory_base(2)[64];

    // Tail pointer must be local 
    volatile int& tail = MPS::ScrMem::memory_base(1)[63];

    // Single writer, we do not need a mutex
    MPS::DummyMutex mutex;

    // Semaphore counting produced slots must be on dest
    MPS::ScrMem::SpinSemaphore semP(2,62);
    
    // Semaphore counting consumed slots must be local
    MPS::ScrMem::SpinSemaphore semC(1,62);
    
    // Semaphore to wait for end condition
    MPS::ScrMem::Semaphore end1(1,61,true,0);
    MPS::ScrMem::Semaphore end2(2,61);

    // Mutex for memory allocator
    MPS::ScrMem::SpinMutex memMutex(1,60,true);

    Producer prod(data, tail, mutex, semP, semC, memMutex, true);

    end1.wait();
    end2.post();

    trace("Producing data");

    time_start(DataSize);
    for (int i = 0; i < Iterations; ++i) {
	volatile DataType* item = prod.alloc();
#if 0
	char buf[DataSize];
	memcpy(const_cast<DataType*>(item), buf, DataSize);
#else
	memcpy(const_cast<DataType*>(item),
	       const_cast<int*>(&MPS::ScrMem::memory_base(1)[64]),
	       DataSize);
#endif
	prod.put(item);
    }
    
    trace("Waiting end1");

    end1.wait();
    end2.post();

    return true;
}

template <int DataSize, int Iterations>
bool
MQueue<DataSize, Iterations>::run2()
{
    trace("Consumer begins");

    // Storage area (should be get with an allocator)
    volatile void* data = &MPS::ScrMem::memory_base(2)[64];

    // Head pointer must be local 
    volatile int& head = MPS::ScrMem::memory_base(2)[63];

    // Single reader, we do not need a mutex
    MPS::DummyMutex mutex;

    // Semaphore counting produced slots must be on dest
    MPS::ScrMem::SpinSemaphore semP(2,62);
    
    // Semaphore counting consumed slots must be local
    MPS::ScrMem::SpinSemaphore semC(1,62);
    
    // Semaphore to signal end condition
    MPS::ScrMem::Semaphore end1(1,61);
    MPS::ScrMem::Semaphore end2(2,61,true,0);

    // Mutex for memory allocator
    MPS::ScrMem::SpinMutex memMutex(1,60);

    Consumer cons(data, head, mutex, semP, semC, memMutex, true);

    end1.post();
    end2.wait();

    trace("Consuming data");

    for (int i = 0; i < Iterations; ++i) {
	volatile DataType* item = cons.get();
	cons.free(item);
    }
    time_stop(DataSize);

    trace("Posting end1");

    end1.post();
    end2.wait();

    return true;
}


class CompositeMQueue: public CompositeTestCase
{
public:
    CompositeMQueue()
	: CompositeTestCase("Throughput of MPS Queues")
    {
  	add(new MQueue<32, 2048>("Queue32"));
  	add(new MQueue<64, 1024>("Queue64"));
  	add(new MQueue<128, 512>("Queue128"));
  	add(new MQueue<256, 256>("Queue256"));
  	add(new MQueue<512, 128>("Queue512"));
  	add(new MQueue<1024, 64>("Queue1024"));
  	add(new MQueue<2048, 32>("Queue2048"));
  	add(new MQueue<4096, 16>("Queue4096"));
    }
};

#ifndef COMBINED_TEST_CASES
int
main()
{
    CompositeMQueue test;

    //test.debug(true);
    test.run();
    stop_simulation();
}
#endif
