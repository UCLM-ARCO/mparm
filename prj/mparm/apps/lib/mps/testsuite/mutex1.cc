#include "TestCase.hh"
#include <MPS/Mutex.h>
#include <MPS/RecMutex.h>

/*

Simple test cases for all mutex versions. Uses most of the API and
tests correct blocking on a locked mutex.

1. p1 lock, p2 lock, p1 unlock, p2 unlock

2. p1 lock, p2 tryLock, p1 unlock, p2 tryLock

*/


template<class Mutex>
class Mutex1 : public TestCase {
public:
    Mutex1(Mutex& mutex, const char* id) : TestCase(id), _mutex(mutex)
    {
    }
    
    bool run1();
    bool run2();

private:
    Mutex& _mutex;
};

template <class Mutex>
bool
Mutex1<Mutex>::run1()
{
    // 1. p1 lock, p2 lock, p1 unlock, p2 unlock
    {
	trace("About to lock (1.1)");
	typename Mutex::Lock lock(_mutex);

	stamp(11);			// Done 1.1
	
	trace("Waiting for p2 lock (1.2)");
	wait(12); 	// Waits for 1.2

	trace("About to unlock (1.3)");
	stamp(13);			// Done 1.3
    }

    trace("Waiting for p2 unlock (1.4)");
    wait(14); 		// Waits for 1.4
    
    // 2. p1 lock, p2 tryLock, p1 unlock, p2 tryLock
    {
	trace("About to lock (2.1)");
	typename Mutex::Lock lock(_mutex);

	stamp(21);			// Done 2.1
	
	trace("Waiting for p2 tryLock (2.2)");
	wait(22); 	// Waits for 2.2

	trace("About to unlock (2.3)");
    }
    stamp(23);			// Done 2.3

    return true;
}

template <class Mutex>
bool
Mutex1<Mutex>::run2()
{
    // 1. p1 lock, p2 lock, p1 unlock, p2 unlock
    trace("Waiting for p1 lock (1.1)");
    wait(11); 		// Waits for 1.1
    {
	trace("About to lock (1.2)");
	stamp(12);			// About to do 1.2
	typename Mutex::Lock lock(_mutex);

	if (!expect(13))
	    return false; 		// Entered before 1.3

	trace("About to unlock (1.3)");
	stamp(14);			// About to do 1.3
    }
    trace("Unlocked (1.4)");
    
    // 2. p1 lock, p2 tryLock, p1 unlock, p2 tryLock
    {
	trace("Waiting for p1 lock (2.1)");
	wait(21); 	// Waits for 2.1
	typename Mutex::TryLock lock(_mutex);

	if (lock.acquired()) {
	    failure("Got a locked lock (2.2)");
	    return false;		// Entered before 2.2
	}
	stamp(22);			// Done 2.2

	trace("Waiting for p1 unlock (2.3)");
	wait(23); 	// Waits for 2.3
	
	if (!lock.tryAcquire()) {
	    failure("Didn't release the lock (2.3)");
	    return false;		// Didn't release the lock
	}
    }
    trace("Unlocked (2.4)");

    return true;
}


typedef Mutex1<MPS::ShMem::Mutex> M1ShMutex;
typedef Mutex1<MPS::ShMem::SpinMutex> M1ShSpinMutex;

typedef Mutex1<MPS::ScrMem::Mutex> M1ScrMutex;
typedef Mutex1<MPS::ScrMem::SpinMutex> M1ScrSpinMutex;
typedef Mutex1<MPS::ScrMem::RemoteMutex> M1ScrRemoteMutex;
typedef Mutex1<MPS::ScrMem::LocalMutex> M1ScrLocalMutex;

typedef Mutex1<MPS::ShMem::RecMutex> M1ShRecMutex;
typedef Mutex1<MPS::ShMem::SpinRecMutex> M1ShSpinRecMutex;

typedef Mutex1<MPS::ScrMem::RecMutex> M1ScrRecMutex;
typedef Mutex1<MPS::ScrMem::SpinRecMutex> M1ScrSpinRecMutex;
typedef Mutex1<MPS::ScrMem::RemoteRecMutex> M1ScrRemoteRecMutex;
typedef Mutex1<MPS::ScrMem::LocalRecMutex> M1ScrLocalRecMutex;

class CompositeMutex1: public CompositeTestCase
{
    int _core;
    
    MPS::ShMem::Mutex _shMutex;
    MPS::ShMem::SpinMutex _shSpinMutex;

    MPS::ScrMem::Mutex _scrMutex;
    MPS::ScrMem::SpinMutex _scrSpinMutex;

    MPS::ShMem::RecMutex _shRecMutex;
    MPS::ShMem::SpinRecMutex _shSpinRecMutex;

    MPS::ScrMem::RecMutex _scrRecMutex;
    MPS::ScrMem::SpinRecMutex _scrSpinRecMutex;

    // These  are not instantiated on every process
    MPS::ScrMem::LocalMutex* _scrLocalMutex;
    MPS::ScrMem::RemoteMutex* _scrRemoteMutex;
    MPS::ScrMem::LocalRecMutex* _scrLocalRecMutex;
    MPS::ScrMem::RemoteRecMutex* _scrRemoteRecMutex;

    enum {
	ID = 10,
	SpinID,
	RecID,
	SpinRecID,
	RemoteID,
	RemoteRecID,
    };

public:
    CompositeMutex1()
	: CompositeTestCase("Mutex1"),
	  _core(get_proc_id()),
	  _shMutex(ID),
	  _shSpinMutex(SpinID),
	  _scrMutex(1, ID, 1 == _core),
	  _scrSpinMutex(1, SpinID, 1 == _core),
	  _shRecMutex(RecID),
	  _shSpinRecMutex(SpinRecID),
	  _scrRecMutex(1, RecID, 1 == _core),
	  _scrSpinRecMutex(1, SpinRecID, 1 == _core),
	  _scrLocalMutex(0),
	  _scrRemoteMutex(0),
	  _scrLocalRecMutex(0),
	  _scrRemoteRecMutex(0)
    {

	add(new M1ShMutex(_shMutex,"ShMutex1"));
	add(new M1ShSpinMutex(_shSpinMutex, "ShSpinMutex1"));
	add(new M1ScrMutex(_scrMutex, "ScrMutex1"));
	add(new M1ScrSpinMutex(_scrSpinMutex, "ScrSpinMutex1"));
	add(new M1ShRecMutex(_shRecMutex, "ShRecMutex1"));
	add(new M1ShSpinRecMutex(_shSpinRecMutex, "ShSpinRecMutex1"));
	add(new M1ScrRecMutex(_scrRecMutex, "ScrRecMutex1"));
	add(new M1ScrSpinRecMutex(_scrSpinRecMutex, "ScrSpinRecMutex1"));

	if (1 == get_proc_id()) {
	    _scrLocalMutex    = new MPS::ScrMem::LocalMutex(RemoteID, RemoteID, true);
	    _scrLocalRecMutex = new MPS::ScrMem::LocalRecMutex(RemoteRecID, RemoteRecID, true);
	    
	    add(new M1ScrLocalMutex(*_scrLocalMutex, "ScrLocalMutex1"));
	    add(new M1ScrLocalRecMutex(*_scrLocalRecMutex, "ScrLocalRecMutex1"));
	}
	else {
	    _scrRemoteMutex    = new MPS::ScrMem::RemoteMutex(1, RemoteID, RemoteID);
	    _scrRemoteRecMutex = new MPS::ScrMem::RemoteRecMutex(1, RemoteRecID, RemoteRecID);
	    
	    add(new M1ScrRemoteMutex(*_scrRemoteMutex, "ScrRemoteMutex"));
	    add(new M1ScrRemoteRecMutex(*_scrRemoteRecMutex, "ScrRemoteRecMutex"));
	}
    }
};

#ifndef COMBINED_TEST_CASES
int
main()
{
    CompositeMutex1 test;

    test.run();
    stop_simulation();
}
#endif
