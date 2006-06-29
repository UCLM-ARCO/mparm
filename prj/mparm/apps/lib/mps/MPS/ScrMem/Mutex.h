#ifndef MPS_SCRMEM_MUTEX_H
#define MPS_SCRMEM_MUTEX_H

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

	
	class RemoteMutexState : public LocalMutexState {
	public:
	    RemoteMutexState(unsigned core,
			     unsigned id,
			     unsigned notify)
		: LocalMutexState(core, id, notify),
		  _local(semaphore_base(get_proc_id())[id]),
		  _waiting(false)
	    {
		_local = 1;	// write a 0, initially locked
		addObserver(get_proc_id());
	    }

	    ~RemoteMutexState()
	    {
		removeObserver(get_proc_id());
	    }
	    
	    bool tryLock()
	    {
		if (_waiting) {
		    if (_local) {
			_waiting = false;
			return MutexState::tryLock();
		    }
		    return false;
		}

		if (MutexState::tryLock())
		    return true;

		_waiting = true;
		return false;
	    }

	private:
	    mutable volatile int& _local;
	    bool _waiting;
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

	typedef MPS::BaseMutex<LocalMutexState> BaseLocalMutex;
	
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

	    LocalMutexState _state;
	};
	
	//---------------------------------------------------------
	// Remote semaphores spin on a local location while blocked.
	// They also notify remote cores when they unlock the
	// mutex.  Otherwise they behave as an standard Mutex
	
	typedef MPS::BaseMutex<RemoteMutexState> BaseRemoteMutex;

	class RemoteMutex: public BaseRemoteMutex
	{
	public:
	    RemoteMutex(unsigned core, unsigned id, unsigned notify)
		: BaseRemoteMutex(_state), _state(core, id, notify)
	    {
	    }
	    
	private:
	    RemoteMutexState _state;
	};

    }
    
}
#endif
