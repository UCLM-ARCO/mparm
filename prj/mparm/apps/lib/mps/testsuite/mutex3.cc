#include "TestCase.hh"
#include <MPS/RecMutex.h>

/*

Simple test cases for recursive mutex versions. Uses recursive locks
and tests correct blocking on a locked mutex.

1. p1 lock, p2 trylock, p1 lock, p2 trylock, p1 unlock,
   p2 trylock, p1 unlock, p2 trylock

*/


template<class Mutex>
class Mutex3 : public TestCase {
public:
    Mutex3(Mutex& mutex, const char* id) : TestCase(id), _mutex(mutex)
    {
    }
    
    bool run1();
    bool run2();

private:
    Mutex& _mutex;
};

template <class Mutex>
bool
Mutex3<Mutex>::run1()
{
    // 1. p1 lock, p2 trylock, p1 lock, p2 trylock, p1 unlock,
    //    p2 trylock, p1 unlock, p2 trylock
    {
	trace("About to lock (1.1)");
	typename Mutex::Lock lock(_mutex);

	_stamp[0] = 11;
	
	trace("Waiting for p2 trylock (1.2)");
	while (_stamp[0] != 12);

	{
	    trace("About to lock (1.3)");
	    typename Mutex::Lock lock(_mutex);

	    _stamp[0] = 13;
	
	    trace("Waiting for p2 trylock (1.4)");
	    while (_stamp[0] != 14);

	    trace("About to unlock (1.5)");
	}
	_stamp[0] = 15;

	trace("Waiting for p2 trylock (1.6)");
	while (_stamp[0] != 16);

	trace("About to unlock (1.5)");
    }
    _stamp[0] = 17;

    return true;
}

template <class Mutex>
bool
Mutex3<Mutex>::run2()
{
    {
	trace("Waiting for p1 lock (1.1)");
	while (_stamp[0] != 11);
	typename Mutex::TryLock lock(_mutex);

	if (lock.acquired()) {
	    failure("Got a locked lock (1.1)");
	    return false;
	}
	_stamp[0] = 12;

	trace("Waiting for p1 lock (1.3)");
	while (_stamp[0] != 13);
	
	if (lock.tryAcquire()) {
	    failure("Got a locked lock (1.3)");
	    return false;
	}

	_stamp[0] = 14;

	trace("Waiting for p1 unlock (1.5)");
	while (_stamp[0] != 15);
	
	if (lock.tryAcquire()) {
	    failure("Got a locked lock (1.5)");
	    return false;
	}

	_stamp[0] = 16;

	trace("Waiting for p1 unlock (1.7)");
	while (_stamp[0] != 17);
	
	if (!lock.tryAcquire()) {
	    failure("Failed to release the lock (1.7)");
	    return false;
	}
    }
    _stamp[0] = 18;
    trace("Unlocked (1.8)");

    return true;
}


typedef Mutex3<MPS::ShMem::RecMutex> M1ShRecMutex;
typedef Mutex3<MPS::ShMem::SpinRecMutex> M1ShSpinRecMutex;

typedef Mutex3<MPS::ScrMem::RecMutex> M1ScrRecMutex;
typedef Mutex3<MPS::ScrMem::SpinRecMutex> M1ScrSpinRecMutex;
typedef Mutex3<MPS::ScrMem::RemoteRecMutex> M1ScrRemoteRecMutex;
typedef Mutex3<MPS::ScrMem::LocalRecMutex> M1ScrLocalRecMutex;

class CompositeMutex3: public CompositeTestCase
{
    int _core;
    
    MPS::ShMem::RecMutex _shRecMutex;
    MPS::ShMem::SpinRecMutex _shSpinRecMutex;

    MPS::ScrMem::RecMutex _scrRecMutex;
    MPS::ScrMem::SpinRecMutex _scrSpinRecMutex;

    // These  are not instantiated on every process
    MPS::ScrMem::LocalRecMutex* _scrLocalRecMutex;
    MPS::ScrMem::RemoteRecMutex* _scrRemoteRecMutex;

    enum {
	RecID = 10,
	SpinRecID,
	RemoteRecID,
    };

public:
    CompositeMutex3()
	: CompositeTestCase("Mutex3"),
	  _core(get_proc_id()),
	  _shRecMutex(RecID),
	  _shSpinRecMutex(SpinRecID),
	  _scrRecMutex(1, RecID, 1 == _core),
	  _scrSpinRecMutex(1, SpinRecID, 1 == _core),
	  _scrLocalRecMutex(0),
	  _scrRemoteRecMutex(0)
    {

	add(new M1ShRecMutex(_shRecMutex, "ShRecMutex3"));
	add(new M1ShSpinRecMutex(_shSpinRecMutex, "ShSpinRecMutex3"));
	add(new M1ScrRecMutex(_scrRecMutex, "ScrRecMutex3"));
	add(new M1ScrSpinRecMutex(_scrSpinRecMutex, "ScrSpinRecMutex3"));

	if (1 == get_proc_id()) {
	    _scrLocalRecMutex = new MPS::ScrMem::LocalRecMutex(RemoteRecID, RemoteRecID, true);
	    
	    add(new M1ScrLocalRecMutex(*_scrLocalRecMutex, "ScrLocalRecMutex3"));
	}
	else {
	    _scrRemoteRecMutex = new MPS::ScrMem::RemoteRecMutex(1, RemoteRecID, RemoteRecID);
	    
	    add(new M1ScrRemoteRecMutex(*_scrRemoteRecMutex, "ScrRemoteRecMutex"));
	}
    }
};

#ifndef COMBINED_TEST_CASES
int
main()
{
    CompositeMutex3 test;

    test.run();
    stop_simulation();
}
#endif
