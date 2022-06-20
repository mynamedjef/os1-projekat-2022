//
// Created by djafere on 6/20/22.
//

#ifndef _sleep_list_hpp
#define _sleep_list_hpp

#include "list.hpp"
#include "tcb.hpp"
#include "syscall_c.h"

struct Sleeping {
    TCB *tcb;
    time_t time;
    Sleeping(TCB *tcb, time_t time) : tcb(tcb), time(time) {}
};

class SleepList : public List<Sleeping> {
public:
    bool Comparator(Sleeping *t1, Sleeping *t2);

    TCB *popPriority();

    void tick();

    bool is_ready();

    time_t time_passed();

    time_t total_time_passed();

    void addPriority(Sleeping *data);

    void addPriority(TCB*, time_t);

    static SleepList *instance();

private:
    time_t passed;

    time_t total_passed;

    static SleepList *inst;

    SleepList() : passed(0), total_passed(0) { };
};

#endif //_sleep_list_hpp
