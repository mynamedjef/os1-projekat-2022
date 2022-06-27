
#include "../h/scheduler.hpp"

TCBList Scheduler::readyThreadQueue;

TCB *Scheduler::get()
{
    return readyThreadQueue.pop();
}

void Scheduler::put(TCB *tcb)
{
    readyThreadQueue.insert(tcb);
}

int Scheduler::size()
{
    return readyThreadQueue.size();
}
