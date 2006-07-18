#ifndef TEST_CASE_HH
#define TEST_CASE_HH

# include "simctrl.h"
# include <string.h>

class TestCaseDelegate {
public:
    TestCaseDelegate() : _debug(false) {}

    bool run1() { stop_simulation(); }
    bool run2() { stop_simulation(); }
    bool run3() { stop_simulation(); }
    bool run4() { stop_simulation(); }
    bool run5() { stop_simulation(); }
    bool run6() { stop_simulation(); }
    bool run7() { stop_simulation(); }
    bool run8() { stop_simulation(); }

    void debug(bool debug) { _debug = debug; }
    
    void failed() const { showResult("FAILED"); }
    void passed() const { showResult("PASSED"); }
    void trace(const char* str) const { if (_debug) showResult(str); }
    void report(const char* str) const { showResult(str); }

    void failure(int expected, int got) const
    {
	pr("Failure ", expected, PR_CPU_ID | PR_STRING | PR_NEWL | PR_DEC);
	pr("Failure ", got, PR_CPU_ID | PR_STRING | PR_NEWL | PR_DEC);
    }

private:
    void showResult(const char* str) const
    {
	pr((char*)str, 0, PR_CPU_ID | PR_STRING | PR_NEWL);
    }
    
private:
    bool _debug;
};


template <class Delegate>
class TestCase {
public:
    TestCase() {}
    
    void debug(bool debug) { _tc.debug(debug); }
    void report(const char* str) { _tc.report(str); }

    bool run()
    {
	bool ret;

	report("Starting...");
	switch (get_proc_id()) {
	case 1: ret = _tc.run1(); break;
	case 2: ret = _tc.run2(); break;
	case 3: ret = _tc.run3(); break;
	case 4: ret = _tc.run4(); break;
	case 5: ret = _tc.run5(); break;
	case 6: ret = _tc.run6(); break;
	case 7: ret = _tc.run7(); break;
	case 8: ret = _tc.run8(); break;
	default: stop_simulation();
	}
	
	if (ret)
	    _tc.passed();
	else
	    _tc.failed();
	return ret;
    }
    
private:
    Delegate _tc;
};

#endif
