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
