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

	typedef MPS::BaseRecMutex<LocalMutexState<MutexState> > BaseLocalRecMutex;
	
	class LocalRecMutex: public BaseLocalRecMutex {
	public:
	    LocalRecMutex(unsigned id, unsigned notify, bool init = false)
		: BaseLocalRecMutex(_state), _state(id, notify, init)
	    {
	    }

	private:
	    LocalRecMutex(const LocalRecMutex&);	// disallow copy ctor
	    void operator=(const LocalRecMutex&);	// disallow assignment operator

	    LocalMutexState<MutexState> _state;
	};

	
#ifdef __rtems__
	typedef MPS::BaseRecMutex<LocalMutexState<SMutexState> > BaseLocalSRecMutex;
	
	class LocalSRecMutex: public BaseLocalSRecMutex {
	public:
	    LocalSRecMutex(unsigned id, unsigned notify, bool init = false)
		: BaseLocalSRecMutex(_state), _state(id, notify, init)
	    {
	    }

	private:
	    LocalSRecMutex(const LocalSRecMutex&);	// disallow copy ctor
	    void operator=(const LocalSRecMutex&);	// disallow assignment operator

	    LocalMutexState<SMutexState> _state;
	};
#endif
	
	typedef MPS::BaseRecMutex<LocalMutexState<SpinMutexState> > BaseLocalSpinRecMutex;
	
	class LocalSpinRecMutex: public BaseLocalSpinRecMutex {
	public:
	    LocalSpinRecMutex(unsigned id, unsigned notify, bool init = false)
		: BaseLocalSpinRecMutex(_state), _state(id, notify, init)
	    {
	    }

	private:
	    LocalSpinRecMutex(const LocalSpinRecMutex&);	// disallow copy ctor
	    void operator=(const LocalSpinRecMutex&);	// disallow assignment operator

	    LocalMutexState<SpinMutexState> _state;
	};
	
	//---------------------------------------------------------
	// Remote semaphores spin on a local location while blocked.
	// They also notify remote cores when they unlock the
	// mutex.  Otherwise they behave as an standard RecMutex
	
	typedef MPS::BaseRecMutex<RemoteMutexState<MutexState> > BaseRemoteRecMutex;

	class RemoteRecMutex: public BaseRemoteRecMutex
	{
	public:
	    RemoteRecMutex(unsigned core, unsigned id, unsigned notify)
		: BaseRemoteRecMutex(_state), _state(core, id, notify)
	    {
	    }
	    
	private:
	    RemoteMutexState<MutexState> _state;
	};

	
#ifdef __rtems__
	typedef MPS::BaseRecMutex<RemoteMutexState<SMutexState> > BaseRemoteSRecMutex;

	class RemoteSRecMutex: public BaseRemoteSRecMutex
	{
	public:
	    RemoteSRecMutex(unsigned core, unsigned id, unsigned notify)
		: BaseRemoteSRecMutex(_state), _state(core, id, notify)
	    {
	    }
	    
	private:
	    RemoteMutexState<SMutexState> _state;
	};
#endif
	
	
	typedef MPS::BaseRecMutex<RemoteMutexState<SpinMutexState> > BaseRemoteSpinRecMutex;

	class RemoteSpinRecMutex: public BaseRemoteSpinRecMutex
	{
	public:
	    RemoteSpinRecMutex(unsigned core, unsigned id, unsigned notify)
		: BaseRemoteSpinRecMutex(_state), _state(core, id, notify)
	    {
	    }
	    
	private:
	    RemoteMutexState<SpinMutexState> _state;
	};

    }

}
#endif
