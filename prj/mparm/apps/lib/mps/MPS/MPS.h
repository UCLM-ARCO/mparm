#ifndef MPS_H
#define MPS_H

/*! \file MPS.h

  \brief Include all of the MPS modules.

  \page design Overall design considerations
  
  MPS modules strictly follow some overall guidelines and rules in
  order to achieve maximum suitability for most MPARM application
  domains.  We will briefly summarize those in the following
  paragraphs.

  \section layers A layered approach

  MPS handles multi-processor application development in a layered
  fashion with well-defined resposibilities.

  \li The MPS low-level synchronization primitives provide a range of
      abstractions frequently found in parallel programs: mutexes,
      semaphores, condition variables, monitors, etc.  Even MPARM
      users not willing to stick to any MPS communication model would
      certainly benefit from the homogeneous and intuitive API of
      these lower-level abstractions when implementing their own
      communication models.  They are even useful for multi-threaded
      programming on a single processor.

  \li The MPS communication primitives provide easy to use,
      object-oriented abstractions for common hardware primitives used
      for inter-processor communication, such as fifo-like structures
      or shared memory.  These primitives rely heavily on the
      lower-level synchronization primitives.  MPARM users may use
      them directly to implement their own application-specific
      communication models or they may choose to stick to the models
      provided by MPS.

  \li The MPS communication models provide a selection of simple to
      use abstractions implementing frequently used communication
      models for parallel applications.  Currently MPS provides
      flexible message queues, bidirectional one-to-one
      connection-oriented channels and a remote method invocation
      infrastructure built on top of the bidirectional channels.
      Support for global data objects as seen in DRI (www.data-re.org)
      or Mercury PAS (www.mcs.com) is in the works.

  MPS abstractions are selected not just from a functional point of
  view, but also trying to maximize reuse in the remaining MPS
  infrastructure.  All of the higher-level primitives rely heavily on
  the lower layers.
  
  \section overheadcpp Object-orientation does not imply inefficient code

  Object orientation has been the target of some criticism because it
  frequently leads to bloated and slow implementations.  Actually this
  has nothing to do with object orientation but with sloppy design.
  In a MPSoC environment we must pay a lot of attention to every
  possible overhead we may incur.  C++ hides a lot of complexity but
  also makes much easier to introduce many small overheads in very
  frequently used operations.  The following C++ features introduce
  some overhead and thus they should be carefully introduced when
  strictly needed:

  \li Inheritance is a powerful feature but it is also the source of
      major headaches.  Whenever there is a virtual member function
      there is an extra indirection each time that member function is
      invoked.  Inlining would not help either since the specific
      method must be identified at run-time by looking into the
      virtual dispatch table.  Virtual dispatching overhead is
      generally considered negligible but many cascaded indirections
      make the overal program impossible to optimize.

  \li Virtual inheritance also introduce an indirection even when
      there are no virtual methods.  In this case the indirection is
      used to access the \c this parameter.

  \li Unnecessary inheritance is not only responsible for slow
      performance, it is also a major source of code bloat and
      maintainment nightmares.  When a class derives from some base
      class, the derived class is strongly coupled with the base
      class.  Any modification in the base class will require a
      recompilation of the derived classes, and any program which
      instantiates the derived class must also include all of the base
      class implementation.

  \li Contrary to popular belief, inheritance is not the only (not
      even the most important) mechanism to reuse functionality in a
      properly designed object-oriented program.  Delegation and
      static composition (using templates) are extremely useful to
      achieve reuse without incurring in the strong coupling related
      problems.

  \li C++ and most object-oriented programming languages greatly
      simplifies the usage of dynamic memory.  This frequently
      translates into extremely inefficient usage of the heap by
      allocating lots of very small chunks of data.  Novice C++ users
      may avoid that by sticking to a good implementation of the
      standard C++ library.  Experienced users may even provide
      specific allocators to optimize heap usage in the target
      application.  Unfortunately both application-specific allocators
      and vendor optimized standard C++ libraries rely heavily on a
      few heap management functions provided by the operating system.
      MPS aims to suit OS-less target applications and even OS
      implementations for MPSoC platforms.  Therefore we cannot assume
      that an implementation of malloc/realloc is available.

  \section overhead Minimum overhead in low-level primitives

  Lower-level MPS primitives aim at providing a nice object-oriented
  interface with zero-overhead with respect to hand-coded equivalent
  primitives.  Using a recent version of GCC compiler the results in
  terms of bus accesses and instruction counts are comparable to
  hand-coded hardware-specific primitives that explicitly state bus
  transactions on memory-mapped synchronization primitives.  This is
  achieved by using a number of techniques:

  \li We never use virtual member functions in lower-level primitives.
      Currently just the MPS distributed object implementations uses
      virtual member functions.

  \li We never use virtual inheritance, also known as fork-join
      inheritance.  There were extensive debates in the past on the
      relative merits of virtual inheritance versus default multiple
      inheritance but most of this criticism does not hold when there
      are no virtual member functions.  Multiple inheritance should
      always be avoided except for the trivial cases anyway.

  \li Whenever possible we statically specialize clases by means of
      static composition (template classes).  This enhances the
      compiler optimization effectiveness (specially for inlined
      code).

  \li We strictly follow the write-once principle by judiciously using
      non-virtual inheritance without any virtual member function.

  \li Values that will be constant for the whole life-time of an
      object are transformed into template parameters if substantial
      gain may be achieved by the compiler on specific values.  For
      example, buffer sizes which are powers of two may lead to much
      better optimized code for modulo operations.

  \section staticcompo Static composition techniques

  Common behaviour of MPS classes is frequently implemented by means
  of a template \c Base* class and specific technology-dependent
  behaviour is delegated in a \c *State template parameter.  For
  example, common behaviour for all mutexes is implemented in the \ref
  MPS::BaseMutex template class and specific technology-dependent
  behaviour is delegated in a \c MutexState template parameter.
  Specialized Mutex versions are provided for scratch semaphores
  (MPS::ScrMem::Mutex) and shared memory semaphores
  (MPS::ShMem::Mutex).

  \c *State classes constitute an inheritance hierarchy to maximize
  reuse.  This is a very limited hierarchy with at most three
  inheritance levels, no virtual member functions and no virtual
  inheritance.
*/

# include <MPS/Semaphore.h>
# include <MPS/Mutex.h>
# include <MPS/RecMutex.h>
# include <MPS/RingBuffer.h>
# include <MPS/Queue.h>
# include <MPS/Channel.h>

#endif
