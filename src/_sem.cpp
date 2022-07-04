//
// Created by djafere on 6/23/22.
//

#include "../h/_sem.hpp"
#include "../test/printing.hpp"

_sem::~_sem()
{
    close();
}

_sem::_sem(sem_t *handle, unsigned init)
{
    *handle = this;
    val = init;
    closed = false;
}

int _sem::close()
{
    if (closed) {
        return -1;
    }

    closed = true;
    val = 0;
    while (waiting.size() > 0) {
        TCB *tcb = waiting.pop();
        tcb->release();
    }
    return 0;
}

int _sem::wait()
{
    if (closed) {
        return -1;
    }
    printString("Wait(");
    printInt(TCB::running->get_id());
    printString(val > 0 ? ") [PROLAZI]" : ") [BLOKIRAN]");
    printString("\n");

    if (--val < 0) {
        waiting.insert(TCB::running);
        TCB::wait();
    }
    return (closed) ? -1 : 0;
}

int _sem::signal()
{
    if (closed) {
        return -1;
    }
    printString("Sign\t- ID pozivajuće niti: ");
    printInt(TCB::running->get_id());
    printString("\n");

    if (++val <= 0) {
        TCB *tcb = waiting.pop();
        tcb->release();
    }
    return 0;
}

bool comp(TCB *t1, TCB *t2)
{
    return t1->get_id() < t2->get_id();
}

int _sem::priority()
{
    if (closed) {
        return -1;
    }
    printString("Prio(");
    printInt(TCB::running->get_id());
    printString(")\n");
    
    if (++val <= 0) {
        TCB *tcb = waiting.pop_priority(comp);
        tcb->release();
    }
    return 0;
}
