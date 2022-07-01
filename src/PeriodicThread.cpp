//
// Created by djafere on 6/24/22.
//

#include "../h/syscall_cpp.hpp"

struct period_struct
{
    PeriodicThread *pt;
    time_t period;
};

void PeriodicThread::wrapper(void *arg)
{
    period_struct *p = (period_struct*)arg;
    while (true) {
        time_sleep(p->period);
        p->pt->periodicActivation();
    }
    delete p;
}

PeriodicThread::PeriodicThread(time_t period) :
    Thread(PeriodicThread::wrapper, (void*)(new (period_struct){this, period}))
{ }

