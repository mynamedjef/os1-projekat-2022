//
// Created by djafere on 6/19/22.
//

#include "../h/_sem.hpp"

_sem::_sem(int value) : value(value), closed(false) {}

_sem::_sem() : _sem(0) {}

_sem::~_sem()
{
    close();
}

int _sem::wait()
{
    if (closed) {
        return -1;
    }


    if (--value < 0) {
        waiting.addLast(TCB::running);
        TCB::running->setStatus(TCB::WAITING);
        TCB::yield();
    }
    
    return (!closed) ? 0 : -1;
}

int _sem::signal()
{
    if (closed) {
        return -1;
    }

    if (++value <= 0) {
        unblock();
    }
    
    return 0;
}

int _sem::val() const
{
    return value;
}

int _sem::close()
{
    int size = waiting.size();
    closed = true;
    while (waiting.size() > 0) {
        unblock();
    }
    return size;
}

void _sem::unblock()
{
    if (waiting.size() > 0) {
        TCB *curr = waiting.removeFirst();
        curr->setStatus(TCB::READY);
        Scheduler::put(curr);
    }
}