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
    RawThroughput(const char* id, int* src) : TestCase(id), _src(src)
    {
    }
    
    bool run1();
    bool run2();

private:
    int* _src;
};

template <int DataSize, int Iterations>
bool
RawThroughput<DataSize, Iterations>::run1()
{
    int* dst = const_cast<int*>(&MPS::ScrMem::memory_base(2)[64]);
    int* src = _src;

    time_start(DataSize);
    for (int i = 0; i < Iterations ; ++i) {
	memcpy(dst, src, DataSize);
	src += DataSize; dst += DataSize;
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
	: CompositeTestCase("Throughput of MPS RingBuf"),
	  _srcScr(const_cast<int*>(&MPS::ScrMem::memory_base(1)[64])),
	  _srcPriv(0)
    {
 	add(new RawThroughput<4, 16384>("MemcpyPriv", _srcPriv));
 	add(new RawThroughput<8,  8192>("MemcpyPriv", _srcPriv));
 	add(new RawThroughput<16, 4096>("MemcpyPriv", _srcPriv));
 	add(new RawThroughput<32, 2048>("MemcpyPriv", _srcPriv));
 	add(new RawThroughput<64, 1024>("MemcpyPriv", _srcPriv));
 	add(new RawThroughput<128, 512>("MemcpyPriv", _srcPriv));
 	add(new RawThroughput<256, 256>("MemcpyPriv", _srcPriv));
 	add(new RawThroughput<512, 128>("MemcpyPriv", _srcPriv));
 	add(new RawThroughput<1024, 64>("MemcpyPriv", _srcPriv));
 	add(new RawThroughput<2048, 32>("MemcpyPriv", _srcPriv));
 	add(new RawThroughput<4096, 16>("MemcpyPriv", _srcPriv));

 	add(new RawThroughput<4, 16384>("MemcpyScratch", _srcScr));
 	add(new RawThroughput<8,  8192>("MemcpyScratch", _srcScr));
 	add(new RawThroughput<16, 4096>("MemcpyScratch", _srcScr));
 	add(new RawThroughput<32, 2048>("MemcpyScratch", _srcScr));
 	add(new RawThroughput<64, 1024>("MemcpyScratch", _srcScr));
 	add(new RawThroughput<128, 512>("MemcpyScratch", _srcScr));
 	add(new RawThroughput<256, 256>("MemcpyScratch", _srcScr));
 	add(new RawThroughput<512, 128>("MemcpyScratch", _srcScr));
 	add(new RawThroughput<1024, 64>("MemcpyScratch", _srcScr));
 	add(new RawThroughput<2048, 32>("MemcpyScratch", _srcScr));
 	add(new RawThroughput<4096, 16>("MemcpyScratch", _srcScr));
    }
    
private:
    int* _srcScr;
    int* _srcPriv;
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
