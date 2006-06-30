#ifndef MPS_SCRMEM_REC_MUTEX_H
#define MPS_SCRMEM_REC_MUTEX_H

namespace MPS {

    namespace ScrMem {

	//----------------------------------------------
	// Recursive mutexes using SCRATCH MEMORY 
    
	typedef MPS::BaseRecMutex<MutexState> BaseRecMutex;
	
	class RecMutex: public BaseRecMutex
	{
	    friend class Cond;

	public:
	    RecMutex(unsigned core, unsigned id, bool init = false)
		: BaseRecMutex(_state), _state(core, id, init)
	    {
	    }

	private:
	    RecMutex(const RecMutex&);		// disallow copy ctor
	    void operator=(const RecMutex&);	// disallow assignment operator

	    MutexState _state;
	};
	
	//----------------------------------------------
	// Spin mutexes do not yield the CPU 
    
	typedef MPS::BaseRecMutex<SpinMutexState> BaseSpinRecMutex;
	
	class SpinRecMutex: public BaseSpinRecMutex
	{
	    friend class Cond;

	public:
	    SpinRecMutex(unsigned core, unsigned id, bool init = false)
		: BaseSpinRecMutex(_state), _state(core, id, init)
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

	typedef MPS::BaseRecMutex<LocalMutexState> BaseLocalRecMutex;
	
	class LocalRecMutex: public BaseLocalRecMutex {
	public:
	    LocalRecMutex(unsigned id, unsigned notify, bool init = false)
		: BaseLocalRecMutex(_state), _state(id, notify, init)
	    {
	    }

	private:
	    LocalRecMutex(const LocalRecMutex&);	// disallow copy ctor
	    void operator=(const LocalRecMutex&);	// disallow assignment operator

	    LocalMutexState _state;
	};

	
	typedef MPS::BaseRecMutex<LocalSpinMutexState> BaseLocalSpinRecMutex;
	
	class LocalSpinRecMutex: public BaseLocalSpinRecMutex {
	public:
	    LocalSpinRecMutex(unsigned id, unsigned notify, bool init = false)
		: BaseLocalSpinRecMutex(_state), _state(id, notify, init)
	    {
	    }

	private:
	    LocalSpinRecMutex(const LocalSpinRecMutex&);	// disallow copy ctor
	    void operator=(const LocalSpinRecMutex&);	// disallow assignment operator

	    LocalSpinMutexState _state;
	};
	
	//---------------------------------------------------------
	// Remote semaphores spin on a local location while blocked.
	// They also notify remote cores when they unlock the
	// mutex.  Otherwise they behave as an standard RecMutex
	
	typedef MPS::BaseRecMutex<RemoteMutexState> BaseRemoteRecMutex;

	class RemoteRecMutex: public BaseRemoteRecMutex
	{
	public:
	    RemoteRecMutex(unsigned core, unsigned id, unsigned notify)
		: BaseRemoteRecMutex(_state), _state(core, id, notify)
	    {
	    }
	    
	private:
	    RemoteMutexState _state;
	};

	
	typedef MPS::BaseRecMutex<RemoteSpinMutexState> BaseRemoteSpinRecMutex;

	class RemoteSpinRecMutex: public BaseRemoteSpinRecMutex
	{
	public:
	    RemoteSpinRecMutex(unsigned core, unsigned id, unsigned notify)
		: BaseRemoteSpinRecMutex(_state), _state(core, id, notify)
	    {
	    }
	    
	private:
	    RemoteSpinMutexState _state;
	};

    }

}
#endif
