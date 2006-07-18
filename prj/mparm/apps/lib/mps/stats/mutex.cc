#include "TestCase.hh"
#include <MPS/Mutex.h>
#include <MPS/RecMutex.h>
#include <MPS/Semaphore.h>

// Meassurements of mutex latency

#define N 20

// Semaphore to signal end condition
MPS::ScrMem::Semaphore end1(1,61,get_proc_id() == 1,0);
MPS::ScrMem::Semaphore end2(2,61,get_proc_id() == 1,0);

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

extern "C" void udelay(int);

void
delay()
{
#if defined __linux__ || defined __rtems__
    sleep(0);
#else
    udelay(1);
#endif
}

template <class Mutex>
bool
MutexLatency<Mutex>::run1()
{
    stamp(11);
    for (int i = 0; i<N; ++i)  {
	time_start(i);
	{
	    typename Mutex::Lock lock(_mutex);
	    trace("lock");

	    end2.post();
	    end1.wait();

	    delay();
	    
	    time_stop(i);
	}
	trace("unlock");
	end2.post();
	end1.wait();
    }
    return true;
}

template <class Mutex>
bool
MutexLatency<Mutex>::run2()
{
    wait(11);
    stamp(0);
    for (int i = 0; i<N; ++i)  {
	end1.post();
	end2.wait();
	{
	    time_start(i);
	    typename Mutex::Lock lock(_mutex);
	    time_stop(i);
	    trace("lock");
	}
	trace("unlock");
	end1.post();
	end2.wait();
    }
    return true;
}


typedef MutexLatency<MPS::ShMem::Mutex> M1ShMutex;
typedef MutexLatency<MPS::ShMem::SpinMutex> M1ShSpinMutex;

typedef MutexLatency<MPS::ScrMem::Mutex> M1ScrMutex;
typedef MutexLatency<MPS::ScrMem::SpinMutex> M1ScrSpinMutex;
typedef MutexLatency<MPS::ScrMem::RemoteMutex> M1ScrRemoteMutex;
typedef MutexLatency<MPS::ScrMem::LocalMutex> M1ScrLocalMutex;

#ifdef __rtems__
typedef MutexLatency<MPS::ScrMem::RemoteSMutex> M1ScrRemoteSMutex;
typedef MutexLatency<MPS::ScrMem::LocalSMutex> M1ScrLocalSMutex;
#endif

typedef MutexLatency<MPS::ScrMem::RemoteSpinMutex> M1ScrRemoteSpinMutex;
typedef MutexLatency<MPS::ScrMem::LocalSpinMutex> M1ScrLocalSpinMutex;

typedef MutexLatency<MPS::ShMem::RecMutex> M1ShRecMutex;
typedef MutexLatency<MPS::ShMem::SpinRecMutex> M1ShSpinRecMutex;

typedef MutexLatency<MPS::ScrMem::RecMutex> M1ScrRecMutex;
typedef MutexLatency<MPS::ScrMem::SpinRecMutex> M1ScrSpinRecMutex;
typedef MutexLatency<MPS::ScrMem::RemoteRecMutex> M1ScrRemoteRecMutex;
typedef MutexLatency<MPS::ScrMem::LocalRecMutex> M1ScrLocalRecMutex;

#ifdef __rtems__
typedef MutexLatency<MPS::ScrMem::RemoteSRecMutex> M1ScrRemoteSRecMutex;
typedef MutexLatency<MPS::ScrMem::LocalSRecMutex> M1ScrLocalSRecMutex;
#endif

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
#ifdef __rtems__
    MPS::ScrMem::LocalSMutex* _scrLocalSMutex;
    MPS::ScrMem::RemoteSMutex* _scrRemoteSMutex;
#endif
    MPS::ScrMem::LocalSpinMutex* _scrLocalSpinMutex;
    MPS::ScrMem::RemoteSpinMutex* _scrRemoteSpinMutex;
    MPS::ScrMem::LocalRecMutex* _scrLocalRecMutex;
    MPS::ScrMem::RemoteRecMutex* _scrRemoteRecMutex;
#ifdef __rtems__
    MPS::ScrMem::LocalSRecMutex* _scrLocalSRecMutex;
    MPS::ScrMem::RemoteSRecMutex* _scrRemoteSRecMutex;
#endif
    MPS::ScrMem::LocalSpinRecMutex* _scrLocalSpinRecMutex;
    MPS::ScrMem::RemoteSpinRecMutex* _scrRemoteSpinRecMutex;

    enum {
	ID = 10,
	SpinID,
	RecID,
	SpinRecID,
	RemoteID,
	RemoteSID,
	RemoteSpinID,
	RemoteRecID,
	RemoteSRecID,
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
#ifdef __rtems__
	  _scrLocalSMutex(0),
	  _scrRemoteSMutex(0),
#endif
	  _scrLocalSpinMutex(0),
	  _scrRemoteSpinMutex(0),
	  _scrLocalRecMutex(0),
	  _scrRemoteRecMutex(0),
#ifdef __rtems__
	  _scrLocalSRecMutex(0),
	  _scrRemoteSRecMutex(0),
#endif
	  _scrLocalSpinRecMutex(0),
	  _scrRemoteSpinRecMutex(0)
    {

	add(new M1ShMutex(_shMutex,"ShMem::Mutex"));
	add(new M1ShSpinMutex(_shSpinMutex, "ShMem::SpinMutex"));
	add(new M1ScrMutex(_scrMutex, "ScrMem::Mutex"));
	add(new M1ScrSpinMutex(_scrSpinMutex, "ScrMem::SpinMutex"));
	add(new M1ShRecMutex(_shRecMutex, "ShMem::RecMutex"));
	add(new M1ShSpinRecMutex(_shSpinRecMutex, "ShMem::SpinRecMutex"));
	add(new M1ScrRecMutex(_scrRecMutex, "ScrMem::RecMutex"));
	add(new M1ScrSpinRecMutex(_scrSpinRecMutex, "ScrMem::SpinRecMutex"));

	if (1 == get_proc_id()) {
	    _scrLocalMutex        = new MPS::ScrMem::LocalMutex(RemoteID, RemoteID, true);
#ifdef __rtems__
	    _scrLocalSMutex       = new MPS::ScrMem::LocalSMutex(RemoteSID, RemoteSID, true);
#endif
	    _scrLocalSpinMutex    = new MPS::ScrMem::LocalSpinMutex(RemoteSpinID, RemoteSpinID, true);
	    _scrLocalRecMutex     = new MPS::ScrMem::LocalRecMutex(RemoteRecID, RemoteRecID, true);
#ifdef __rtems__
	    _scrLocalSRecMutex    = new MPS::ScrMem::LocalSRecMutex(RemoteSRecID, RemoteSRecID, true);
#endif
	    _scrLocalSpinRecMutex = new MPS::ScrMem::LocalSpinRecMutex(RemoteSpinRecID, RemoteSpinRecID, true);
	    
	    add(new M1ScrLocalMutex(*_scrLocalMutex, "ScrMem::LocalMutex"));
#ifdef __rtems__
	    add(new M1ScrLocalSMutex(*_scrLocalSMutex, "ScrMem::LocalSMutex"));
#endif
	    add(new M1ScrLocalSpinMutex(*_scrLocalSpinMutex, "ScrMem::LocalSpinMutex"));
	    add(new M1ScrLocalRecMutex(*_scrLocalRecMutex, "ScrMem::LocalRecMutex"));
#ifdef __rtems__
	    add(new M1ScrLocalSRecMutex(*_scrLocalSRecMutex, "ScrMem::LocalSRecMutex"));
#endif
	    add(new M1ScrLocalSpinRecMutex(*_scrLocalSpinRecMutex, "ScrMem::LocalSpinRecMutex"));
	}
	else {
	    _scrRemoteMutex        = new MPS::ScrMem::RemoteMutex(1, RemoteID, RemoteID);
#ifdef __rtems__
	    _scrRemoteSMutex       = new MPS::ScrMem::RemoteSMutex(1, RemoteSID, RemoteSID);
#endif
	    _scrRemoteSpinMutex    = new MPS::ScrMem::RemoteSpinMutex(1, RemoteSpinID, RemoteSpinID);
	    _scrRemoteRecMutex     = new MPS::ScrMem::RemoteRecMutex(1, RemoteRecID, RemoteRecID);
#ifdef __rtems__
	    _scrRemoteSRecMutex    = new MPS::ScrMem::RemoteSRecMutex(1, RemoteSRecID, RemoteSRecID);
#endif
	    _scrRemoteSpinRecMutex = new MPS::ScrMem::RemoteSpinRecMutex(1, RemoteSpinRecID, RemoteSpinRecID);
	    
	    add(new M1ScrRemoteMutex(*_scrRemoteMutex, "ScrMem::RemoteMutex"));
#ifdef __rtems__
	    add(new M1ScrRemoteSMutex(*_scrRemoteSMutex, "ScrMem::RemoteSMutex"));
#endif
	    add(new M1ScrRemoteSpinMutex(*_scrRemoteSpinMutex, "ScrMem::RemoteSpinMutex"));
	    add(new M1ScrRemoteRecMutex(*_scrRemoteRecMutex, "ScrMem::RemoteRecMutex"));
#ifdef __rtems__
	    add(new M1ScrRemoteSRecMutex(*_scrRemoteSRecMutex, "ScrMem::RemoteSRecMutex"));
#endif
	    add(new M1ScrRemoteSpinRecMutex(*_scrRemoteSpinRecMutex, "ScrMem::RemoteSpinRecMutex"));
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
