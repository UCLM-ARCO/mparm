#include "TestCase.hh"
#include <MPS/Semaphore.h>
#include <string.h>

// Meassurements of Raw throughput

template <int DataSize, int Iterations>
class RawThroughput : public TestCaseDelegate {

    struct DataType {
	int data[DataSize/sizeof(int)];
    };
    
public:
    RawThroughput(const char* id, int* src)
	: TestCase(id), _src(reinterpret_cast<DataType*>(src))
    {
    }
    
    bool run1();

private:
    DataType* _src;
};

template <int DataSize, int Iterations>
bool
RawThroughput<DataSize, Iterations>::run1()
{ 
    DataType*& dst = *reinterpret_cast<DataType**>(const_cast<int*>(&MPS::ScrMem::memory_base(1)[31]));
    DataType*& src = *reinterpret_cast<DataType**>(const_cast<int*>(&MPS::ScrMem::memory_base(1)[30]));

    dst = reinterpret_cast<DataType*>(const_cast<int*>(&MPS::ScrMem::memory_base(2)[64]));
    src = _src;

    start_metric();
    time_start(DataSize);
    for (int i = 0; i < Iterations ; ++i) {
	*dst++ = *src++;
    }
    time_stop(DataSize);
    stop_metric();
    
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
#if 0
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
#else
//    	add(new RawThroughput<4, 16384>("MemcpyScratch", _srcScr));
//    	add(new RawThroughput<8,  8192>("MemcpyScratch", _srcScr));
//    	add(new RawThroughput<16, 4096>("MemcpyScratch", _srcScr));
//    	add(new RawThroughput<32, 2048>("MemcpyScratch", _srcScr));
//    	add(new RawThroughput<64, 1024>("MemcpyScratch", _srcScr));
//    	add(new RawThroughput<128, 512>("MemcpyScratch", _srcScr));
    	add(new RawThroughput<256, 256>("MemcpyScratch", _srcScr));
//    	add(new RawThroughput<512, 128>("MemcpyScratch", _srcScr));
//    	add(new RawThroughput<1024, 64>("MemcpyScratch", _srcScr));
//    	add(new RawThroughput<2048, 32>("MemcpyScratch", _srcScr));
//  	add(new RawThroughput<4096, 16>("MemcpyScratch", _srcScr));
#endif
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
