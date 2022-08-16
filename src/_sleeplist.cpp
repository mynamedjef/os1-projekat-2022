//
// Created by djafere on 6/23/22.
//

#include "../h/_sleeplist.hpp"

_sleeplist _sleeplist::Sleeping;

time_t _sleeplist::passed = 0;

time_t _sleeplist::total_passed = 0;

void _sleeplist::insert(SleepNode *data)
{
    Elem *head = Sleeping.head;
    if (!head || Comparator(data, head->data))
    {
        Sleeping.addFirst(data);
    }
    else
    {
        Elem *prev = head;
        for (Elem *curr = head->next; curr; curr = curr->next) {
            if (Comparator(data, curr->data)) {
                Sleeping.insertAfter(prev, data);
                return;
            }
            prev = curr;
        }
        Sleeping.addLast(data);
    }
}

void _sleeplist::insert(TCB *tcb, time_t timeout)
{
    insert(new SleepNode(tcb, timeout + passed));
}

TCB *_sleeplist::pop()
{
    if (Sleeping.size() <= 0) { return nullptr; }
    SleepNode *s = Sleeping.removeFirst();
    TCB *ret = s->tcb;
    delete s;
    return ret;
}

SleepNode *_sleeplist::pop_node()
{
    return Sleeping.removeFirst();
}

void _sleeplist::tick()
{
    total_passed++;
    passed = (Sleeping.size() > 0) ?
             passed + 1 :
             0;
    
    while (ready()) {
        TCB *tcb = pop();
        tcb->wake();
    }
}

bool _sleeplist::ready()
{
    return (Sleeping.size() > 0 &&
        Sleeping.peekFirst()->timeout <= passed);
}
