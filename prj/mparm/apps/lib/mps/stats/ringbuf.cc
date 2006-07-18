#include "TestCase.hh"
#include <MPS/RingBuffer.h>
#include <MPS/Mutex.h>
#include <MPS/Semaphore.h>

// Meassurements of RingBuffer throughput

// 1. Single producer and a single consumer locking is unnecesary
// 2. Semaphores are SpinSemaphores located at the apropriate end
// 3. Size, even for data types, is always a power of two

// Semaphores to wait for initial condition
MPS::ScrMem::Semaphore end1(1,0,get_proc_id() == 1,0);
MPS::ScrMem::Semaphore end2(2,0,get_proc_id() == 1,0);

const int BufferSize = 32;

template <int DataSize, int Iterations>
class RingBuf : public TestCaseDelegate {

    struct DataType {
	int data[DataSize/sizeof(int)];
    };
    
    typedef MPS::RingBuffer::Producer<
	DataType,
	MPS::DummyMutex,
	MPS::ScrMem::SpinSemaphore,
	MPS::ScrMem::SpinSemaphore,
	BufferSize> Producer;

    typedef MPS::RingBuffer::Consumer<
	DataType,
	MPS::DummyMutex,
	MPS::ScrMem::SpinSemaphore,
	MPS::ScrMem::SpinSemaphore,
	BufferSize> Consumer;

public:
    RingBuf() : TestCaseDelegate() {}
    
    bool run1();
    bool run2();
};


template <int DataSize, int Iterations>
bool
RingBuf<DataSize, Iterations>::run1()
{
    trace("Starting producer");
    
    // Storage area (should be get with an allocator)
    volatile DataType* data = reinterpret_cast<volatile DataType*>(&MPS::ScrMem::memory_base(2)[64]);

    // Tail pointer must be local 
    volatile int& tail = MPS::ScrMem::memory_base(1)[63];

    // Single writer, we do not need a mutex
    MPS::DummyMutex mutex;

    // Semaphore counting the # of produced slots must be on dest
    MPS::ScrMem::SpinSemaphore semP(2,62,true,0);
    
    // Semaphore counting the # of consumed slots must be local
    MPS::ScrMem::SpinSemaphore semC(1,62,true,0);
    

    trace("Building producer side");
    Producer prod(data, tail, mutex, semP, semC, true);
    prod.activate(); // Only on one side

    end2.post();
    end1.wait();

    DataType* item = reinterpret_cast<DataType*>(const_cast<int*>(&MPS::ScrMem::memory_base(1)[64]));

    trace("Producing data");

    time_start(DataSize);
    start_metric();
    for (int i = 0; i < Iterations; ++i) {
#if 0
	for (int j=0; j< DataSize/4; ++j)
	    item[i].data[j] = i;
#endif
	prod.put(item[i]);
    }
    stop_metric();

    return true;
}

template <int DataSize, int Iterations>
bool
RingBuf<DataSize, Iterations>::run2()
{
    trace("Starting consumer");

    // Storage area (should be get with an allocator)
    volatile DataType* data = reinterpret_cast<volatile DataType*>(&MPS::ScrMem::memory_base(2)[64]);

    // Head pointer must be local 
    volatile int& head = MPS::ScrMem::memory_base(2)[63];

    // Single reader, we do not need a mutex
    MPS::DummyMutex mutex;

    // Semaphore counting produced slots must be on dest
    MPS::ScrMem::SpinSemaphore semP(2,62);
    
    // Semaphore counting consumed slots must be local
    MPS::ScrMem::SpinSemaphore semC(1,62);
    
    end2.wait();

    trace("Building consumer side");
    Consumer cons(data, head, mutex, semP, semC, true);

    trace("Sync");

    end1.post();

    DataType item;

    trace("Getting data");

    for (int i = 0; i < Iterations; ++i) {
	item = cons.get();
#if 0
	for (int j=0; j< DataSize/4; ++j) {
	    if (item.data[j] != i) {
		pr("Send failed i =", i, PR_CPU_ID | PR_STRING | PR_NEWL | PR_DEC);
		pr("Send failed j =", j, PR_CPU_ID | PR_STRING | PR_NEWL | PR_DEC);
		goto done;
	    }
	}
#endif
    }
 done:
    
    time_stop(DataSize);

    return true;
}


int
main()
{
    TestCase<RingBuf<Size, 65536/Size> > test;

    test.run();
    return 0;
}
