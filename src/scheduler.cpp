
#include "../h/scheduler.hpp"
#include "../h/tcb.hpp"

TCB *Scheduler::head = nullptr;

TCB *Scheduler::tail = nullptr;

int Scheduler::count = 0;

TCB *Scheduler::get()
{
    if (head == nullptr)
    {
        return nullptr;
    }

    count--;
    TCB *ret = head;
    head = head->next;
    if (ret == tail) { tail = nullptr; }
    return ret;
}

void Scheduler::put(TCB *tcb)
{
    count++;
    tcb->next = nullptr;
    if (head == nullptr)
    {
        head = tail = tcb;
    }
    else
    {
        tail = tail->next = tcb;
    }
}

int Scheduler::size()
{
    return count;
}
