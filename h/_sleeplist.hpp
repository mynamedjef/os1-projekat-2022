//
// Created by djafere on 6/23/22.
//

#ifndef __sleeplist_hpp
#define __sleeplist_hpp

#include "tcb.hpp"

struct SleepNode {
    TCB *tcb;
    time_t timeout;

    SleepNode(TCB *_tcb, time_t _timeout) : tcb(_tcb), timeout(_timeout) { }

    void *operator new(size_t size) { return __mem_alloc(size); }

    void operator delete(void *ptr) { __mem_free(ptr); }
};

class _sleeplist : public List<SleepNode> {
public:
    _sleeplist() : passed(0), total_passed(0) { }

    void insert(TCB *tcb, time_t timeout);

    void tick();

private:
    TCB *pop();

    time_t passed;

    time_t total_passed;

    bool ready();

    static bool Comparator(SleepNode *t1, SleepNode *t2) { return t1->timeout < t2->timeout; }

};

#endif //__sleeplist_hpp
