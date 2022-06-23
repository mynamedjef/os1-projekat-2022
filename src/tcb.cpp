//
// Created by marko on 20.4.22..
//

#include "../h/tcb.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.h"

TCB *TCB::running = nullptr;

TCB *TCB::kernel = nullptr;

uint64 TCB::timeSliceCounter = 0;

TCB *TCB::createThread(Body body, void *arg, uint64 *stack)
{
    TCB *thr = initThread(body, arg, stack);
    thr->status = READY;
    Scheduler::put(thr);
    return thr;
}

TCB *TCB::initThread(Body body, void *arg, uint64 *stack)
{
    return new TCB(body, arg, stack);
}

TCB *TCB::kernelThread()
{
    if (!kernel) {
        TCB *thr = new TCB();
        running = kernel = thr;
    }
    return kernel;
}

int TCB::start()
{
    if (status != CREATED) {
        return -1;
    }
    status = READY;
    Scheduler::put(this);
    return 0;
}

int TCB::exit()
{
    if (running->status != RUNNING) {
        return -1;
    }
    running->status = FINISHED;
    dispatch();
    return 0;
}

int TCB::wait()
{
    if (running->status != RUNNING) {
        return -1;
    }
    running->status = WAITING;
    dispatch();
    return 0;
}

int TCB::release()
{
    if (status != WAITING) {
        return -1;
    }
    status = READY;
    Scheduler::put(this);
    return 0;
}

void TCB::dispatch()
{
    TCB *old = running;
    if (old->status == RUNNING) {
        old->status = READY;
        Scheduler::put(old);
    }

    running = Scheduler::get();
    running->status = RUNNING;

    TCB::contextSwitch(&old->context, &running->context);
}

void TCB::threadWrapper()
{
    Riscv::popSppSpie();
    running->body(running->arg);
    running->setStatus(FINISHED);
    thread_dispatch();
}
