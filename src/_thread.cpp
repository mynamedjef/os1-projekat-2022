//
// Created by djafere on 6/18/22.
//

#include "../h/_thread.hpp"

_thread::_thread(thread_t *handle, Body body, uint64 *stack_space, void *arg=nullptr)
{
    tcb = TCB::createThread(body, stack_space, arg, true);
    *handle = this;
}

int _thread::exit()
{
    return TCB::exit();
}

int _thread::start()
{
    return tcb->start();
}

void _thread::dispatch()
{
    TCB::yield();
}

_thread::~_thread()
{
    delete tcb;
}
