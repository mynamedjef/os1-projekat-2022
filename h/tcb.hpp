//
// Created by marko on 20.4.22..
//

#ifndef OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP
#define OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP

#include "../lib/hw.h"
#include "scheduler.hpp"

// Thread Control Block
class TCB
{
public:
    enum Status {
        CREATED,
        FINISHED,
        READY,
        RUNNING
    };
    
    ~TCB() { delete[] stack; }

    bool isFinished() const { return status == FINISHED; }

    void setStatus(Status status) { this->status = status; }

    uint64 getTimeSlice() const { return timeSlice; }

    using Body = void (*)(void*);

    static TCB *createThread(Body, uint64*, void*);

    static void yield();

    static TCB *running;

private:
    TCB(Body body, uint64 timeSlice, uint64 *stack_space, void *arg) :
            body(body),
            stack(body != nullptr ? stack_space : nullptr),
            context({(uint64) &threadWrapper,
                     stack != nullptr ? (uint64) &stack[DEFAULT_STACK_SIZE] : 0
                    }),
            timeSlice(timeSlice),
            arg(body != nullptr ? arg : nullptr)
    {
        if (body != nullptr) {
            Scheduler::put(this);
            status = READY;
        }
        else { // radi se o kernel main niti
            status = RUNNING;
        }
    }

    struct Context
    {
        uint64 ra;
        uint64 sp;
    };

    Body body;
    uint64 *stack;
    Context context;
    uint64 timeSlice;
    Status status;
    void *arg;

    friend class Riscv;

    static void threadWrapper();

    static void contextSwitch(Context *oldContext, Context *runningContext);

    static void dispatch();

    static uint64 timeSliceCounter;
};

#endif //OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP
