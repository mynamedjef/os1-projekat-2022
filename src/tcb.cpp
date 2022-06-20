//
// Created by marko on 20.4.22..
//

#include "../h/tcb.hpp"
#include "../h/riscv.hpp"
#include "../h/sys_opcodes.h"

TCB *TCB::running = nullptr;

TCB *TCB::idle = nullptr;

uint64 TCB::timeSliceCounter = 0;

TCB *TCB::createThread(Body body, uint64 *stack_space, void *arg, bool init)
{
    return new TCB(body, DEFAULT_TIME_SLICE, stack_space, arg, init);
}

TCB *TCB::idleThread(Body body, uint64 *stack_space)
{
    return new TCB(body, stack_space);
}

// koliko vidim ovo se poziva samo iz korisničkog režima
// radi isto kao sistemski poziv
void TCB::yield()
{
    uint64 volatile a0;
    __asm__ volatile ("mv %0, a0" : "=r" (a0));
    __asm__ volatile ("mv a0, %0" : : "r" (YIELD));
    __asm__ volatile ("ecall");
    __asm__ volatile ("mv a0, %0" : : "r" (a0));
}

void TCB::dispatch()
{
    TCB *old = running;
    if (old->status == RUNNING) { // ako nije završena (FINISHED) ili ako ne čeka na semaforu (WAITING)
        old->status = READY;
        Scheduler::put(old);
    }
    running = Scheduler::get();
    if (running) {
        running->status = RUNNING;
    }
    else if (idle) {
        running = idle;
    }
    else {
        return;
    }

    TCB::contextSwitch(&old->context, &running->context);
}

void TCB::threadWrapper()
{
    Riscv::popSppSpie();
    running->body(running->arg);
    running->status = FINISHED;
    TCB::yield();
}

int TCB::exit()
{
    if (running->status != RUNNING) {
        return -1;
    }

    running->status = FINISHED;
    yield();
    return 0;
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
