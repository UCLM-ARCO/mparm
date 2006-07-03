#include "TestCase.hh"
#include <MPS/Semaphore.h>

// Meassurements of semaphores latency

#define N 20

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
    for (int i = 0; i<N; ++i)  {
	wait(0);

	time_start(i);
	_sem.wait();

	trace("lock");
	stamp(11);
	wait(12);

	sleep(0);
	
	time_stop(i);
	_sem.post();
	
	trace("unlock");
	stamp(13);
    }
    return true;
}

template <class Semaphore>
bool
Semaphore1<Semaphore>::run2()
{
    for (int i = 0; i<N; ++i)  {
	wait(11);

	stamp(12);

	time_start(i);
	_sem.wait();
	time_stop(i);

	trace("lock");

	_sem.post();
	
	trace("unlock");
	wait(13);
	stamp(0);
    }
    return true;
}


typedef Semaphore1<MPS::ShMem::Semaphore> M1ShSem;
typedef Semaphore1<MPS::ShMem::SpinSemaphore> M1ShSpinSem;

typedef Semaphore1<MPS::ScrMem::Semaphore> M1ScrSem;
typedef Semaphore1<MPS::ScrMem::SpinSemaphore> M1ScrSpinSem;
typedef Semaphore1<MPS::ScrMem::RemoteSemaphore> M1ScrRemoteSem;
typedef Semaphore1<MPS::ScrMem::LocalSemaphore> M1ScrLocalSem;
typedef Semaphore1<MPS::ScrMem::RemoteSpinSemaphore> M1ScrRemoteSpinSem;
typedef Semaphore1<MPS::ScrMem::LocalSpinSemaphore> M1ScrLocalSpinSem;

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
    MPS::ScrMem::LocalSpinSemaphore* _scrLocalSpinSem;
    MPS::ScrMem::RemoteSpinSemaphore* _scrRemoteSpinSem;

    enum {
	ID = 10,
	SpinID,
	RecID,
	RemoteID,
	RemoteSpinID
    };

public:
    CompositeSemaphore1()
	: CompositeTestCase("Sem1"),
	  _core(get_proc_id()),
	  _shSem(ID, ID, 1 == _core, 1),
	  _shSpinSem(SpinID, SpinID, 1 == _core, 1),
	  _scrSem(1, ID, 1 == _core, 1),
	  _scrSpinSem(1, SpinID, 1 == _core, 1),
	  _scrLocalSem(0),
	  _scrRemoteSem(0),
	  _scrLocalSpinSem(0),
	  _scrRemoteSpinSem(0)
    {

 	add(new M1ShSem(_shSem,"ShMem::Sem"));
 	add(new M1ShSpinSem(_shSpinSem, "ShMem::SpinSem"));
 	add(new M1ScrSem(_scrSem, "ScrMem::Sem"));
 	add(new M1ScrSpinSem(_scrSpinSem, "ScrMem::SpinSem"));

	if (1 == get_proc_id()) {
	    _scrLocalSem = new MPS::ScrMem::LocalSemaphore(RemoteID, RemoteID, true, 1);
	    _scrLocalSpinSem = new MPS::ScrMem::LocalSpinSemaphore(RemoteSpinID, RemoteSpinID, true, 1);
	    add(new M1ScrLocalSem(*_scrLocalSem, "ScrMem::LocalSem"));
	    add(new M1ScrLocalSpinSem(*_scrLocalSpinSem, "ScrMem::LocalSpinSem"));
	}
	else {
	    _scrRemoteSem = new MPS::ScrMem::RemoteSemaphore(1, RemoteID, RemoteID);
	    _scrRemoteSpinSem = new MPS::ScrMem::RemoteSpinSemaphore(1, RemoteSpinID, RemoteSpinID);
	    add(new M1ScrRemoteSem(*_scrRemoteSem, "ScrMem::RemoteSem"));
	    add(new M1ScrRemoteSpinSem(*_scrRemoteSpinSem, "ScrMem::RemoteSpinSem"));
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
