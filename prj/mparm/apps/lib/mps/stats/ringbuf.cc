#include "TestCase.hh"
#include <MPS/RingBuffer.h>
#include <MPS/Mutex.h>
#include <MPS/Semaphore.h>

// Meassurements of RingBuffer throughput

// 1. Single reader and a single consumer locking is unnecesary
// 2. Single consumer, semaphore is a SpinSemaphore in consumer
// 3. Size, even for data types, is always a power of two

#define N 64

const int BufferSize = 16;

template <int DataSize = 4>
class RingBuf : public TestCase {

    struct DataType {
	char data[DataSize];
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

template <int DataSize>
bool
RingBuf<DataSize>::run1()
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
    
    Producer prod(data, tail, mutex, semP, semC, true);

    stamp(11);
    wait(12);

#if 0
    // First we test full-speed memory transfer from scratch of core 1
    // to scratch of core 2

    report("Full speed memory transfer");
    
    time_start(0);
    memcpy(data, &MPS::ScrMem::memory_base(1)[64], DataSize*N);
    time_stop(0);
#endif
    
    report("MPS RingBuffer transmission");

    DataType item;
    
    time_start(DataSize);
    for (int i = 0; i < N; ++i) {
	prod.put(item);
    }
    
    return true;
}

template <int DataSize>
bool
RingBuf<DataSize>::run2()
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
    
    Consumer cons(data, head, mutex, semP, semC, true);

    report("MPS RingBuffer reception");

    wait(11);
    stamp(12);

    DataType item;
    
    for (int i = 0; i < N; ++i) {
	item = cons.get();
    }
    time_stop(DataSize);

    return true;
}


class CompositeRingBuf: public CompositeTestCase
{
public:
    CompositeRingBuf()
	: CompositeTestCase("Throughput of MPS RingBuf")
    {
 	add(new RingBuf<4>("RingBuf4"));
 	add(new RingBuf<8>("RingBuf8"));
 	add(new RingBuf<16>("RingBuf16"));
 	add(new RingBuf<32>("RingBuf32"));
 	add(new RingBuf<64>("RingBuf64"));
 	add(new RingBuf<128>("RingBuf128"));
 	add(new RingBuf<256>("RingBuf256"));
 	add(new RingBuf<512>("RingBuf512"));
 	add(new RingBuf<1024>("RingBuf1024"));
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
