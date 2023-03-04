
#include "../h/scheduler.hpp"
#include "../h/tcb.hpp"

List<TCB> Scheduler::readyThreadQueue;

TCB *Scheduler::get()
{
    return readyThreadQueue.removeFirst();
}

void Scheduler::put(TCB *ccb)
{
    readyThreadQueue.addLast(ccb);
}

int Scheduler::size()
{
    return readyThreadQueue.size();
}

void Scheduler::flush_user_threads()
{
    TCB *sentinel = nullptr;
    put(sentinel);

    TCB *curr = get();
    while (curr != sentinel)
    {
        if (curr->is_systhread())
        {
            put(curr);
        }
        curr = get();
    }
}
