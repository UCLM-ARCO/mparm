#include "TestCase.hh"
#include <MPS/Semaphore.h>

/*

Simple test cases for semaphores. Uses most of the API and tests
correct blocking on a locked semaphore.

1. p1 init(2), p2 dec, p1 dec, p2 dec(blq), p1 inc

*/


template<class Semaphore>
class Semaphore2 : public TestCase {
public:
    Semaphore2(Semaphore& mutex, const char* id) : TestCase(id), _sem(mutex)
    {
    }
    
    bool run1();
    bool run2();
    bool run3();

private:
    Semaphore& _sem;
};

template <class Semaphore>
bool
Semaphore2<Semaphore>::run1()
{
    wait(1,1);
    wait(1,2);

    trace("Sem initialized (1.1)");
    stamp(11, 0);
    wait(13, 2);

    trace("About to wait (1.4)");
    stamp(14,0);
    _sem.wait();

    trace("Waiting end (1.6)");
    wait(16, 2);

    stamp(16, 0);

    return true;
}

template <class Semaphore>
bool
Semaphore2<Semaphore>::run2()
{
    stamp(1, 1);

    trace("Waiting init (1.1)");
    wait(11, 0);

    trace("About to wait (1.2)");
    _sem.wait();

    stamp(12, 1);
    
    trace("Waiting wait (1.4)");
    wait(14, 0);

    trace("About to wait [blq] (1.5)");
    stamp(15, 1);
    _sem.wait();

    trace("Waiting end (1.6)");
    wait(16, 2);

    stamp(16, 1);
    
    return true;
}

template <class Semaphore>
bool
Semaphore2<Semaphore>::run3()
{
    stamp(1, 2);

    trace("Waiting wait (1.2)");
    wait(12, 1);

    trace("About to wait (1.3)");
    _sem.wait();

    stamp(13, 2);
    
    trace("Waiting wait (1.5)");
    wait(15, 1);

    _sem.post(2);

    stamp(16, 2);

    wait(16, 0);
    wait(16, 1);

    return true;
}

typedef Semaphore2<MPS::ShMem::Semaphore> M1ShSem;
typedef Semaphore2<MPS::ShMem::SpinSemaphore> M1ShSpinSem;

typedef Semaphore2<MPS::ScrMem::Semaphore> M1ScrSem;
typedef Semaphore2<MPS::ScrMem::SpinSemaphore> M1ScrSpinSem;
typedef Semaphore2<MPS::ScrMem::RemoteSemaphore> M1ScrRemoteSem;
typedef Semaphore2<MPS::ScrMem::LocalSemaphore> M1ScrLocalSem;

class CompositeSemaphore2: public CompositeTestCase
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
    CompositeSemaphore2()
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
    CompositeSemaphore2 test;

    test.run();
    stop_simulation();
}
#endif
