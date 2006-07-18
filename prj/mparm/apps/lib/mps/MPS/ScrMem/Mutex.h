#ifndef MPS_SCRMEM_MUTEX_H
#define MPS_SCRMEM_MUTEX_H

#include <MPS/ScrMem/Suspend.h>

namespace MPS {

    namespace ScrMem {

	// State classes are not meant to be used directly. They are
	// just a convenience for easier adaptation of HW
	// semantics. Please see ${SWARMDIR}/core/scratch_mem.h for
	// information on the current semantics.

	class MutexState
	{
	public:
 	    MutexState(unsigned core, unsigned id, bool init = false)
		: _semaphore(semaphore_base(core)[id])
	    {
		if (init) {
		    _semaphore = 2;	// init to 1 (unlocked)
		}
	    }

	    bool tryLock() const
	    {
		return (1 == _semaphore);
	    }

	    void unlock() const
	    {
		_semaphore = 2;		// set to 1 (unlocked)
	    }

	    void yield() const
	    {
		sleep(0);
	    }

	    bool willUnlock() const
	    {
		return true;	// non-recursive mutexes will always unlock
	    }
	    
	private:
	    MutexState(const MutexState&);	// disallow copy ctor
	    void operator=(const MutexState&);	// disallow assignment operator

	    volatile int& _semaphore;
	};


	class SpinMutexState: public MutexState
	{
	public:
	    SpinMutexState(unsigned core, unsigned id, bool init = false)
		: MutexState(core, id, init) {}
	    
	    void yield() const
	    {
	    }
	};
	

#ifdef __rtems__
	// Suspensive mutexes (platform specific)
	class SMutexState: public MutexState, public Suspend {
	public:
	    SMutexState(unsigned core,
			unsigned id,
			bool init = false)
		: MutexState(core, id, init)
	    {
	    }
	    
	    void yield()
	    {
		Suspend::yield();
	    }
	};
#endif

	// Local/Remote semaphores cooperate to avoid unneeded polling
	// traffic in the bus.  An increment of a semaphore generates
	// remote notifications to all subscribed cores.

	template <class MutexState>
	class LocalMutexState: public MutexState
	{
	public:
	    LocalMutexState(unsigned id,       // HW semaphore
			    unsigned notify,   // Addr of subscribers
			    bool init = false) // Initialize?
		: MutexState(get_proc_id(), id, init),
		  _id(id), _notify(memory_base(get_proc_id())[notify])
	    {
	    }
	    
	    void unlock() const
	    {
		MutexState::unlock();
		unsigned notify = _notify;
		for (int i = 0; i < 32; ++i) {
		    if (notify == 0)
			break;
		    if (notify & 1) {
			semaphore_base(i + 1)[_id] = 2;
		    }
		    notify = notify >> 1;
		}
	    }

	protected:
	    LocalMutexState(unsigned core,
			    unsigned id,
			    unsigned notify)
		: MutexState(core, id, false),
		  _id(id), _notify(memory_base(core)[notify])
	    {
	    }

	    void addObserver(unsigned id)
	    {
		_notify |= (1 << (id - 1));
	    }

	    void removeObserver(unsigned id)
	    {
		_notify &= ~(1 << (id - 1));
	    }

	private:
	    unsigned _id;
	    volatile int& _notify;
	};

	
	template <class MutexState>
	class RemoteMutexState : public LocalMutexState<MutexState> {
	public:
	    RemoteMutexState(unsigned core,
			     unsigned id,
			     unsigned notify)
		: LocalMutexState<MutexState>(core, id, notify),
		  _local(semaphore_base(get_proc_id())[id]),
		  _retries(2), _waiting(false)
	    {
		LocalMutexState<MutexState>::addObserver(get_proc_id());
	    }

	    ~RemoteMutexState()
	    {
		LocalMutexState<MutexState>::removeObserver(get_proc_id());
	    }
	    
	    bool tryLock()
	    {
		if (_waiting) {
		    if (_local) {
			_retries = 2;
			_waiting = false;
			return MutexState::tryLock();
		    }
		    return false;
		}

		_local = 1;	// write a 0
		while (_local);	// activate suspend_flag

		if (MutexState::tryLock())
		    return true;

		if (0 == --_retries)
		    _waiting = true;

		return false;
	    }

	private:
	    mutable volatile int& _local;
	    int _retries;
	    bool _waiting;
	};
	
	
	//-----------------------------------------------
	// Non-recursive mutexes using SCRATCH MEMORY 

	typedef MPS::BaseMutex<MutexState> BaseMutex;
	
	class Mutex: public BaseMutex
	{
	    friend class Cond;

	public:
	    Mutex(unsigned core, unsigned id, bool init = false)
		: BaseMutex(_state), _state(core, id, init)
	    {
	    }

	private:
	    Mutex(const Mutex&);		// disallow copy ctor
	    void operator=(const Mutex&);	// disallow assignment operator

	    MutexState _state;
	};
	
	//----------------------------------------------
	// Spin mutexes do not yield the CPU 
    
	typedef MPS::BaseMutex<SpinMutexState> BaseSpinMutex;
	
	class SpinMutex: public BaseSpinMutex
	{
	    friend class Cond;

	public:
	    SpinMutex(unsigned core, unsigned id, bool init = false)
		: BaseSpinMutex(_state), _state(core, id, init)
	    {
	    }

	private:
	    SpinMutex(const SpinMutex&);	// disallow copy ctor
	    void operator=(const SpinMutex&);	// disallow assignment operator

	    SpinMutexState _state;
	};

	//---------------------------------------------------------
	// Local/Remote mutexes cooperate to avoid unneeded polling
	// traffic in the bus.  An unlock of a mutex generates remote
	// notifications to all subscribed cores.

	typedef MPS::BaseMutex<LocalMutexState<MutexState> > BaseLocalMutex;
	
	class LocalMutex: public BaseLocalMutex {
	public:
	    LocalMutex(unsigned id, unsigned notify, bool init = false)
		: BaseLocalMutex(_state),
		  _state(id, notify, init)
	    {
	    }

	private:
	    LocalMutex(const LocalMutex&);	// disallow copy ctor
	    void operator=(const LocalMutex&);	// disallow assignment operator

	    LocalMutexState<MutexState> _state;
	};

	
#ifdef __rtems__
	typedef MPS::BaseMutex<LocalMutexState<SMutexState> > BaseLocalSMutex;
	
	class LocalSMutex: public BaseLocalSMutex {
	public:
	    LocalSMutex(unsigned id, unsigned notify, bool init = false)
		: BaseLocalSMutex(_state),
		  _state(id, notify, init)
	    {
	    }

	private:
	    LocalSMutex(const LocalSMutex&);	// disallow copy ctor
	    void operator=(const LocalSMutex&);	// disallow assignment operator

	    LocalMutexState<SMutexState> _state;
	};
#endif
	
	
	typedef MPS::BaseMutex<LocalMutexState<SpinMutexState> > BaseLocalSpinMutex;
	
	class LocalSpinMutex: public BaseLocalSpinMutex {
	public:
	    LocalSpinMutex(unsigned id, unsigned notify, bool init = false)
		: BaseLocalSpinMutex(_state),
		  _state(id, notify, init)
	    {
	    }

	private:
	    LocalSpinMutex(const LocalSpinMutex&);	// disallow copy ctor
	    void operator=(const LocalSpinMutex&);	// disallow assignment operator

	    LocalMutexState<SpinMutexState> _state;
	};
	
	//---------------------------------------------------------
	// Remote semaphores spin on a local location while blocked.
	// They also notify remote cores when they unlock the
	// mutex.  Otherwise they behave as an standard Mutex
	
	typedef MPS::BaseMutex<RemoteMutexState<MutexState> > BaseRemoteMutex;

	class RemoteMutex: public BaseRemoteMutex
	{
	public:
	    RemoteMutex(unsigned core, unsigned id, unsigned notify)
		: BaseRemoteMutex(_state), _state(core, id, notify)
	    {
	    }
	    
	private:
	    RemoteMutexState<MutexState> _state;
	};

	
#ifdef __rtems__
	typedef MPS::BaseMutex<RemoteMutexState<SMutexState> > BaseRemoteSMutex;

	class RemoteSMutex: public BaseRemoteSMutex
	{
	public:
	    RemoteSMutex(unsigned core, unsigned id, unsigned notify)
		: BaseRemoteSMutex(_state), _state(core, id, notify)
	    {
	    }
	    
	private:
	    RemoteMutexState<SMutexState> _state;
	};
#endif
	
	
	typedef MPS::BaseMutex<RemoteMutexState<SpinMutexState> > BaseRemoteSpinMutex;

	class RemoteSpinMutex: public BaseRemoteSpinMutex
	{
	public:
	    RemoteSpinMutex(unsigned core, unsigned id, unsigned notify)
		: BaseRemoteSpinMutex(_state), _state(core, id, notify)
	    {
	    }
	    
	private:
	    RemoteMutexState<SpinMutexState> _state;
	};

    }
    
}
#endif
