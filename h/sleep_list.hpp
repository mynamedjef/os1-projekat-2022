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

    void addPriority(Sleeping *data);

private:
    time_t passed;
};

#endif //_sleep_list_hpp
