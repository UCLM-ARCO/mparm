#ifndef MPS_SCRMEM_REC_MUTEX_H
#define MPS_SCRMEM_REC_MUTEX_H

namespace MPS {

    namespace ScrMem {

	//----------------------------------------------
	// Recursive mutexes using SCRATCH MEMORY 
    
	typedef typename MPS::BaseRecMutex<MutexState> BaseRecMutex;
	
	class RecMutex: public BaseRecMutex
	{
	    friend class Cond;

	public:
	    RecMutex(unsigned core, unsigned id, bool init = false)
		: _state(core, id, init), BaseRecMutex(_state)
	    {
	    }

	private:
	    RecMutex(const RecMutex&);		// disallow copy ctor
	    void operator=(const RecMutex&);	// disallow assignment operator

	    MutexState _state;
	};
	
	//----------------------------------------------
	// Spin mutexes do not yield the CPU 
    
	typedef typename MPS::BaseRecMutex<SpinMutexState> BaseSpinRecMutex;
	
	class SpinRecMutex: public BaseSpinRecMutex
	{
	    friend class Cond;

	public:
	    SpinRecMutex(unsigned core, unsigned id, bool init = false)
		: _state(core, id, init), BaseSpinRecMutex(_state)
	    {
	    }

	private:
	    SpinRecMutex(const SpinRecMutex&);	// disallow copy ctor
	    void operator=(const SpinRecMutex&);	// disallow assignment operator

	    SpinMutexState _state;
	};

	//---------------------------------------------------------
	// Local/Remote mutexes cooperate to avoid unneeded polling
	// traffic in the bus.  An unlock of a mutex generates remote
	// notifications to all subscribed cores.

	typedef typename MPS::BaseRecMutex<LocalMutexState> BaseLocalRecMutex;
	
	class LocalRecMutex: public BaseLocalRecMutex {
	public:
	    LocalRecMutex(unsigned id, unsigned notify, bool init = false)
		: _state(id, notify, init),
		  BaseLocalRecMutex(_state)
	    {
	    }

	private:
	    LocalRecMutex(const LocalRecMutex&);	// disallow copy ctor
	    void operator=(const LocalRecMutex&);	// disallow assignment operator

	    LocalMutexState _state;
	};
	
	//---------------------------------------------------------
	// Remote semaphores spin on a local location while blocked.
	// They also notify remote cores when they unlock the
	// mutex.  Otherwise they behave as an standard RecMutex
	
	typedef typename MPS::BaseRecMutex<RemoteMutexState>
	BaseRemoteRecMutex;

	class RemoteRecMutex: public BaseRemoteRecMutex
	{
	public:
	    RemoteRecMutex(unsigned core, unsigned id, unsigned notify)
		: _state(core, id, notify), BaseRemoteRecMutex(_state)
	    {
	    }
	    
	private:
	    RemoteMutexState _state;
	};

    }

}
#endif
