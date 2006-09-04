// -*- C++ -*-
#ifndef MPS_CHANNEL_H
#define MPS_CHANNEL_H

/*! \file MPS/Channel.h

  \brief Simple abstractions for connection-oriented data transport.

  Channels are meant for dynamic one-to-one communication.  They do
  not need remote semaphores nor locking on the ring buffers.  This
  simplifies things a lot, we may have a single template
  implementation for every channel.

  Channel connection management use scratch semaphores and scratch
  memory for synchronization and locking.  Data storage may be located
  in any addressable memory.

  Channels are instantiated at the server side as a response to a
  client-side request.  Clients use a Connector to request a new
  channel and servers wait for connections using an Acceptor.  The
  acceptor/connector abstractions are modeled after the
  acceptor/connector design pattern first proposed by Douglas Schmidt
  et al.

  Both connectors and acceptors behave as channel allocators.  They
  return a transceiver to the caller.  A transceiver provides methods
  for two-way data communication with a peer transceiver.  In order to
  handle data efficiently we use a shared memory area for data storage
  and a ring-buffer to pass references to the peer.
*/

#include <MPS/RingBuffer.h>
#include <MPS/Queue.h>
#include <MPS/Semaphore.h>
#include <MPS/Mutex.h>

namespace MPS {

    namespace Channel {

	//! \brief Holds internal state of client-side transceiver.
	//!
	//! This class is not meant to be used directly. It is used to
	//! instantiate client-side Transceivers. 
    	struct ClientTransceiverState
	{
	    //! \brief RingBuffer area used to transmit references to
	    //! the server.
	    volatile void* toSrv;

	    //! \brief RingBuffer area used to transmit references to
	    //! the client.
	    volatile void* toClnt;
	    
	    //! \brief Shared storage area for data allocation.
	    volatile void* mem;

	    //! \brief Guarantees mutual exclussion when accessing the
	    //shared memory area.
	    unsigned mutex;

	    //! \brief First semaphore identifier used by the server.
	    unsigned srvSem;
	    
	    //! \brief First semaphore identifier used by the client.
	    unsigned clntSem;
	    
	    //! \brief Client and server process identifiers.
	    int clnt, srv;
	};
    
	//! \brief Holds internal state of server-side transceiver.
	//!
	//! This class is not meant to be used directly. It is used to
	//! instantiate server-side Transceivers.  The only difference
	//! with respect to the client-side transceiver is an extra
	//! synchronization point used as a rendezvous for
	//! client/server transceiver setup.  Connector/acceptor
	//! rendezvous is implemented using a simple lock.
	struct ServerTransceiverState : public ClientTransceiverState
	{
	    //! \brief Address of a scratch semaphore at the
	    //! client-side.
	    //!
	    //! When a client requests a connection it waits here.
	    //! Client side may use ScrMem::Mutex or ScrMem::SpinMutex.
	    //! 
	    unsigned clntLock;
	};

	//! \brief Data required for a message allocator in a given
	//! transceiver.
	//!
	//! Holds data required to instantiate a message allocator for
	//! a given transceiver.  The transceiver allocator uses this
	//! struct to provide a hierarchical allocation scheme.
	//! See \ref TransceiverAllocator for details.
	template <int N, int M>
	struct TransceiverBufferData {

	    //! \brief Internal state of the transceiver at the server
	    //! side.
	    ServerTransceiverState state;

	    //! \brief Ring-buffer storage area used to transmit
	    //! message references to the client.
	    void* toClnt[N];
	    
	    //! \brief Ring-buffer storage area used to transmit
	    //! message references to the server.
	    void* toSrv[N];
	    
	    //! \brief Message area.
	    char  mem[M];
	};
	
	//! \brief Simple specialized allocator for Transceivers.
	//!
	//! This class may be used as a starting point to setup an
	//! optimized layout for each channel-oriented applications.
	//!
	//! Note that it is configured as a parent allocator of the
	//! message area.  It is a FixedSizeAllocator whose items are
	//! message areas.  Therefore you are using a nice hierarchical
	//! allocation scheme.
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

	    //! \brief Common allocator constructor for transceivers.
	    //!
	    //! It mimics the behaviour of \ref FixedSizeAllocator
	    //! constructor.
	    //!
	    //! \param mem Storage area for transceiver allocator.
	    //!
	    //! \param sem First available scratch semaphore for this
	    //! transceiver allocator.  The allocator consumes just one
	    //! scratch semaphore for the internal Mutex.  Each
	    //! allocated transceiver consumes three additional scratch
	    //! semaphores.
	    //!
	    //! \param init Whether this allocator should be
	    //! initialized or not.  The same allocator may be
	    //! instantiated on different threads (or even on different
	    //! processors) but there should always be just one
	    //! instance with this parameter set to true.  All other
	    //! instances should only be used once the first instance
	    //! is fully intitialized.
	    //!
	    TransceiverAllocator(volatile void* mem, unsigned sem, bool init = false)
		: BaseAllocator(mem, _mutex, init), _mutex(get_proc_id(), sem), _sem(sem + 1)
	    {
	    }

	    //! \brief Allocates a ServerTransceiverState.
	    //!
	    //! This allocator does not use dynamic memory.  The
	    //! returned data is allocated from the internal shared
	    //! memory storage area.
	    //!
	    //! \returns Returns a ServerTransceiverState that can be
	    //! used to instantiate a server-side Transceiver.
	    //!
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

	    //! \brief Marks a ServerTransceiverState as available.
	    //!
	    //! Makes a ServerTransceiverState available for other
	    //! threads or processors.
	    //!
	    //! \param mem A ServerTransceiverState previously
	    //! allocated by \ref alloc.
	    //!
	    void free(volatile ServerTransceiverState* mem)
	    {
		BaseAllocator::free(static_cast<volatile TransceiverBufferData*>(mem));
	    }

	private:

	    //! \brief Achieves mutual exclusion in the allocator.
	    MPS::ScrMem::SpinMutex _mutex;

	    //! \brief Semaphore id for easy signaling of
	    //! end-of-memory condition
	    unsigned _sem;
	};


	//! \brief Bidirectional channel using two MPS::Queues.
	//!
	//! Transceivers use a single shared memory region for both
	//! queues following this layout: head1, tail1, head2, tail2,
	//! message memory.
	//!
	//! Two different constructors for client-side and server-side
	//! transceivers are provided.  The API is similar to that of
	//! Queue::Producer and Queue::Consumer.
	//!
	template <class T, class PeerSem, class LocalSem, class Mutex, int N>
	class Transceiver
	    : public MPS::Queue::Producer<T, DummyMutex, PeerSem, LocalSem, Mutex, N>,
	      public MPS::Queue::Consumer<T, DummyMutex, LocalSem, PeerSem, Mutex, N>
	{
	    typedef MPS::Queue::Producer<T, DummyMutex, PeerSem, LocalSem, Mutex, N> Producer;
	    typedef MPS::Queue::Consumer<T, DummyMutex, LocalSem, PeerSem, Mutex, N> Consumer;
	    
	public:

	    //! \brief Transceiver constructor for the client side.
	    //!
	    //! Each component of the transceiver is set up to allow
	    //! direct communication with the server side.  E.g. the
	    //! ring-buffers used for producing data (put method) and
	    //! consuming data (get method) respectively are just the
	    //! opposite of those used in the server side (client
	    //! produces where server consumes and server produces
	    //! where client consumes).
	    //!
	    //! \param s The parameter encapsulates all of the data
	    //! required to setup the transceiver.  Everything is
	    //! static, so that this structure can be sent to another
	    //! processor (see connection management in the \ref
	    //! Connector and \ref Acceptor classes).
	    //!
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

	    //! \brief Transceiver constructor for the server side.
	    //!
	    //! Each component of the transceiver is set up to allow
	    //! direct communication with the client side.  E.g. the
	    //! ring-buffers used for producing data (put method) and
	    //! consuming data (get method) respectively are just the
	    //! opposite of those used in the client side (client
	    //! produces where server consumes and server produces
	    //! where client consumes).
	    //!
	    //! \param s The parameter encapsulates all of the data
	    //! required to setup the transceiver.  Everything is
	    //! static, so that this structure can be sent to another
	    //! processor.  Since a ServerTransceiverState is a
	    //! superset of ClientTransceiverState the same structure
	    //! can be used to setup both ends of the channel.  See
	    //! connection management in the \ref Connector and \ref
	    //! Acceptor classes for further details.
	    //!
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
		//! \note Although we use a specific Mutex the
		//! Transceiver code would work with ScrMem::Mutex and
		//! ScrMem::SpinMutex since it is just used for an
		//! unlock operation.
		MPS::ScrMem::Mutex lock(s.clnt, s.clntLock);
		lock.unlock();		// Notifies the other end that
					// the channel is ready
	    }

	    //! \brief Close the transceiver.
	    //!
	    //! This method should be invoked when the channel is not
	    //! needed anymore.  It sends a null pointer to the other
	    //! end to signal the shutdown condition.  It unblocks any
	    //! possible peer writer waiting for some space in the
	    //! output buffer.
	    void close()
	    {
		Producer::put(0);
		_peerSemC.init(N - 1);	// unblocks waiting writers
		_peerSemC.post();
	    }

	private:
	    //! \brief Semaphore counting available items in the
	    //! incoming buffer.
	    //!
	    //! This hardware semaphore is signalled every time a new
	    //! item is produced from the peer side of the transceiver
	    //! (whenever \c put is invoked on the other end).  This
	    //! side of the transceiver will wait on this semaphore
	    //! whenever \c get is invoked.
	    LocalSem _mySemP;

	    //! \brief Semaphore counting available slots in the
	    //! outgoing buffer.
	    //!
	    //! This hardware semaphore is signalled every time a new
	    //! item is consumed from the peer side of the transceiver
	    //! (whenever \c get is invoked on the other end).  This
	    //! side of the transceiver will wait on this semaphore
	    //! whenever \c put is invoked to guarantee that there is
	    //! enough room in the outgoind buffer.
	    LocalSem _mySemC;

	    //! \brief Semaphore counting available items in the
	    //! outgoing buffer.
	    //!
	    //! This hardware semaphore is signalled every time a new
	    //! item is produced from this side of the transceiver
	    //! (whenever \c put is invoked on this end).  The peer
	    //! side of the transceiver will wait on this semaphore
	    //! whenever \c get is invoked.
	    PeerSem _peerSemP;
	    
	    //! \brief Semaphore counting available slots in the
	    //! incoming buffer.
	    //!
	    //! This hardware semaphore is signalled every time a new
	    //! item is consumed from this side of the transceiver
	    //! (whenever \c get is invoked on this end).  The peer
	    //! side of the transceiver will wait on this semaphore
	    //! whenever \c put is invoked to guarantee that there is
	    //! enough room in the incoming buffer.
	    PeerSem _peerSemC;

	    //! \brief Mutual exclusion in the message allocator.
	    //!
	    //! This mutex is used to guarantee thread-safe and mp-safe
	    //! operation of the message allocator.  The same allocator
	    //! is used on both ends of the channel to simplify memory
	    //! management.  Buffers allocated on one side may be freed
	    //! or recirculated on the other side.
	    Mutex _mutex;

	    //! \brief Fake mutex for ring buffers.
	    //!
	    //! Since channels are one-to-one communication channels
	    //! there is no need to care about locking during message
	    //! transmission.  This dummy mutex is used to always grant
	    //! access to the incoming and outgoing buffers.
	    DummyMutex _dummy;
	};


	//! \brief Information sent by client to request a connection.
	//!
	//! \c Acceptors setup an incomming connection ring buffer that
	//! queues connection requests.  A ConnectionRequest
	//! encapsulates the required information from the client in
	//! order to be able to setup a transceiver.
	struct ConnectionRequest {
	    //! \brief Processor id of the client.
	    //!
	    //! This is just needed to setup the scratch semaphores on
	    //! the client side.
	    int clnt;

	    //! \brief First semaphore on client side.
	    //!
	    //! This is the identifier of the first available scratch
	    //! semaphore on the client side.  Each channel consumes
	    //! three semaphores: consumed and produced items on the
	    //! client side, and an additional hardware semaphore where
	    //! client is waiting for the connection (see \c Connector)
	    unsigned sem;

	    //! \brief Shared memory area to store client side
	    //! transceiver data.
	    //!
	    //! The \c Acceptor running on the server side will fill
	    //! the required fields of this structure prior to
	    //! signalling a new connection to the client.
	    ClientTransceiverState* state;
	};

	
	//! \brief Connection establishment (passive side).
	//!
	//! An acceptor encapsulates an incoming ring buffer where \c
	//! ConnectionRequests are received.  Connection requests are
	//! sequentially processed to setup a bidirectional channel
	//! (transceiver).  The acceptor cooperates with the \c
	//! Connector to setup both ends of the transceiver.
	//!
	//! It is defined as a template where the user can select the
	//! maximum number of simultaneous connections, and the size of
	//! the message area in bytes.  Always try to use power of two
	//! sizes in order to optimize modulo operations.
	//!
	template <int N, int M>
	class Acceptor {
	    typedef MPS::RingBuffer::Consumer<ConnectionRequest,
					      DummyMutex,
					      MPS::ScrMem::SpinSemaphore,
					      MPS::ScrMem::SpinSemaphore,
					      N> ConnectionQueue;
	    typedef TransceiverAllocator<N, M> TransceiverAllocator;
	    
	public:
	    
	    //! \brief Acceptor constructor.
	    //!
	    //! Creates a new acceptor using a shared memory region and
	    //! some scratch semaphores.  An acceptor behaves as a
	    //! consumer of the connection request queue.
	    //!
	    //! The acceptor assumes the following layout in the shared
	    //! memory region: head pointer, then tail pointer, and
	    //! then data area for the connection request ring buffer.
	    //!
	    //! Likewise, an acceptor assumes a specific layout for
	    //! semaphores: produced items in the connection request
	    //! ring buffer (semP), then available slots in the
	    //! connection request ring buffer (semC), then a mutex to
	    //! guarantee mutual exclusion in presence of multiple
	    //! simultaneous connection requests (mutexP), and finally
	    //! the scratch semaphores required by the transceiver
	    //! allocator (semAlloc).  The transceiver allocator
	    //! consumes a single scratch semaphore for internal
	    //! locking and three additional semaphores per each
	    //! transceiver. There are N transceivers at most where N
	    //! is the first template parameter for Acceptor.
	    //!
	    //! \param mem Shared memory region (may also be scratch
	    //! memory) to be used for transceiver allocation.
	    //!
	    //! \param sem First scratch semaphore available for
	    //! allocator and transceiver synchronization.
	    //!
	    Acceptor(volatile void* mem, unsigned sem)
		: _incoming(reinterpret_cast<volatile ConnectionRequest*>(static_cast<volatile int*>(mem) + 2), // mem
			    *static_cast<volatile int*>(mem),		// head
			    _dummy, _semP, _semC),			// no mutex
		  _allocator(static_cast<volatile int*>(mem) + 2 + 3*N, sem + 3, true),
		  _semP(get_proc_id(), sem), _semC(get_proc_id(), sem + 1)
	    {
		_incoming.deactivate();	// Do not accept connections yet
	    }
	
	    //! \brief Stop processing incomming connection requests.
	    //!
	    //! In the current implementation there is no way to notify
	    //! clients that the acceptor is no longer available.
	    //! Therefore there is nothing to do on this method except
	    //! for a possible unlock of waiting writers.
	    //!
	    //! At this moment the way to notify exceptional conditions
	    //! such as acceptor unavailable is at the application
	    //! level through a shared memory status variable.
	    //!
	    //! An API compatible extension may be developed in the
	    //! near future to support basic connection exception
	    //! support.
	    //!
	    void close()
	    {
		_incoming.activate();	// Unblocks waiting writers
	    }

	    //! \brief Activate the incoming connection request queue.
	    //!
	    //! Allows clients to \c put requests in the connection
	    //! request ring buffer.  Similar to the homonimous
	    //! function in the POSIX TCP/IP programming interface.
	    //!
	    void listen()
	    {
		_incoming.activate();	// Allows incoming connections
	    }

	    //! \brief Allocates a transceiver for the first
	    //! connection request available.
	    //!
	    //! Process the first available connection request, setup a
	    //! transceiver and send client-side data to the client
	    //! side.  Note that we do not need to explicitly notify
	    //! the client side about the construction of the server
	    //! side transceiver because the \c Transceiver server side
	    //! constructor already does that.
	    //!
	    //! If all available transceivers are busy \c accept will
	    //! block until a transceiver is freed.  This is compatible
	    //! with the standard POSIX semantics but a user may change
	    //! the default semantics by subclassing the \c Acceptor.
	    //! If you want to avoid this blocking behaviour you may
	    //! also use a multi-threaded server.
	    //!
	    //! \returns This method returns a reference to a structure
	    //! holding all required data for server-side transceiver.
	    //! It does not return a transceiver in order to avoid
	    //! dynamic memory allocation and allow transceivers to be
	    //! subclassed.
	    //!
	    volatile ServerTransceiverState& accept()
	    {
		ConnectionRequest req = _incoming.get();
		volatile ServerTransceiverState* state = _allocator.alloc();

		//! \note The ServerTransceiverState is freed by the
		//! client side.  As it is currently implemented this
		//! requires a zero-state allocator. Otherwise you need
		//! a way to communicate the allocator data to the
		//! client (probably through ClientTransceiverState).

		state->clnt     = req.clnt;
		state->clntSem  = req.sem + 1;
		state->clntLock = req.sem;

		ClientTransceiverState* toClnt = const_cast<ServerTransceiverState*>(state);
		*req.state = *toClnt;	// send the ClientTransceiverState
		return *state;
	    }

	private:
	    //! \brief Connection request ring buffer.
	    //! 
	    //! The main responsibility of an acceptor is to manage
	    //! this queue of incomming connections.  Each connection
	    //! request is translated into a new transceiver till no
	    //! more transceivers are available.
	    //! 
	    ConnectionQueue _incoming;

	    //! \brief Allocates and deallocates transceivers.
	    //! 
	    //! All transceivers share the same shared memory region
	    //! for message allocation and deallocation.  If you want
	    //! different memory areas just use several acceptors.
	    //! 
	    TransceiverAllocator _allocator;

	    //! \brief Semaphore counting pending connection requests.
	    //! 
	    //! This semaphore is signalled each time a client requests
	    //! a connection by using a \c Connector. Each invocation
	    //! of \c accept will wait for available requests on this
	    //! semaphore.
	    MPS::ScrMem::SpinSemaphore _semP;

	    //! \brief Semaphore counting available slots in the
	    //! connection request queue.
	    //! 
	    //! This semaphore is signalled whenever \c accept is
	    //! successfully invoked.  Clients requesting a connection
	    //! automatically wait on this semaphore by using a \c
	    //! Connector.
	    MPS::ScrMem::SpinSemaphore _semC;

	    //! \brief Fake mutex to avoid unnecessary locking.
	    //! 
	    //! The connection request queue is a single-reader
	    //! configuration.  Therefore the server side (single
	    //! consumer of this queue) does not require locking.
	    DummyMutex _dummy;
	};

	
	//! \brief Connection establishment (active side)
	//! 
	//! A connector encapsulates client-initiated dynamic channel
	//! allocation.  It produces ConnectionRequests to be processed
	//! by an \c Acceptor and blocks on a scratch semaphore.  The
	//! acceptor cooperates with the \c Connector to setup both
	//! ends of the transceiver.  As soon as the acceptor builds
	//! the server-side transceiver it notifies the connector that
	//! the client-side transceiver may be instantiated using data
	//! provided by the acceptor.
	//! 
	//! It is defined as a template where the user can select the
	//! maximum number of simultaneous connections.  Always try to
	//! use power of two sizes in order to optimize modulo
	//! operations.
	//! 
	template <int N>
	class Connector {
	    typedef MPS::RingBuffer::Producer<ConnectionRequest,
					      MPS::ScrMem::SpinMutex,
					      MPS::ScrMem::SpinSemaphore,
					      MPS::ScrMem::SpinSemaphore,
					      N> ConnectionQueue;
	public:

	    //! \brief Connector constructor.
	    //! 
	    //! Creates a new connector using a shared memory region
	    //! and some remote scratch semaphores.  A connector
	    //! behaves as a producer of the connection request queue
	    //! on a peer acceptor.
	    //! 
	    //! The connector assumes the following layout in the
	    //! shared memory region: head pointer, then tail pointer,
	    //! and then data area for the connection request ring
	    //! buffer.
	    //! 
	    //! Likewise, a connector assumes a specific layout for
	    //! remote semaphores: produced items in the connection
	    //! request ring buffer (semP), then available slots in the
	    //! connection request ring buffer (semC), then a mutex to
	    //! guarantee mutual exclusion in presence of multiple
	    //! simultaneous connection requests (mutex), and finally
	    //! the scratch semaphores required by the transceiver
	    //! allocator (semAlloc) which are not directly visible
	    //! from the connector side (see \c Acceptor for more
	    //! details).
	    //! 
	    //! \param peer Processor id running the \c Acceptor
	    //! instance.
	    //! 
	    //! \param mem Shared memory region (may also be scratch
	    //! memory in the \c peer processor) to be used for
	    //! transceiver allocation.
	    //! 
	    //! \param sem First scratch semaphore available for
	    //! allocator and transceiver synchronization at the
	    //! acceptor side.
	    //! 
	    //! \param localMem Shared memory region (may also be
	    //! client-side scratch memory) to store required data to
	    //! setup client-side transceiver.
	    //! 
	    //! \param localSem Scratch semaphore id to be used for
	    //! notification of connection establishment from the
	    //! server-side.
	    //! 
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

	    //! \brief Request a connection.
	    //! 
	    //! Produces a connection request on the connection request
	    //! queue of a peer acceptor.  Then blocks on a local
	    //! scratch semaphore to wait for a notification from the
	    //! peer acceptor.
	    //! 
	    //! The connection request holds a reference to a local
	    //! memory region where the peer acceptor will store
	    //! client-side inititalization data.
	    //! 
	    //! \returns This method returns a reference to a structure
	    //! holding all required data for server-side transceiver.
	    //! It does not return a transceiver in order to avoid
	    //! dynamic memory allocation and allow transceivers to be
	    //! subclassed.
	    //! 
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

	    //! \brief Connection request ring buffer.
	    //! 
	    //! A connector acts as a producer on a peer acceptor queue
	    //! of incomming connection requests.  Each connection
	    //! request is translated by the remote acceptor into a new
	    //! transceiver.
	    //! 
	    ConnectionQueue _connections;
	    
	    //! \brief Semaphore counting pending connection requests.
	    //! 
	    //! This semaphore is signalled each time a client requests
	    //! a connection by using \c connect.  The acceptor will
	    //! wait for available requests on this semaphore.
	    MPS::ScrMem::SpinSemaphore _semP;

	    //! \brief Semaphore counting available slots in the
	    //! connection request queue.
	    //! 
	    //! This semaphore is signalled by the acceptor.  Clients
	    //! requesting a connection automatically wait on this
	    //! semaphore by using \c connect.
	    MPS::ScrMem::SpinSemaphore _semC;

	    //! \brief Used for synchronization among multiple connectors.
	    //! 
	    //! A single connection request queue may receive multiple
	    //! connection requests from different peers.  This mutex
	    //! guarantees proper synchronization of all simultaneous
	    //! \c connect invocations.
	    //! 
	    MPS::ScrMem::SpinMutex _mutex;

	    //! \brief Data required for client-side transceiver setup.
	    //! 
	    //! This variable is located in a shared memory region and
	    //! is filled by the acceptor when the connection request
	    //! is processed.  An intermediate transceiver state is
	    //! useful in order to avoid dynamic memory allocation and
	    //! allow transceivers to be subclassed.
	    //! 
	    volatile ClientTransceiverState& _state;

	    //! \brief Local scratch semaphore to wait for connection
	    //! establishment.
	    //! 
	    //! The client-side application will wait on this semaphore
	    //! automaically when a connection is requested.  The peer
	    //! acceptor will signal this semaphore when the
	    //! server-side transceiver is fully initialized.
	    //! 
	    unsigned _sem;
	};
	
    }
    
}

#endif
