#include "TestCase.hh"
#include <MPS/Semaphore.h>
#include <string.h>

// Meassurements of Raw throughput

template <int DataSize = 4, int Iterations = 16384>
class RawThroughput : public TestCase {

    struct DataType {
	int data[DataSize/sizeof(int)];
    };
    
public:
    RawThroughput(const char* id) : TestCase(id)
    {
    }
    
    bool run1();
    bool run2();
};

template <int DataSize, int Iterations>
bool
RawThroughput<DataSize, Iterations>::run1()
{
    time_start(DataSize);
    for (int i = 0; i < Iterations ; ++i) {
#if 1
	char buf[DataSize];
	memcpy(const_cast<int*>(&MPS::ScrMem::memory_base(2)[64]),
	       buf,
	       DataSize);
#else
	memcpy(const_cast<int*>(&MPS::ScrMem::memory_base(2)[64]),
	       const_cast<int*>(&MPS::ScrMem::memory_base(1)[64]),
	       DataSize);
#endif
    }
    time_stop(DataSize);
    
    MPS::ScrMem::Semaphore end(2,61);
    end.post();
    
    return true;
}

template <int DataSize, int Iterations>
bool
RawThroughput<DataSize, Iterations>::run2()
{
    MPS::ScrMem::Semaphore end(2,61,true,0);
    end.wait();
    return true;
}

class CompositeRawThroughput: public CompositeTestCase
{
public:
    CompositeRawThroughput()
	: CompositeTestCase("Throughput of MPS RingBuf")
    {
 	add(new RawThroughput<4, 16384>("RawThroughput4"));
 	add(new RawThroughput<8,  8192>("RawThroughput8"));
 	add(new RawThroughput<16, 4096>("RawThroughput16"));
 	add(new RawThroughput<32, 2048>("RawThroughput32"));
 	add(new RawThroughput<64, 1024>("RawThroughput64"));
 	add(new RawThroughput<128, 512>("RawThroughput128"));
 	add(new RawThroughput<256, 256>("RawThroughput256"));
 	add(new RawThroughput<512, 128>("RawThroughput512"));
 	add(new RawThroughput<1024, 64>("RawThroughput1024"));
 	add(new RawThroughput<2048, 32>("RawThroughput2048"));
 	add(new RawThroughput<4096, 16>("RawThroughput4096"));
    }
};

#ifndef COMBINED_TEST_CASES
int
main()
{
    CompositeRawThroughput test;

    test.run();
    stop_simulation();
}
#endif
