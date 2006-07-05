#ifndef MPS_SIMPLE_CHANNEL_H
#define MPS_SIMPLE_CHANNEL_H

// Simple abstractions to provide connection-oriented data transport

// One-to-one, does not need remote semaphores nor locking on the ring
// buffers.  This simplifies things a lot, we may have a single
// template implementation for every channel.

// Channels use Scratch semaphores and memories except for storage.

#include <MPS/RingBuffer.h>
#include <MPS/Queue.h>
#include <MPS/Semaphore.h>
#include <MPS/Mutex.h>

namespace MPS {

    namespace Channel {
    
	// *State classes are not meant to be used directly. They are use
	// to instantiate client-side and server-side Transceivers
    
	struct ClientTransceiverState
	{
	    volatile void* toSrv;	// RingBuffer area to server
	    volatile void* toClnt;	// RingBuffer area to client
	    volatile void* mem;		// Storage mem (shared)
	    unsigned mutex;		// Mutex for shared memory
	    unsigned srvSem;		// First sem_id on server
	    unsigned clntSem;		// First sem_id on client
	    int clnt, srv;		// Client and server proc ids
	};
    
	struct ServerTransceiverState : public ClientTransceiverState
	{
	    unsigned clntLock;	// Client is locked here
	};

	// This is a simple specialized allocator for Transceivers.
	// You may use it as a starting point to setup an optimized
	// layout for your channel-oriented applications.  Note that
	// it is configured as a parent allocator of the message area.
	// Therefore you are using a nice hierarchical allocation
	// scheme.

	template <int N, int M>
	struct TransceiverBufferData {
	    ServerTransceiverState state;
	    void* toClnt[N];
	    void* toSrv[N];
	    char  mem[M];
	};
	
	template <int MaxPendingMsg, int MsgBufferSize>
	class TransceiverAllocator
	    : public FixedSizeAllocator<TransceiverBufferData<MaxPendingMsg, MsgBufferSize>,
					MPS::ScrMem::SpinMutex,
					MaxPendingMsg>
	{
	    typedef FixedSizeAllocator<TransceiverBufferData<MaxPendingMsg, MsgBufferSize>,
				       MPS::ScrMem::SpinMutex, MaxPendingMsg> BaseAllocator;
	    typedef TransceiverBufferData<MaxPendingMsg, MsgBufferSize> TransceiverBufferData;
	    
	public:
	    TransceiverAllocator(volatile void* mem, unsigned sem, bool init = false)
		: BaseAllocator(mem, _mutex, init), _mutex(get_proc_id(), sem), _sem(sem + 1)
	    {
	    }
	    
	    volatile ServerTransceiverState* alloc()
	    {
		int i;
		volatile TransceiverBufferData* ret = BaseAllocator::alloc(i);
		ret->state.toClnt = ret->toClnt;
		ret->state.toSrv  = ret->toSrv;
		ret->state.mem    = ret->mem;
		ret->state.mutex  = _sem + 3*i;
		ret->state.srvSem = _sem + 3*i + 1;
		ret->state.srv = get_proc_id();
		return &ret->state;
	    }

	    void free(volatile ServerTransceiverState* mem)
	    {
		BaseAllocator::free(static_cast<volatile TransceiverBufferData*>(mem));
	    }

	private:
	    MPS::ScrMem::SpinMutex _mutex;
	    unsigned _sem;
	};

	//-----------------------------------------------------------------------
	// Bidirectional channel using two MPS::Queues

	// The following layout is assumed in mem.
	// ShMem: head1, tail1, head2, tail2, msgMem

	template <class T, class PeerSem, class LocalSem, class Mutex, int N>
	class Transceiver
	    : public MPS::Queue::Producer<T, DummyMutex, PeerSem, LocalSem, Mutex, N>,
	      public MPS::Queue::Consumer<T, DummyMutex, LocalSem, PeerSem, Mutex, N>
	{
	    typedef MPS::Queue::Producer<T, DummyMutex, PeerSem, LocalSem, Mutex, N> Producer;
	    typedef MPS::Queue::Consumer<T, DummyMutex, LocalSem, PeerSem, Mutex, N> Consumer;
	    
	public:
	    Transceiver(volatile ClientTransceiverState& s)
		: Producer(static_cast<volatile T* volatile *>(s.toSrv),	// ringbuf
			   static_cast<volatile int *>(s.mem) + 4,	// mem
			   *(static_cast<volatile int *>(s.mem) + 3),	// tail
			   _dummy, _peerSemP, _mySemC, _mutex, true),
		  Consumer(static_cast<volatile T* volatile *>(s.toClnt),	// ringbuf
			   static_cast<volatile int *>(s.mem) + 4,	// mem
			   *(static_cast<volatile int *>(s.mem) + 0),	// head
			   _dummy, _mySemP, _peerSemC, _mutex, true),
		  _mySemP(get_proc_id(), s.clntSem), _mySemC(get_proc_id(), s.clntSem + 1),
		  _peerSemP(s.srv, s.srvSem), _peerSemC(s.srv, s.srvSem + 1),
		  _mutex(get_proc_id(), s.mutex, true)
	    {
	    }

	    Transceiver(volatile ServerTransceiverState& s)
		: Producer(static_cast<volatile T* volatile *>(s.toClnt),	// ringbuf
			   static_cast<volatile int *>(s.mem) + 4,	// mem
			   *(static_cast<volatile int *>(s.mem) + 1),	// tail
			   _dummy, _peerSemP, _mySemC, _mutex, true),
		  Consumer(static_cast<volatile T* volatile *>(s.toSrv),	// ringbuf
			   static_cast<volatile int *>(s.mem) + 4,	// mem
			   *(static_cast<volatile int *>(s.mem) + 2),	// head
			   _dummy, _mySemP, _peerSemC, _mutex, true),
		  _mySemP(get_proc_id(), s.srvSem), _mySemC(get_proc_id(), s.srvSem + 1),
		  _peerSemP(s.clnt, s.clntSem), _peerSemC(s.clnt, s.clntSem + 1),
		  _mutex(get_proc_id(), s.mutex, true)
	    {
		// Note that although we use a specific Mutex the code
		// would work with any ScrMem Mutex since it is just used
		// for an unlock operation
		MPS::ScrMem::Mutex lock(s.clnt, s.clntLock);
		lock.unlock();		// Notifies the other end that
					// the channel is ready
	    }

	    void close()
	    {
		Producer::put(0);
		_peerSemC.init(N - 1);	// unblocks waiting writers
		_peerSemC.post();
	    }

	private:
	    LocalSem _mySemP;
	    LocalSem _mySemC;
	    PeerSem _peerSemP;
	    PeerSem _peerSemC;
	    Mutex _mutex;
	    DummyMutex _dummy;
	};


	// Information sent by client to request a connection
	struct ConnectionRequest {
	    int clnt;
	    unsigned sem;	// First sem (3 are needed)
	    ClientTransceiverState* state;
	};

	
	//-----------------------------------------------------------------------
	// Connection establishment (passive side)


	// Assumes this layout:
	// Mem: head, tail, mem
	// Sem: semP, semC, mutexP, semAlloc
	template <int N, int M>
	class Acceptor {
	    typedef MPS::RingBuffer::Consumer<ConnectionRequest,
					      DummyMutex,
					      MPS::ScrMem::SpinSemaphore,
					      MPS::ScrMem::SpinSemaphore,
					      N> IncomingConnectionBuffer;
	    typedef TransceiverAllocator<N, M> TransceiverAllocator;
	    
	public:
	    Acceptor(volatile void* mem, unsigned sem)
		: _incoming(reinterpret_cast<volatile ConnectionRequest*>(static_cast<volatile int*>(mem) + 2), // mem
			    *static_cast<volatile int*>(mem),		// head
			    _dummy, _semP, _semC),			// no mutex
		  _allocator(static_cast<volatile int*>(mem) + 2 + 3*N, sem + 3, true),
		  _semP(get_proc_id(), sem), _semC(get_proc_id(), sem + 1)
	    {
		_incoming.deactivate();	// Do not accept connections yet
	    }
	
	    void close()
	    {
		_incoming.activate();	// Unblocks waiting writers
	    }
	
	    void listen()
	    {
		_incoming.activate(); 	// Allows incoming connections
	    }

	    volatile ServerTransceiverState& accept()
	    {
		ConnectionRequest req = _incoming.get();
		volatile ServerTransceiverState* state = _allocator.alloc();

		// This is freed by the Client side.  As it is
		// currently implemented this requires a zero-state
		// allocator. Otherwise you need a way to communicate
		// the allocator data to the client (probably through
		// ClientTransceiverState)

		state->clnt     = req.clnt;
		state->clntSem  = req.sem + 1;
		state->clntLock = req.sem;

		ClientTransceiverState* toClnt = const_cast<ServerTransceiverState*>(state);
		*req.state = *toClnt;	// Communicate ClientTransceiverState
		return *state;
	    }

	private:
	    IncomingConnectionBuffer _incoming;
	    TransceiverAllocator _allocator;
	    MPS::ScrMem::SpinSemaphore _semP;
	    MPS::ScrMem::SpinSemaphore _semC;
	    DummyMutex _dummy;
	};

	
	//-----------------------------------------------------------------------
	// Connection establishment (active side)
	template <int N>
	class Connector {
	    typedef MPS::RingBuffer::Producer<ConnectionRequest,
					      MPS::ScrMem::SpinMutex,
					      MPS::ScrMem::SpinSemaphore,
					      MPS::ScrMem::SpinSemaphore,
					      N> ConnectionQueue;
	public:
	    Connector(int peer, volatile void* mem, unsigned sem, volatile void* localMem, unsigned localSem)
		: _connections(reinterpret_cast<volatile ConnectionRequest*>(static_cast<volatile int*>(mem) + 2), // mem
			       *(static_cast<volatile int*>(mem) + 1), // tail
			       _mutex, _semP, _semC),
		  _semP(peer, sem),
		  _semC(peer, sem + 1),
		  _mutex(peer, sem + 2),
		  _state(*static_cast<volatile ClientTransceiverState*>(localMem)),
		  _sem(localSem)
	    {
	    }
	
	    volatile ClientTransceiverState& connect()
	    {
		ConnectionRequest req = {
		    get_proc_id(),
		    _sem,
		    const_cast<ClientTransceiverState*>(&_state)
		};
		
		MPS::ScrMem::SpinMutex mutex(get_proc_id(), _sem, true);
		mutex.lock();
		
		pr("Sending connection request", 0, PR_CPU_ID | PR_STRING | PR_NEWL);
		_connections.put(req);	// Sends connection request

		pr("Waiting on lock", 0, PR_CPU_ID | PR_STRING | PR_NEWL);
		mutex.lock();		// Waits for incoming data (on a lock)
		return _state;
	    }
	
	private:
	    ConnectionQueue _connections;
	    MPS::ScrMem::SpinSemaphore _semP;
	    MPS::ScrMem::SpinSemaphore _semC;
	    MPS::ScrMem::SpinMutex _mutex;
	    volatile ClientTransceiverState& _state;
	    unsigned _sem;
	};
	
    }
    
}

#endif
