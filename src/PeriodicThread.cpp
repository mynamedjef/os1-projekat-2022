//
// Created by djafere on 6/24/22.
//

#include "../h/syscall_cpp.hpp"

PeriodicThread::PeriodicThread(time_t period) : period(period) { }

void PeriodicThread::run()
{
    while (true)
    {
        time_sleep(period);
        periodicActivation();
    }
}
