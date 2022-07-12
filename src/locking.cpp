//
// Created by djafere on 7/12/22.
//

#include "../h/locking.hpp"
#include "../h/tcb.hpp"

volatile int Locking::lock_cnt = 0;

volatile bool Locking::dispatch_call = false;

void Locking::lock()
{
    lock_cnt++;
}

void Locking::unlock()
{
    if (--lock_cnt <= 0 && dispatch_call)
    {
        TCB::dispatch();
    }
}
