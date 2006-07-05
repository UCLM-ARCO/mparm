#include "TestCase.hh"
#include "scratch_semaphore.h"
#include "appsupport.h"
#ifdef __rtems__
# include "ss_semaphore.h"
#endif
#include <MPS/Semaphore.h>

#include <unistd.h>

// Semaphore to signal end condition
MPS::ScrMem::Semaphore end1(1,61,get_proc_id() == 1,0);
MPS::ScrMem::Semaphore end2(2,61,get_proc_id() == 1,0);

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
	time_start(i);
	WAIT(_sem);

	trace("lock");

	end2.post();
	end1.wait();

	sleep(0);

	time_stop(i);
	SIGNAL(_sem);
	
	trace("unlock");
	end2.post();
	end1.wait();
    }
    return true;
}

bool
ShSemLegacy::run2()
{
    for (int i = 0; i<N; ++i)  {
	end2.wait();
	end1.post();
	
	time_start(i);
	WAIT(_sem);
	time_stop(i);
	
	trace("lock");

	SIGNAL(_sem);
	
	trace("unlock");
	end2.wait();
	end1.post();
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
	time_start(i);
	while (!scratch_sem_wait(_sem));

	trace("lock");

	end2.post();
	end1.wait();

	sleep(0);

	time_stop(i);
	scratch_sem_signal(_sem);
	
	trace("unlock");
	end2.post();
	end1.wait();
    }
    return true;
}

bool
ScrSemLegacy::run2()
{
    for (int i = 0; i<N; ++i)  {
	end2.wait();
	end1.post();

	time_start(i);
	while(!scratch_sem_wait(_sem));
	time_stop(i);
	
	trace("lock");

	scratch_sem_signal(_sem);
	
	trace("unlock");
	end2.wait();
	end1.post();
    }
    return true;
}

#ifdef __rtems__

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
	time_start(i);
	ss_sem_wait(_sem);

	trace("lock");

	end2.post();
	end1.wait();

	sleep(0);

	time_stop(i);
	ss_sem_signal(_sem);
	
	trace("unlock");
	end2.post();
	end1.wait();
    }
    return true;
}

bool
ScrSSemLegacy::run2()
{
    for (int i = 0; i<N; ++i)  {
	end2.wait();
	end1.post();

	time_start(i);
	ss_sem_wait(_sem);
	time_stop(i);
	
	trace("lock");

	ss_sem_signal(_sem);
	
	trace("unlock");
	end2.wait();
	end1.post();
    }
    return true;
}

#endif

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
	add(new ShSemLegacy(ID, "appsupport"));
	add(new ScrSemLegacy(ScratchID, "scratch_semaphore"));
#ifdef __rtems__
	add(new ScrSSemLegacy(SScratchID, "ss_semaphore"));
#endif
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
