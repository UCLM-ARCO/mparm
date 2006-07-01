#include "TestCase.hh"
#include <MPS/Mutex.h>
#include <MPS/RecMutex.h>

// Meassurements of mutex latency

#define N 20

template<class Mutex>
class MutexLatency : public TestCase {
public:
    MutexLatency(Mutex& mutex, const char* id) : TestCase(id), _mutex(mutex)
    {
    }
    
    bool run1();
    bool run2();

private:
    Mutex& _mutex;
};

template <class Mutex>
bool
MutexLatency<Mutex>::run1()
{
    for (int i = 0; i<N; ++i)  {
	wait(0);
	{
	    time_start(i);
	    typename Mutex::Lock lock(_mutex);
	    trace("lock");
	    stamp(11);
	    wait(12);
	    sleep(0);
	    time_stop(i);
	}
	trace("unlock");
	stamp(13);
    }
    return true;
}

template <class Mutex>
bool
MutexLatency<Mutex>::run2()
{
    for (int i = 0; i<N; ++i)  {
	wait(11);
	{
	    stamp(12);
	    time_start(i);
	    typename Mutex::Lock lock(_mutex);
	    time_stop(i);
	    trace("lock");
	}
	trace("unlock");
	wait(13);
	stamp(0);
    }
    return true;
}


typedef MutexLatency<MPS::ShMem::Mutex> M1ShMutex;
typedef MutexLatency<MPS::ShMem::SpinMutex> M1ShSpinMutex;

typedef MutexLatency<MPS::ScrMem::Mutex> M1ScrMutex;
typedef MutexLatency<MPS::ScrMem::SpinMutex> M1ScrSpinMutex;
typedef MutexLatency<MPS::ScrMem::RemoteMutex> M1ScrRemoteMutex;
typedef MutexLatency<MPS::ScrMem::LocalMutex> M1ScrLocalMutex;
typedef MutexLatency<MPS::ScrMem::RemoteSpinMutex> M1ScrRemoteSpinMutex;
typedef MutexLatency<MPS::ScrMem::LocalSpinMutex> M1ScrLocalSpinMutex;

typedef MutexLatency<MPS::ShMem::RecMutex> M1ShRecMutex;
typedef MutexLatency<MPS::ShMem::SpinRecMutex> M1ShSpinRecMutex;

typedef MutexLatency<MPS::ScrMem::RecMutex> M1ScrRecMutex;
typedef MutexLatency<MPS::ScrMem::SpinRecMutex> M1ScrSpinRecMutex;
typedef MutexLatency<MPS::ScrMem::RemoteRecMutex> M1ScrRemoteRecMutex;
typedef MutexLatency<MPS::ScrMem::LocalRecMutex> M1ScrLocalRecMutex;
typedef MutexLatency<MPS::ScrMem::RemoteSpinRecMutex> M1ScrRemoteSpinRecMutex;
typedef MutexLatency<MPS::ScrMem::LocalSpinRecMutex> M1ScrLocalSpinRecMutex;

class CompositeMutexLatency: public CompositeTestCase
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
    MPS::ScrMem::LocalSpinMutex* _scrLocalSpinMutex;
    MPS::ScrMem::RemoteSpinMutex* _scrRemoteSpinMutex;
    MPS::ScrMem::LocalRecMutex* _scrLocalRecMutex;
    MPS::ScrMem::RemoteRecMutex* _scrRemoteRecMutex;
    MPS::ScrMem::LocalSpinRecMutex* _scrLocalSpinRecMutex;
    MPS::ScrMem::RemoteSpinRecMutex* _scrRemoteSpinRecMutex;

    enum {
	ID = 10,
	SpinID,
	RecID,
	SpinRecID,
	RemoteID,
	RemoteSpinID,
	RemoteRecID,
	RemoteSpinRecID,
    };

public:
    CompositeMutexLatency()
	: CompositeTestCase("MPS Mutex latencies"),
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
	  _scrLocalSpinMutex(0),
	  _scrRemoteSpinMutex(0),
	  _scrLocalRecMutex(0),
	  _scrRemoteRecMutex(0),
	  _scrLocalSpinRecMutex(0),
	  _scrRemoteSpinRecMutex(0)
    {

	add(new M1ShMutex(_shMutex,"ShMutex"));
	add(new M1ShSpinMutex(_shSpinMutex, "ShSpinMutex"));
	add(new M1ScrMutex(_scrMutex, "ScrMutex"));
	add(new M1ScrSpinMutex(_scrSpinMutex, "ScrSpinMutex"));
	add(new M1ShRecMutex(_shRecMutex, "ShRecMutex"));
	add(new M1ShSpinRecMutex(_shSpinRecMutex, "ShSpinRecMutex"));
	add(new M1ScrRecMutex(_scrRecMutex, "ScrRecMutex"));
	add(new M1ScrSpinRecMutex(_scrSpinRecMutex, "ScrSpinRecMutex"));

	if (1 == get_proc_id()) {
	    _scrLocalMutex        = new MPS::ScrMem::LocalMutex(RemoteID, RemoteID, true);
	    _scrLocalSpinMutex    = new MPS::ScrMem::LocalSpinMutex(RemoteSpinID, RemoteSpinID, true);
	    _scrLocalRecMutex     = new MPS::ScrMem::LocalRecMutex(RemoteRecID, RemoteRecID, true);
	    _scrLocalSpinRecMutex = new MPS::ScrMem::LocalSpinRecMutex(RemoteSpinRecID, RemoteSpinRecID, true);
	    
	    add(new M1ScrLocalMutex(*_scrLocalMutex, "ScrLocalMutex"));
	    add(new M1ScrLocalSpinMutex(*_scrLocalSpinMutex, "ScrLocalSpinMutex"));
	    add(new M1ScrLocalRecMutex(*_scrLocalRecMutex, "ScrLocalRecMutex"));
	    add(new M1ScrLocalSpinRecMutex(*_scrLocalSpinRecMutex, "ScrLocalSpinRecMutex"));
	}
	else {
	    _scrRemoteMutex        = new MPS::ScrMem::RemoteMutex(1, RemoteID, RemoteID);
	    _scrRemoteSpinMutex    = new MPS::ScrMem::RemoteSpinMutex(1, RemoteSpinID, RemoteSpinID);
	    _scrRemoteRecMutex     = new MPS::ScrMem::RemoteRecMutex(1, RemoteRecID, RemoteRecID);
	    _scrRemoteSpinRecMutex = new MPS::ScrMem::RemoteSpinRecMutex(1, RemoteSpinRecID, RemoteSpinRecID);
	    
	    add(new M1ScrRemoteMutex(*_scrRemoteMutex, "ScrRemoteMutex"));
	    add(new M1ScrRemoteSpinMutex(*_scrRemoteSpinMutex, "ScrRemoteSpinMutex"));
	    add(new M1ScrRemoteRecMutex(*_scrRemoteRecMutex, "ScrRemoteRecMutex"));
	    add(new M1ScrRemoteSpinRecMutex(*_scrRemoteSpinRecMutex, "ScrRemoteSpinRecMutex"));
	}
    }
};

#ifndef COMBINED_TEST_CASES
int
main()
{
    CompositeMutexLatency test;

    //test.debug(true);
    test.run();
    stop_simulation();
}
#endif
