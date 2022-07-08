//
// Created by djafere on 6/23/22.
//

#include "../h/_sem.hpp"

List<_sem> _sem::all_sems;

_sem::~_sem()
{
    close();
}

_sem::_sem(sem_t *handle, unsigned init)
{
    *handle = this;
    val = init;
    closed = false;
    all_sems.addLast(this);
}

int _sem::close()
{
    if (closed) {
        return -1;
    }

    closed = true;
    val = 0;
    while (waiting.size() > 0) {
        TCB *tcb = waiting.removeFirst();
        tcb->release();
    }
    return 0;
}

int _sem::wait()
{
    if (closed) {
        return -1;
    }

    if (--val < 0) {
        waiting.addLast(TCB::running);
        TCB::wait();
    }
    return (closed) ? -1 : 0;
}

int _sem::signal()
{
    if (closed) {
        return -1;
    }

    if (++val <= 0) {
        TCB *tcb = waiting.removeFirst();
        tcb->release();
    }
    return 0;
}

void _sem::cleanup()
{
    while (all_sems.size() > 0) {
        _sem *temp = all_sems.removeFirst();
        if (temp) delete temp;
    }
}

