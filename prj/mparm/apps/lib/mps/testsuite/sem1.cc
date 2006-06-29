#include "TestCase.hh"
#include <MPS/Semaphore.h>

/*

Simple test cases for semaphores. Uses most of the API and tests
correct blocking on a locked semaphore.

1. p1 init(2), p2 dec, p1 dec, p2 dec(blq), p1 inc

*/


template<class Semaphore>
class Semaphore1 : public TestCase {
public:
    Semaphore1(Semaphore& mutex, const char* id) : TestCase(id), _sem(mutex)
    {
    }
    
    bool run1();
    bool run2();

private:
    Semaphore& _sem;
};

template <class Semaphore>
bool
Semaphore1<Semaphore>::run1()
{
    wait(1);

    trace("Sem initialized (1.1)");
    stamp(11);
    wait(12);

    trace("About to wait (1.3)");
    _sem.wait();
    
    stamp(13);
    wait(14);
    
    for (int i=0; i<100000; ++i);
    trace("About to post (1.5)");
    _sem.post();

    trace("Done (1.5)");
    stamp(15);

    return true;
}

template <class Semaphore>
bool
Semaphore1<Semaphore>::run2()
{
    stamp(1);

    trace("Waiting init (1.1)");
    wait(11);

    trace("About to wait (1.2)");
    _sem.wait();

    stamp(12);
    
    trace("Waiting wait (1.3)");
    wait(13);

    trace("About to wait [blq] (1.4)");
    stamp(14);
    _sem.wait();

    trace("Waiting end (1.5)");
    wait(15);

    return true;
}


typedef Semaphore1<MPS::ShMem::Semaphore> M1ShSem;
typedef Semaphore1<MPS::ShMem::SpinSemaphore> M1ShSpinSem;

typedef Semaphore1<MPS::ScrMem::Semaphore> M1ScrSem;
typedef Semaphore1<MPS::ScrMem::SpinSemaphore> M1ScrSpinSem;
typedef Semaphore1<MPS::ScrMem::RemoteSemaphore> M1ScrRemoteSem;
typedef Semaphore1<MPS::ScrMem::LocalSemaphore> M1ScrLocalSem;

class CompositeSemaphore1: public CompositeTestCase
{
    int _core;
    
    MPS::ShMem::Semaphore _shSem;
    MPS::ShMem::SpinSemaphore _shSpinSem;

    MPS::ScrMem::Semaphore _scrSem;
    MPS::ScrMem::SpinSemaphore _scrSpinSem;

    // These  are not instantiated on every process
    MPS::ScrMem::LocalSemaphore* _scrLocalSem;
    MPS::ScrMem::RemoteSemaphore* _scrRemoteSem;

    enum {
	ID = 10,
	SpinID,
	RecID,
	RemoteID,
    };

public:
    CompositeSemaphore1()
	: CompositeTestCase("Sem1"),
	  _core(get_proc_id()),
	  _shSem(ID, ID, 1 == _core, 2),
	  _shSpinSem(SpinID, SpinID, 1 == _core, 2),
	  _scrSem(1, ID, 1 == _core, 2),
	  _scrSpinSem(1, SpinID, 1 == _core, 2),
	  _scrLocalSem(0),
	  _scrRemoteSem(0)
    {

 	add(new M1ShSem(_shSem,"ShSem1"));
 	add(new M1ShSpinSem(_shSpinSem, "ShSpinSem1"));
 	add(new M1ScrSem(_scrSem, "ScrSem1"));
 	add(new M1ScrSpinSem(_scrSpinSem, "ScrSpinSem1"));

	if (1 == get_proc_id()) {
	    _scrLocalSem
		= new MPS::ScrMem::LocalSemaphore(RemoteID, RemoteID, true, 2);
	    add(new M1ScrLocalSem(*_scrLocalSem, "ScrLocalSem1"));
	}
	else {
	    _scrRemoteSem
		= new MPS::ScrMem::RemoteSemaphore(1, RemoteID, RemoteID);
	    add(new M1ScrRemoteSem(*_scrRemoteSem, "ScrRemoteSem"));
	}
    }
};

#ifndef COMBINED_TEST_CASES
int
main()
{
    CompositeSemaphore1 test;

    test.run();
    stop_simulation();
}
#endif
