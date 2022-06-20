//
// Created by os on 6/20/22.
//

#include "../h/sleep_list.hpp"

SleepList *SleepList::inst = nullptr;

SleepList *SleepList::instance()
{
    if (inst) {
        return inst;
    }
    inst = new SleepList();
    return inst;
}

bool SleepList::Comparator(Sleeping *t1, Sleeping *t2)
{
    return (t1->time < t2->time);
}

TCB *SleepList::popPriority()
{
    if (!head) { return nullptr; }
    Sleeping *s = removeFirst();
    TCB *ret = s->tcb;
    
    delete s;
    return ret;
}

void SleepList::tick()
{
    passed = (size() < 1) ? 0 : passed+1;
    total_passed++;
    while (is_ready()) {
        popPriority()->wake();
    }
}

int SleepList::sleep(time_t time)
{
    return TCB::running->sleep(time);
}

bool SleepList::is_ready()
{
    return (size() > 0 && peekFirst()->time <= passed);
}

time_t SleepList::time_passed()
{
    return passed;
}

time_t SleepList::total_time_passed()
{
    return total_passed;
}

void SleepList::addPriority(Sleeping *data) {
    if (!head || data->time < head->data->time)
    {
        addFirst(data);
        return;
    }
    else
    {
        Elem *elem = new Elem(data, nullptr);
        count++;
        
        Elem *prev = head;
        for (Elem *curr = head->next; curr; curr = curr->next) {
            if (data->time < curr->data->time) {
                prev->next = elem;
                elem->next = curr;
                return;
            }
        }
        prev->next = elem;
        tail = elem;
    }
}

void SleepList::addPriority(TCB *tcb, time_t time)
{
    SleepList::addPriority(new Sleeping(tcb, time+passed));
}
