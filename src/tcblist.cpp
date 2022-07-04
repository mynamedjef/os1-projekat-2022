//
// Created by djafere on 6/27/22.
//

#include "../h/tcblist.hpp"


void TCBList::insert(TCB *data)
{
    Elem *elem = new Elem(data, 0);
    count++;
    if (tail)
    {
        tail->next = elem;
        tail = elem;
    }
    else
    {
        head = tail = elem;
    }
}

TCB *TCBList::pop()
{
    if (!head) { return nullptr; }

    count--;
    Elem *elem = head;
    head = head->next;
    if (!head) { tail = 0; }

    TCB *ret = elem->data;
    delete elem;
    return ret;
}

TCB *TCBList::pop_priority(bool(*comp)(TCB*, TCB*))
{
    if (!count) { return nullptr; }
    Elem *prev = head;
    Elem *best = head;
    Elem *prev_of_best = nullptr;
    for (Elem *curr = head->next; curr; curr = curr->next) {
        if (comp(curr->data, best->data)) {
            best = curr;
            prev_of_best = prev;
        }
        prev = curr;
    }
    
    if (best == head) { return pop(); }
    
    TCB *ret = best->data;
    count--;
    prev_of_best->next = best->next;
    if (best == tail) { tail = prev_of_best; }
    delete best;
    return ret;
}
