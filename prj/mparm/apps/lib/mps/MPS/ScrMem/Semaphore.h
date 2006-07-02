#ifndef MPS_SCRMEM_SEMAPHORE_H
#define MPS_SCRMEM_SEMAPHORE_H

//------------------------------------------------------------------------
// Semaphore implementation for scratchpad memories

// HW Resources: A single location on Scratch Memory Semaphores of a core

// Example:
//
// On core i:
//
//	MPS::ScrMem::Semaphore sem(SEM_CORE, SEM_ID);
//
//	... produce data ...
//	sem.post();
//
//
// On core j:
//
//	MPS::ScrMem::Semaphore sem(SEM_CORE, SEM_ID);
//
//	sem.wait();
//	... consume data ...

namespace MPS {

    namespace ScrMem {

	// State classes are not meant to be used directly. They are
	// just a convenience for easier adaptation of HW
	// semantics. Please see ${SWARMDIR}/core/scratch_mem.h for
	// information on the current semantics.
	
	class SemaphoreState {
	public:
	    SemaphoreState(unsigned core,     // Semaphore core
			   unsigned id,       // HW semaphore to be wrapped
			   bool init = false, // Set an initial value?
			   long value = 0)
		: _semaphore(semaphore_base(core)[id])
	    {
		if (init)
		    initialize (value);
	    }

	    void initialize (int value)
	    {
		_semaphore = value + 1;
	    }
	    
	    bool dec()
	    {
		return _semaphore != 0;
	    }
	    
	    void inc(int add = 1)
	    {
		for (int i = 0; i < add; ++i)
		    _semaphore = 0;
	    }

	    void yield()
	    {
		sleep(0);
	    }
	    
	private:
	    mutable volatile int& _semaphore;
	};

	class SpinSemaphoreState: public SemaphoreState {
	public:
	    SpinSemaphoreState(unsigned core,
			       unsigned id,
			       bool init = false,
			       long value = 0)
		: SemaphoreState(core, id, init, value)
	    {
	    }
	    
	    void yield() { }	// Just overrides yield to avoid ctx switch
	};

	// Local/Remote semaphores cooperate to avoid unneeded polling
	// traffic in the bus.  An increment of a semaphore generates
	// remote notifications to all subscribed cores.

	class LocalSemaphoreState : public SemaphoreState {
	public:
	    LocalSemaphoreState(unsigned id,       // HW semaphore
				unsigned notify,   // Addr of subscribers
				bool init = false, // Set an initial value?
				long value = 0)    // Initial value
		: SemaphoreState(get_proc_id(), id, init, value),
		  _id(id), _notify(memory_base(get_proc_id())[notify])
	    {
	    }
	    
	    void inc(int add = 1)
	    {
		SemaphoreState::inc(add);
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
	    LocalSemaphoreState(unsigned core,
				unsigned id,
				unsigned notify)
		: SemaphoreState(core, id, false),
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

	class LocalSpinSemaphoreState : public LocalSemaphoreState {
	public:
	    LocalSpinSemaphoreState(unsigned id,
				    unsigned notify,
				    bool init = false,
				    long value = 0)
		: LocalSemaphoreState(id, notify, init, value)
	    {
	    }

	    void yield()
	    {
	    }
	};
	
	// Local/Remote semaphores cooperate to avoid unneeded polling
	// traffic in the bus.  A remote semaphore spins on a local
	// lock when the semaphore is locked.

	// _retries is meant to avoid potential race conditions that
	// may arise as a consequence of non-atomic notifications
	class RemoteSemaphoreState : public LocalSemaphoreState {
	public:
	    RemoteSemaphoreState(unsigned core,
				 unsigned id,
				 unsigned notify)
		: LocalSemaphoreState(core, id, notify),
		  _local(semaphore_base(get_proc_id())[id]),
		  _retries(2), _waiting(false)
	    {
		addObserver(get_proc_id());
	    }

	    ~RemoteSemaphoreState()
	    {
		removeObserver(get_proc_id());
	    }
	    
	    bool dec()
	    {
		if (_waiting) {
		    if (_local) {
			_retries = 2;
			_waiting = false;
			return SemaphoreState::dec();
		    }
		    return false;
		}

		if (SemaphoreState::dec())
		    return true;

		if (0 == --_retries) {
		    _local = 1;		// write a 0 (locked)
		    _waiting = true;
		}
		return false;
	    }

	private:
	    mutable volatile int& _local;
	    int _retries;
	    bool _waiting;
	};

	class RemoteSpinSemaphoreState : public RemoteSemaphoreState {
	public:
	    RemoteSpinSemaphoreState(unsigned core,
				     unsigned id,
				     unsigned notify)
		: RemoteSemaphoreState(core, id, notify)
	    {
	    }

	    void yield()
	    {
	    }
	};
	
	//------------------------------------------------------------
	// POSIX style semaphores for SCRATCH MEMORY

	// The semantics of scratch semaphores fits quite nicely in
	// the POSIX style implementation.  Therefore they are far
	// more efficient than SHARED MEMORY semaphores.
	
	typedef MPS::BaseSemaphore<SemaphoreState> BaseSemaphore;

	class Semaphore: public BaseSemaphore
	{
	public:
	    Semaphore(unsigned core, unsigned id, bool init = false,
		      long value = 0)
		: BaseSemaphore(_state), _state(core, id, init, value)
	    {
	    }
	private:
	    SemaphoreState _state;
	};


	//---------------------------------------------------------
	// Local semaphores notify remote cores when a semaphore is
	// unlocked
	
	typedef MPS::BaseSemaphore<LocalSemaphoreState> BaseLocalSemaphore;

	class LocalSemaphore: public BaseLocalSemaphore
	{
	public:
	    LocalSemaphore(unsigned id, unsigned notify,
			   bool init = false, long value = 0)
		: BaseLocalSemaphore(_state),
		  _state(id, notify, init, value)
	    {
	    }
	    
	private:
	    LocalSemaphoreState _state;
	};

	
	typedef MPS::BaseSemaphore<LocalSpinSemaphoreState> BaseLocalSpinSemaphore;

	class LocalSpinSemaphore: public BaseLocalSpinSemaphore
	{
	public:
	    LocalSpinSemaphore(unsigned id, unsigned notify,
			   bool init = false, long value = 0)
		: BaseLocalSpinSemaphore(_state),
		  _state(id, notify, init, value)
	    {
	    }
	    
	private:
	    LocalSpinSemaphoreState _state;
	};


	//---------------------------------------------------------
	// Remote semaphores spin on a local location while blocked.
	// They also notify remote cores when they unblock the
	// semaphore.  Otherwise they behave as an standard Semaphore
	
	typedef MPS::BaseSemaphore<RemoteSemaphoreState> BaseRemoteSemaphore;

	class RemoteSemaphore: public BaseRemoteSemaphore
	{
	public:
	    RemoteSemaphore(unsigned core, unsigned id, unsigned notify)
		: BaseRemoteSemaphore(_state), _state(core, id, notify)
	    {
	    }
	private:
	    RemoteSemaphoreState _state;
	};

	typedef MPS::BaseSemaphore<RemoteSpinSemaphoreState> BaseRemoteSpinSemaphore;

	class RemoteSpinSemaphore: public BaseRemoteSpinSemaphore
	{
	public:
	    RemoteSpinSemaphore(unsigned core, unsigned id, unsigned notify)
		: BaseRemoteSpinSemaphore(_state), _state(core, id, notify)
	    {
	    }
	private:
	    RemoteSpinSemaphoreState _state;
	};

	//----------------------------------------------
	// Spin semaphores do not yield the CPU while waiting for a
	// locked semaphore
	
	typedef MPS::BaseSemaphore<SpinSemaphoreState> BaseSpinSemaphore;

	class SpinSemaphore: public BaseSpinSemaphore
	{
	public:
	    SpinSemaphore(unsigned core, unsigned id, bool init = false,
			  unsigned value = 0)
		: BaseSpinSemaphore(_state), _state(core, id, init, value)
	    {
	    }
	private:
	    SpinSemaphoreState _state;
	};

    }

}

#endif
