#include <MPS/Config.h>
#include "TestCase.hh"
#include "scratch_semaphore.h"
#include "ss_semaphore.h"
#include "appsupport.h"

// Meassurements of legacy semaphores latency

class ShSemLegacy : public TestCase {
public:
    ShSemLegacy(int sem, const char* id) : TestCase(id), _sem(sem)
    {
    }
    
    bool run1();
    bool run2();

private:
    int _sem;
};

#define N 20

bool
ShSemLegacy::run1()
{
    for (int i = 0; i<N; ++i)  {
	wait(0);

	time_start(i);
	WAIT(_sem);

	trace("lock");

	stamp(11);
	wait(12);

	sleep(0);

	time_stop(i);
	SIGNAL(_sem);
	
	trace("unlock");
	stamp(13);
    }
    return true;
}

bool
ShSemLegacy::run2()
{
    for (int i = 0; i<N; ++i)  {
	wait(11);
	stamp(12);

	time_start(i);
	WAIT(_sem);
	time_stop(i);
	
	trace("lock");

	SIGNAL(_sem);
	
	trace("unlock");
	wait(13);
	stamp(0);
    }
    return true;
}


class ScrSemLegacy : public TestCase {
public:
    ScrSemLegacy(int sem, const char* id)
	: TestCase(id), _sem(&MPS::ScrMem::semaphore_base(1)[sem])
    {
	if (get_proc_id() == 1)
	    scratch_sem_init(_sem, 1);
    }
    
    bool run1();
    bool run2();

private:
    SCRATCH_SEMAPHORE _sem;
};

bool
ScrSemLegacy::run1()
{
    for (int i = 0; i<N; ++i)  {
	wait(0);

	time_start(i);
	while (!scratch_sem_wait(_sem));

	trace("lock");

	stamp(11);
	wait(12);

	sleep(0);

	time_stop(i);
	scratch_sem_signal(_sem);
	
	trace("unlock");
	stamp(13);
    }
    return true;
}

bool
ScrSemLegacy::run2()
{
    for (int i = 0; i<N; ++i)  {
	wait(11);
	stamp(12);

	time_start(i);
	while(!scratch_sem_wait(_sem));
	time_stop(i);
	
	trace("lock");

	scratch_sem_signal(_sem);
	
	trace("unlock");
	wait(13);
	stamp(0);
    }
    return true;
}

class ScrSSemLegacy : public TestCase {
public:
    ScrSSemLegacy(int sem, const char* id)
	: TestCase(id), _sem(&MPS::ScrMem::semaphore_base(2)[sem])
    {
	ss_sem_system_init((unsigned)(MPS::ScrMem::semaphore_base(2) + sem),
			   (unsigned)(MPS::ScrMem::memory_base(2) + sem),
			   32);
	if (get_proc_id() == 1)
	    ss_sem_init(_sem, 1);
    }
    
    bool run1();
    bool run2();

private:
    SS_SEMAPHORE _sem;
};

bool
ScrSSemLegacy::run1()
{
    for (int i = 0; i<N; ++i)  {
	wait(0);

	time_start(i);
	ss_sem_wait(_sem);

	trace("lock");

	stamp(11);
	wait(12);

	sleep(0);

	time_stop(i);
	ss_sem_signal(_sem);
	
	trace("unlock");
	stamp(13);
    }
    return true;
}

bool
ScrSSemLegacy::run2()
{
    for (int i = 0; i<N; ++i)  {
	wait(11);
	stamp(12);

	time_start(i);
	ss_sem_wait(_sem);
	time_stop(i);
	
	trace("lock");

	ss_sem_signal(_sem);
	
	trace("unlock");
	wait(13);
	stamp(0);
    }
    return true;
}


class CompositeLegacyLatency: public CompositeTestCase
{
    int _core;
    enum {
	ID = 5,
	ScratchID,
	SScratchID,
    };

public:
    CompositeLegacyLatency()
	: CompositeTestCase("Legacy semaphores latencies"),
	  _core(get_proc_id())
    {
	add(new ShSemLegacy(ID, "ShSemLegacy"));
	add(new ScrSemLegacy(ScratchID, "ScrSemLegacy"));
	add(new ScrSSemLegacy(SScratchID, "ScrSSemLegacy"));
    }
};

#ifndef COMBINED_TEST_CASES
int
main()
{
    CompositeLegacyLatency test;

    //test.debug(true);
    test.run();
    stop_simulation();
}
#endif
