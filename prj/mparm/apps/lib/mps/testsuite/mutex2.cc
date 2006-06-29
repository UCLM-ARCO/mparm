#include "TestCase.hh"
#include <MPS/Mutex.h>
#include <MPS/RecMutex.h>

/*

Similar to mutex1 but also tests more than two cores.  A similar test
case wouldn't work on ss_semaphore.h because just a single task is
allowed to wait on a locked mutex. For both recursive and
non-recursive versions.

First we try two processes blocked on a locked mutex. Then we test a
tryLock on a mutex that has been requested by two processes.

1. p1 lock, p2/p3 lock, p1 unlock, p2/p3 unlock

2. p1 lock, p3 lock, p2 tryLock, p1 unlock, p3 unlock, p2 tryLock

*/


template<class Mutex>
class Mutex2 : public TestCase {
public:
    Mutex2(Mutex& mutex, const char* id) : TestCase(id), _mutex(mutex)
    {
    }
    
    bool run1();
    bool run2();
    bool run3();

private:
    Mutex& _mutex;
};



template <class Mutex>
bool
Mutex2<Mutex>::run1()
{
    // 1. p1 lock, p2/p3 lock, p1 unlock, p2/p3 unlock
    {
	// It is important to invoke RemoteMutexes constructors before
	// we use a LocalMutex. The remaining varieties are not
	// affected.
	trace("Wait till all cores ready (0.1)");
	while (_stamp[1] != 1 || _stamp[2] != 1);
	

	trace("About to lock (1.1)");
	typename Mutex::Lock lock(_mutex);

	_stamp[0] = 11;			// Done 1.1
	
	trace("Waiting for p2/p3 to lock (1.2)");
	while (_stamp[1] != 12 || _stamp[2] != 12); 	// Waits for 1.2

	trace("About to unlock (1.3)");
	_stamp[0] = 13;			// Done 1.3
    }

    trace("Waiting for p2/p3 unlock (1.4)");
    while (_stamp[1] != 14 || _stamp[2] != 14); 	// Waits for 1.4
    
    // 2. p1 lock, p3 lock, p2 tryLock, p1 unlock, p3 unlock, p2 tryLock
    {
	trace("About to lock (2.1)");
	typename Mutex::Lock lock(_mutex);

	_stamp[0] = 21;			// Done 2.1
	
	trace("Waiting for p2 tryLock (2.3)");
	while (_stamp[1] != 23); 	// Waits for 2.3

	trace("About to unlock (2.4)");
    }
    _stamp[0] = 24;			// Done 2.4

    trace("Waiting for end");
    while (_stamp[1] != 0);
    _stamp[0] = 0;

    return true;
}

template <class Mutex>
bool
Mutex2<Mutex>::run2()
{
    _stamp[1] = 1;
    
    // 1. p1 lock, p2/p3 lock, p1 unlock, p2/p3 unlock
    trace("Waiting for p1 lock (1.1)");
    while (_stamp[0] != 11); 		// Waits for 1.1
    {
	trace("About to lock (1.2)");
	_stamp[1] = 12;			// About to do 1.2
	typename Mutex::Lock lock(_mutex);

	if (_stamp[0] != 13) {
	    failure(13, _stamp[0]);
	    return false; 		// Entered before 1.3
	}

	trace("About to unlock (1.4)");
	_stamp[1] = 14;			// About to do 1.3
    }
    trace("Unlocked (1.4)");
    
    // 2. p1 lock, p3 lock, p2 tryLock, p1 unlock, p3 unlock, p2 tryLock
    {
	trace("Waiting for p3 lock (2.2)");
	while (_stamp[2] != 22); 	// Waits for 2.2
	typename Mutex::TryLock lock(_mutex);

	if (lock.acquired()) {
	    failure("Got a locked lock (2.3)");
	    return false;		// Entered before 2.2
	}
	_stamp[1] = 23;			// Done 2.3

	trace("Waiting for p3 unlock (2.5)");
	while (_stamp[2] != 25); 	// Waits for 2.5
	
	if (!lock.tryAcquire()) {
	    failure("Didn't release the lock (2.5)");
	    return false;		// Didn't release the lock
	}
    }
    trace("Unlocked (2.6)");
    _stamp[1] = 0;
    
    return true;
}

template <class Mutex>
bool
Mutex2<Mutex>::run3()
{
    _stamp[2] = 1;

    // 1. p1 lock, p2/p3 lock, p1 unlock, p2/p3 unlock
    trace("Waiting for p1 lock (1.1)");
    while (_stamp[0] != 11); 	// Waits for 1.1
    {
	trace("About to lock (1.2)");
	_stamp[2] = 12;			// About to do 1.2
	typename Mutex::Lock lock(_mutex);

	if (_stamp[0] != 13) {
	    failure(13, _stamp[0]);
	    return false; 		// Entered before 1.3
	}

	trace("About to unlock (1.4)");
	_stamp[2] = 14;			// About to do 1.4
    }
    trace("Unlocked (1.4)");
    
    // 2. p1 lock, p3 lock, p2 tryLock, p1 unlock, p3 unlock, p2 tryLock
    {
	trace("Waiting for p1 lock (2.1)");
	while (_stamp[0] != 21); 	// Waits for 2.1

	trace("About to lock (2.2)");
	_stamp[2] = 22;
	typename Mutex::Lock lock(_mutex);

	if (_stamp[0] != 24) {
	    failure(24, _stamp[0]);
	    return false; 		// Entered before 2.4
	}

	trace("About to unlock (2.5)");
    }
    _stamp[2] = 25;		// About to do 2.5

    trace("Waiting for end");
    while (_stamp[1] != 0);
    _stamp[2] = 0;
    
    return true;
}


typedef Mutex2<MPS::ShMem::Mutex> M1ShMutex;
typedef Mutex2<MPS::ShMem::SpinMutex> M1ShSpinMutex;

typedef Mutex2<MPS::ScrMem::Mutex> M1ScrMutex;
typedef Mutex2<MPS::ScrMem::SpinMutex> M1ScrSpinMutex;
typedef Mutex2<MPS::ScrMem::RemoteMutex> M1ScrRemoteMutex;
typedef Mutex2<MPS::ScrMem::LocalMutex> M1ScrLocalMutex;

typedef Mutex2<MPS::ShMem::RecMutex> M1ShRecMutex;
typedef Mutex2<MPS::ShMem::SpinRecMutex> M1ShSpinRecMutex;

typedef Mutex2<MPS::ScrMem::RecMutex> M1ScrRecMutex;
typedef Mutex2<MPS::ScrMem::SpinRecMutex> M1ScrSpinRecMutex;
typedef Mutex2<MPS::ScrMem::RemoteRecMutex> M1ScrRemoteRecMutex;
typedef Mutex2<MPS::ScrMem::LocalRecMutex> M1ScrLocalRecMutex;

class CompositeMutex2: public CompositeTestCase
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
    CompositeMutex2()
	: CompositeTestCase("Mutex2"),
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
 	add(new M1ShMutex(_shMutex,"ShMutex2"));
  	add(new M1ShSpinMutex(_shSpinMutex, "ShSpinMutex2"));
  	add(new M1ScrMutex(_scrMutex, "ScrMutex2"));
  	add(new M1ScrSpinMutex(_scrSpinMutex, "ScrSpinMutex2"));
  	add(new M1ShRecMutex(_shRecMutex, "ShRecMutex2"));
  	add(new M1ShSpinRecMutex(_shSpinRecMutex, "ShSpinRecMutex2"));
  	add(new M1ScrRecMutex(_scrRecMutex, "ScrRecMutex2"));
 	add(new M1ScrSpinRecMutex(_scrSpinRecMutex, "ScrSpinRecMutex2"));

 	if (1 == get_proc_id()) {
 	    _scrLocalMutex    = new MPS::ScrMem::LocalMutex(RemoteID, RemoteID, true);
 	    _scrLocalRecMutex = new MPS::ScrMem::LocalRecMutex(RemoteRecID, RemoteRecID, true);
	    
 	    add(new M1ScrLocalMutex(*_scrLocalMutex, "ScrLocalMutex2"));
 	    add(new M1ScrLocalRecMutex(*_scrLocalRecMutex, "ScrLocalRecMutex2"));
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
    CompositeMutex2 test;

    //test.debug(true);
    test.run();
    stop_simulation();
}
#endif
