//
// Created by djafere on 6/23/22.
//

#include "../h/_sleeplist.hpp"

void _sleeplist::insert(TCB *tcb, time_t timeout)
{
    priorityInsert(new SleepNode(tcb, timeout + passed), Comparator);
}

TCB *_sleeplist::pop()
{
    if (size() <= 0) { return nullptr; }
    SleepNode *s = removeFirst();
    TCB *ret = s->tcb;
    delete s;
    return ret;
}

void _sleeplist::tick()
{
    total_passed++;
    passed = (size() > 0) ?
             passed + 1 :
             0;

    while (ready()) {
        TCB *tcb = pop();
        tcb->wake();
    }
}

bool _sleeplist::ready() const
{
    return (size() > 0 && peekFirst()->timeout <= passed);
}
