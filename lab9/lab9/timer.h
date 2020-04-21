#ifndef _CSCI441_TIMER_H_
#define _CSCI441_TIMER_H_

#include <plf/nanotimer.h>

class Timer {
protected:
	plf::nanotimer _timer;
    double _delta;

public:
    Timer() : _delta(0) {}
    void start() { _timer.start(); }
    void stop() { _delta += _timer.get_elapsed_ms(); }
    void reset() { _delta = 0; }
    double total() const { return _delta; }
};
#endif
