#ifndef TEST_CASE_HH
#define TEST_CASE_HH

# include "simctrl.h"
# include <string.h>

class TestCaseDelegate {
public:
    bool run1() { stop_simulation(); }
    bool run2() { stop_simulation(); }
    bool run3() { stop_simulation(); }
    bool run4() { stop_simulation(); }
    bool run5() { stop_simulation(); }
    bool run6() { stop_simulation(); }
    bool run7() { stop_simulation(); }
    bool run8() { stop_simulation(); }

    void debug(bool debug) { _debug = debug; }
    
protected:
    void failed() const { showResult("FAILED"); }
    void passed() const { showResult("PASSED"); }
    void trace(const char* str) const { if (_debug) showResult(str); }
    void report(const char* str) const { showResult(str); }

    void failure(int expected, int got) const
    {
	char msg[80] = "Failure ";
	strcat(msg, _id);
	pr(msg, expected, PR_CPU_ID | PR_STRING | PR_NEWL | PR_DEC);
	pr(msg, got, PR_CPU_ID | PR_STRING | PR_NEWL | PR_DEC);
    }

private:
    void showResult(const char* str) const
    {
	char msg[80] = "Test ";
	strcat(msg, _id);
	strcat(msg, " : ");
	strcat(msg, str);
	pr(msg, 0, PR_CPU_ID | PR_STRING | PR_NEWL);
    }
    
private:
    const char* _id;
    bool _debug;
};


template <class Delegate>
class TestCase {
public:
    TestCase(const char* id): _tc(id) {}
    const char* id() const { return _tc.id(); }
    
    void debug(bool debug) { _tc.debug(debug); }

    bool run()
    {
	bool ret;
	
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
	    passed();
	else
	    failed();
	return ret;
    }

};

#endif
