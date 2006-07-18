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
    RawThroughput() : TestCaseDelegate() { }
    bool run1();
};


template <int DataSize, int Iterations>
bool
RawThroughput<DataSize, Iterations>::run1()
{ 
#ifdef Copy_From_Priv
    DataType* src = 0;
#else
    DataType* src = reinterpret_cast<DataType*>(const_cast<int*>(MPS::ScrMem::memory_base(1)));
#endif
    DataType* dst = reinterpret_cast<DataType*>(const_cast<int*>(MPS::ScrMem::memory_base(2)));

    time_start(DataSize);
    start_metric();
    for (int i = 0; i < Iterations ; ++i) {
	*dst++ = *src++;
    }
    stop_metric();
    time_stop(DataSize);
    
    return true;
}


int
main()
{
    TestCase<RawThroughput<Size, 65536/Size> > test;

    test.run();
    return 0;
}
