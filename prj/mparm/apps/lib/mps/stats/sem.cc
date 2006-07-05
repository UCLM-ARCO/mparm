#include "TestCase.hh"
#include <MPS/Semaphore.h>

// Meassurements of semaphores latency

#define N 20

// Semaphores to wait for end condition
MPS::ScrMem::Semaphore end1(1,61,get_proc_id() == 1,0);
MPS::ScrMem::Semaphore end2(2,61,get_proc_id() == 1,0);

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
	time_start(i);
	_sem.wait();

	trace("lock");
	sem2.post();
	sem1.wait();

	sleep(0);
	
	time_stop(i);
	_sem.post();
	
	trace("unlock");
	sem2.post();
	sem1.wait();
    }
    return true;
}

template <class Semaphore>
bool
Semaphore1<Semaphore>::run2()
{
    for (int i = 0; i<N; ++i)  {
	sem2.wait()
	sem1.post();

	time_start(i);
	_sem.wait();
	time_stop(i);

	trace("lock");

	_sem.post();
	
	trace("unlock");
	sem2.wait();
	sem1.post();
    }
    return true;
}


typedef Semaphore1<MPS::ShMem::Semaphore> M1ShSem;
typedef Semaphore1<MPS::ShMem::SpinSemaphore> M1ShSpinSem;

typedef Semaphore1<MPS::ScrMem::Semaphore> M1ScrSem;
typedef Semaphore1<MPS::ScrMem::SpinSemaphore> M1ScrSpinSem;
typedef Semaphore1<MPS::ScrMem::RemoteSemaphore> M1ScrRemoteSem;
typedef Semaphore1<MPS::ScrMem::LocalSemaphore> M1ScrLocalSem;

#ifdef __rtems__
typedef Semaphore1<MPS::ScrMem::RemoteSSemaphore> M1ScrRemoteSSem;
typedef Semaphore1<MPS::ScrMem::LocalSSemaphore> M1ScrLocalSSem;
#endif

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

#ifdef __rtems__
    MPS::ScrMem::LocalSSemaphore* _scrLocalSSem;
    MPS::ScrMem::RemoteSSemaphore* _scrRemoteSSem;
#endif
    
    MPS::ScrMem::LocalSpinSemaphore* _scrLocalSpinSem;
    MPS::ScrMem::RemoteSpinSemaphore* _scrRemoteSpinSem;

    enum {
	ID = 10,
	SpinID,
	RecID,
	RemoteID,
	RemoteSID,
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
#ifdef __rtems__
	  _scrLocalSSem(0),
	  _scrRemoteSSem(0),
#endif
	  _scrLocalSpinSem(0),
	  _scrRemoteSpinSem(0)
    {

 	add(new M1ShSem(_shSem,"ShMem::Sem"));
 	add(new M1ShSpinSem(_shSpinSem, "ShMem::SpinSem"));
 	add(new M1ScrSem(_scrSem, "ScrMem::Sem"));
 	add(new M1ScrSpinSem(_scrSpinSem, "ScrMem::SpinSem"));

	if (1 == get_proc_id()) {
	    _scrLocalSem = new MPS::ScrMem::LocalSemaphore(RemoteID, RemoteID, true, 1);
#ifdef __rtems__
	    _scrLocalSSem = new MPS::ScrMem::LocalSSemaphore(RemoteSID, RemoteSID, true, 1);
#endif
	    _scrLocalSpinSem = new MPS::ScrMem::LocalSpinSemaphore(RemoteSpinID, RemoteSpinID, true, 1);
	    add(new M1ScrLocalSem(*_scrLocalSem, "ScrMem::LocalSem"));
#ifdef __rtems__
	    add(new M1ScrLocalSSem(*_scrLocalSSem, "ScrMem::LocalSSem"));
#endif
	    add(new M1ScrLocalSpinSem(*_scrLocalSpinSem, "ScrMem::LocalSpinSem"));
	}
	else {
	    _scrRemoteSem = new MPS::ScrMem::RemoteSemaphore(1, RemoteID, RemoteID);
#ifdef __rtems__
	    _scrRemoteSSem = new MPS::ScrMem::RemoteSSemaphore(1, RemoteSID, RemoteSID);
#endif
	    _scrRemoteSpinSem = new MPS::ScrMem::RemoteSpinSemaphore(1, RemoteSpinID, RemoteSpinID);
	    add(new M1ScrRemoteSem(*_scrRemoteSem, "ScrMem::RemoteSem"));
#ifdef __rtems__
	    add(new M1ScrRemoteSSem(*_scrRemoteSSem, "ScrMem::RemoteSSem"));
#endif
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
