//
// Created by djafere on 6/23/22.
//

#include "../h/_sleeplist.hpp"

void _sleeplist::insert(SleepNode *data)
{
    if (!head || Comparator(data, head->data))
    {
        addFirst(data);
    }
    else
    {
        Elem *prev = head;
        for (Elem *curr = head->next; curr; curr = curr->next) {
            if (Comparator(data, curr->data)) {
                insertAfter(prev, data);
                return;
            }
            prev = curr;
        }
        addLast(data);
    }
}

void _sleeplist::insert(TCB *tcb, time_t timeout)
{
    insert(new SleepNode(tcb, timeout + passed));
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

bool _sleeplist::ready()
{
    return (size() > 0 && peekFirst()->timeout <= passed);
}
