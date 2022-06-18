//
// Created by djafere on 6/18/22.
//

#include "../h/_thread.hpp"

_thread::_thread(thread_t *handle, Body body, uint64 *stack_space, void *arg=nullptr)
{
    tcb = TCB::createThread(body, stack_space, arg);
    *handle = this;
}

int _thread::exit()
{
    if (TCB::running->isFinished()) { return -1; }
    
    TCB::running->setFinished(true);
    dispatch();
    return 0;
}

void _thread::dispatch()
{
    TCB::yield();
}

_thread::~_thread()
{
    delete tcb;
}
