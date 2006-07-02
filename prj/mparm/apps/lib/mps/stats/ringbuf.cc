#include "TestCase.hh"
#include <MPS/RingBuffer.h>
#include <MPS/Mutex.h>
#include <MPS/Semaphore.h>

// Meassurements of RingBuffer throughput

// 1. Single reader and a single consumer locking is unnecesary
// 2. Semaphores are SpinSemaphores located at the apropriate end
// 3. Size, even for data types, is always a power of two

const int BufferSize = 32;

template <int DataSize = 4, int Iterations = 16384>
class RingBuf : public TestCase {

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
    RingBuf(const char* id) : TestCase(id)
    {
    }
    
    bool run1();
    bool run2();

private:
};

template <int DataSize, int Iterations>
bool
RingBuf<DataSize, Iterations>::run1()
{
    // Storage area (should be get with an allocator)
    volatile DataType* data = reinterpret_cast<volatile DataType*>(&MPS::ScrMem::memory_base(2)[64]);

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

    Producer prod(data, tail, mutex, semP, semC, true);

    stamp(11);
    wait(12);

    DataType item;

    time_start(DataSize);
    for (int i = 0; i < Iterations; ++i) {
	item.data[DataSize/4 - 1] = i;
	prod.put(item);
    }
    
    end1.wait();
    end2.post();

    return true;
}

template <int DataSize, int Iterations>
bool
RingBuf<DataSize, Iterations>::run2()
{
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
    
    // Semaphore to signal end condition
    MPS::ScrMem::Semaphore end1(1,61);
    MPS::ScrMem::Semaphore end2(2,61,true,0);

    Consumer cons(data, head, mutex, semP, semC, true);

    wait(11);
    stamp(12);

    DataType item;

    for (int i = 0; i < Iterations; ++i) {
	item = cons.get();
#if 0
	if (item.data[DataSize/4 - 1] != i)
	    failure("Did not send data");
#endif
    }
    time_stop(DataSize);

    end1.post();
    end2.post();
    
    return true;
}


class CompositeRingBuf: public CompositeTestCase
{
public:
    CompositeRingBuf()
	: CompositeTestCase("Throughput of MPS RingBuf")
    {
 	add(new RingBuf<4, 16384>("RingBuf4"));
 	add(new RingBuf<8,  8192>("RingBuf8"));
 	add(new RingBuf<16, 4096>("RingBuf16"));
 	add(new RingBuf<32, 2048>("RingBuf32"));
 	add(new RingBuf<64, 1024>("RingBuf64"));
 	add(new RingBuf<128, 512>("RingBuf128"));
 	add(new RingBuf<256, 256>("RingBuf256"));
 	add(new RingBuf<512, 128>("RingBuf512"));
 	add(new RingBuf<1024, 64>("RingBuf1024"));
    }
};

#ifndef COMBINED_TEST_CASES
int
main()
{
    CompositeRingBuf test;

    test.run();
    stop_simulation();
}
#endif
