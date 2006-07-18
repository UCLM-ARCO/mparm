#include "TestCase.hh"
#include <MPS/Queue.h>
#include <MPS/Semaphore.h>

// Meassurements of Queue throughput

// 1. Single producer and a single consumer locking is unnecesary
// 2. Semaphores are SpinSemaphores
// 3. Size, even for data types, is always a power of two

// Note that the real power of queues is for multiple producers and
// consumers.  Each end can copy data in parallel with just minimal
// locking.

// Semaphore to signal initial condition
MPS::ScrMem::Semaphore end1(1,61,get_proc_id() == 1,0);
MPS::ScrMem::Semaphore end2(2,61,get_proc_id() == 1,0);

const int BufferSize = 32;

template <int DataSize, int Iterations>
class MQueue : public TestCaseDelegate {

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
    MQueue() : TestCaseDelegate() { }
    
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
    
    // Mutex for memory allocator
    MPS::ScrMem::SpinMutex memMutex(1,60,true);

    Producer prod(data, tail, mutex, semP, semC, memMutex, true);
    prod.activate(); // Only on one side

    end2.post();
    end1.wait();

    trace("Producing data");

    time_start(DataSize);
    start_metric();
    for (int i = 0; i < Iterations; ++i) {
	volatile DataType* item = prod.alloc();
	char buf[DataSize];
	memcpy(const_cast<DataType*>(item), buf, DataSize);
	prod.put(item);
    }
    stop_metric();
    
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
    
    // Mutex for memory allocator
    MPS::ScrMem::SpinMutex memMutex(1,60);

    end2.wait();

    Consumer cons(data, head, mutex, semP, semC, memMutex, true);

    end1.post();

    trace("Consuming data");

    for (int i = 0; i < Iterations; ++i) {
	volatile DataType* item = cons.get();
	cons.free(item);
    }
    time_stop(DataSize);

    return true;
}


int
main()
{
    TestCase<MQueue<Size, 65536/Size> > test;

    test.run();
    return 0;
}
