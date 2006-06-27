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

	// Local/Remote semaphores cooperate to avoid unneeded polling
	// traffic in the bus.  An increment of a blocked semaphore
	// generates remote notifications to all subscribed cores.
	
	class LocalSemaphoreState : public SemaphoreState {
	public:
	    LocalSemaphoreState(unsigned id,       // HW semaphore
				unsigned notify,   // Addr of subscribers
				bool init = false, // Set an initial value?
				long value = 0)    // Initial value
		: _id(id), _notify(memory_base(get_cpu_id())[notify]),
		  SemaphoreState(get_cpu_id(), id, init, value)
	    {
	    }
	    
	    void inc(int add = 1)
	    {
		if (SemaphoreState::dec()) {
		    SemaphoreState::inc(2);
		}
		else {  // will unblock, notify all
		    unsigned notify = _notify;
		    for (int i = 0; i < 32; ++i) {
			if (notify == 0)
			    break;
			if (notify & 1)
			    semaphore_base(i + 1)[_id] = 2;
			notify = notify >> 1;
		    }
		}
	    }

	protected:
	    LocalSemaphoreState(unsigned core,
				unsigned id,
				unsigned notify)
		: _notify(memory_base(core)[notify]),
		  SemaphoreState(core, id, false)
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
	    volatile unsigned int& _notify;
	};

	// Local/Remote semaphores cooperate to avoid unneeded polling
	// traffic in the bus.  A remote semaphore spins on a local
	// lock when the semaphore is locked.
	
	class RemoteSemaphoreState : public LocalSemaphoreState {
	public:
	    RemoteSemaphoreState(unsigned core,
				 unsigned id,
				 unsigned notify)
		: _local(semaphore_base(get_cpu_id())[id]),
		  _waiting(false),
		  LocalSemaphoreState(core, id, notify)
	    {
		_local = 1;	// write a 0, initially locked
		addObserver(get_cpu_id());
	    }

	    ~RemoteSemaphoreState()
	    {
		removeObserver(get_cpu_id());
	    }
	    
	    bool dec()
	    {
		if (_waiting) {
		    if (_local)
			return SemaphoreState::dec();
		    return false;
		}

		if (SemaphoreState::dec())
		    return true;

		_waiting = true;
		return false;
	    }

	private:
	    mutable volatile int& _local;
	    bool _waiting;
	};
	
	// This class is not meant to be used directly. It is just a
	// convenience for easier adaptation of HW semantics.
	class SpinSemaphoreState: public SemaphoreState {
	public:
	    void yield() { }	// Just overrides yield to avoid ctx switch
	};

	
	//------------------------------------------------------------
	// POSIX style semaphores for SCRATCH MEMORY

	// The semantics of scratch semaphores fits quite nicely in
	// the POSIX style implementation.  Therefore they are far
	// more efficient than SHARED MEMORY semaphores.
	
	typedef typename MPS::BaseSemaphore<SemaphoreState> BaseSemaphore;

	class Semaphore: public BaseSemaphore
	{
	public:
	    Semaphore(unsigned core, unsigned id, bool init = false,
		      long value = 0)
		: _state(core, id, init, value), BaseSemaphore(_state)
	    {
	    }
	private:
	    SemaphoreState _state;
	};


	//---------------------------------------------------------
	// Local semaphores notify remote cores when a semaphore is
	// unlocked
	
	typedef typename MPS::BaseSemaphore<LocalSemaphoreState>
	BaseLocalSemaphore;

	class LocalSemaphore: public BaseLocalSemaphore
	{
	public:
	    LocalSemaphore(unsigned id, unsigned notify,
			   bool init = false, long value = 0)
		: _state(id, notify, init, value),
		  BaseLocalSemaphore(_state)
	    {
	    }
	    
	    void notify (unsigned notify)
	    {
		_state.notify(notify);
	    }
	    
	private:
	    LocalSemaphoreState _state;
	};

	
	//---------------------------------------------------------
	// Remote semaphores spin on a local location while blocked.
	// They also notify remote cores when they unblock the
	// semaphore.  Otherwise they behave as an standard Semaphore
	
	typedef typename MPS::BaseSemaphore<RemoteSemaphoreState>
	BaseRemoteSemaphore;

	class RemoteSemaphore: public BaseRemoteSemaphore
	{
	public:
	    RemoteSemaphore(unsigned core, unsigned id, unsigned notify)
		: _state(core, id, notify), BaseRemoteSemaphore(_state)
	    {
	    }
	private:
	    RemoteSemaphoreState _state;
	};

	//----------------------------------------------
	// Spin semaphores do not yield the CPU while waiting for a
	// locked semaphore
	
	typedef typename MPS::BaseSemaphore<SpinSemaphoreState>
	BaseSpinSemaphore;

	class SpinSemaphore: public BaseSpinSemaphore
	{
	public:
	    SpinSemaphore(unsigned core, unsigned id, bool init = false,
			  long value = 0)
		: _state(core, id, init, value), BaseSpinSemaphore(_state)
	    {
	    }
	private:
	    SpinSemaphoreState _state;
	};

    }

}

#endif
