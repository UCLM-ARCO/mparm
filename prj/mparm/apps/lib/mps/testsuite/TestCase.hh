#ifndef TEST_CASE_HH
#define TEST_CASE_HH

# include "simctrl.h"
# include <stdio.h>

# define MAX_COMPOSITE_TEST_CASES 20

class TestCase {
public:
    static volatile int* const _stamp;
    
public:
    TestCase(const char* id): _id(id), _debug(false) {}
    virtual ~TestCase() {}
    const char* id() const { return _id; }
    
    bool run()
    {
	bool ret;
	
	switch (get_proc_id()) {
	case 1:
	    ret = run1();
	    break;
	case 2:
	    ret = run2();
	    break;
	case 3:
	    ret = run3();
	    break;
	case 4:
	    ret = run4();
	    break;
	case 5:
	    ret = run5();
	    break;
	case 6:
	    ret = run6();
	    break;
	case 7:
	    ret = run7();
	    break;
	case 8:
	    ret = run8();
	    break;
	default:
	    stop_simulation();
	}
	if (ret)
	    passed();
	else
	    failed();
	return ret;
    }

    virtual bool run1() { stop_simulation(); }
    virtual bool run2() { stop_simulation(); }
    virtual bool run3() { stop_simulation(); }
    virtual bool run4() { stop_simulation(); }
    virtual bool run5() { stop_simulation(); }
    virtual bool run6() { stop_simulation(); }
    virtual bool run7() { stop_simulation(); }
    virtual bool run8() { stop_simulation(); }

    void debug(bool debug) { _debug = debug; }
    
protected:
    void failed() const { showResult("FAILED"); }
    void passed() const { showResult("PASSED"); }
    void trace(const char* str) const { if (_debug) showResult(str); }
    void failure(const char* str) const { showResult(str); }

    void failure(int expected, int got) const
    {
	char msg[80];
	sprintf(msg, "Test %s: Expected %d, Got %d", _id, expected, got);
	pr(msg, 0, PR_CPU_ID | PR_STRING | PR_NEWL);
    }

private:
    void showResult(const char* str) const
    {
	char msg[80];
	sprintf(msg, "Test %s: %s", _id, str);
	pr(msg, 0, PR_CPU_ID | PR_STRING | PR_NEWL);
    }
    
private:
    const char* _id;
    bool _debug;
};



class CompositeTestCase: public TestCase {
public:
    CompositeTestCase(const char* id): TestCase(id), _ntest(0){}
    
    CompositeTestCase& add(TestCase* t)
    {
	_test[_ntest++] = t;
	return *this;
    }

    bool run() {
	int failed = 0;
	for (int i=0; i < _ntest; ++i)
	    if (! _test[i]->run())
		++failed;
	
	showResult(failed);
	return failed == 0;
    }
    
    void debug(bool debug)
    {
	for (int i=0; i < _ntest; ++i)
	    _test[i]->debug(debug);
    }

private:
    void showResult(int failed) const
    {
	char msg[80];
	sprintf(msg, "Test %s: %s", id(), (failed? "FAILED": "PASSED"));
	pr(msg, failed,
	   PR_CPU_ID | PR_STRING | PR_NEWL | (failed? PR_DEC: 0));
    }
    
    
protected:
    int _ntest;
    TestCase* _test[MAX_COMPOSITE_TEST_CASES];
};

#endif
