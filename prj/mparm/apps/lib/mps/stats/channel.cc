#include "TestCase.hh"
#include <MPS/SimpleChannel.h>
#include <MPS/Semaphore.h>

// Meassurements of Channel throughput

// 1. Single reader and a single consumer locking is unnecesary
// 2. Semaphores are SpinSemaphores
// 3. Size, even for data types, is always a power of two

// Note that the real power of channels is for double-sided
// communications.  Each end can recycle buffers with just minimal
// locking.

const int BufferSize = 32;

// Semaphore to signal end condition
MPS::ScrMem::Semaphore end1(1,61,get_proc_id() == 1,0);
MPS::ScrMem::Semaphore end2(2,61,get_proc_id() == 1,0);

template <int DataSize = 4, int Iterations = 16384>
class MChannel : public TestCase {

    struct DataType {
	int data[DataSize/sizeof(int)];
    };
    
    typedef MPS::Channel::Acceptor<2, 32*DataSize> Acceptor;
    typedef MPS::Channel::Connector<32*DataSize> Connector;
    typedef MPS::Channel::Transceiver<DataType,
				      MPS::ScrMem::SpinSemaphore,
				      MPS::ScrMem::SpinSemaphore,
				      MPS::ScrMem::SpinMutex,
				      32> Transceiver;

public:
    MChannel(const char* id) : TestCase(id)
    {
    }
    
    bool run1();
    bool run2();

private:
};

template <int DataSize, int Iterations>
bool
MChannel<DataSize, Iterations>::run1()
{
    trace("Producer begins");

    end1.wait();

    // Storage area (should be get with an allocator)
    Connector conn(2,
		   &MPS::ScrMem::memory_base(2)[64], 64,
		   &MPS::ScrMem::memory_base(1)[64], 64);

    trace("Trying to connect");

    volatile MPS::Channel::ClientTransceiverState& state = conn.connect();

    trace("Connected");

    Transceiver tr(state);
    
    trace("Producing data");

    time_start(DataSize);
    for (int i = 0; i < Iterations; ++i) {
	volatile DataType* item = tr.alloc();
	memcpy(const_cast<DataType*>(item),
	       const_cast<int*>(&MPS::ScrMem::memory_base(1)[64]),
	       DataSize);
	tr.put(item);
    }
    
    trace("Waiting end1");

    end1.wait();
    end2.post();

    return true;
}

template <int DataSize, int Iterations>
bool
MChannel<DataSize, Iterations>::run2()
{
    trace("Consumer begins");

    Acceptor a(&MPS::ScrMem::memory_base(2)[64], 64);

    trace("Listening");

    a.listen();

    end1.post();

    trace("Accepting connections");

    volatile MPS::Channel::ServerTransceiverState& state = a.accept();

    trace("Connected");

    Transceiver tr(state);
    
    trace("Consuming data");

    for (int i = 0; i < Iterations; ++i) {
	volatile DataType* item = tr.get();
	tr.free(item);
    }
    time_stop(DataSize);

    trace("Posting end1");

    end1.post();
    end2.wait();

    return true;
}


class CompositeMChannel: public CompositeTestCase
{
public:
    CompositeMChannel()
	: CompositeTestCase("Throughput of MPS Channels")
    {
   	add(new MChannel<4, 16384>("Channel"));
   	add(new MChannel<8,  8192>("Channel"));
   	add(new MChannel<16, 4096>("Channel"));
   	add(new MChannel<32, 2048>("Channel"));
   	add(new MChannel<64, 1024>("Channel"));
   	add(new MChannel<128, 512>("Channel"));
   	add(new MChannel<256, 256>("Channel"));
  	add(new MChannel<512, 128>("Channel"));
  	add(new MChannel<1024, 64>("Channel"));
  	add(new MChannel<2048, 32>("Channel"));
  	add(new MChannel<4096, 16>("Channel"));
    }
};

#ifndef COMBINED_TEST_CASES
int
main()
{
    CompositeMChannel test;

    test.debug(true);
    test.run();
    stop_simulation();
}
#endif
