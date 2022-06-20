//
// Created by marko on 20.4.22..
//

#ifndef OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP
#define OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP

#include "../lib/hw.h"
#include "scheduler.hpp"
#include "sleep_list.hpp"

// Thread Control Block
class TCB
{
public:
    enum Status {
        CREATED,
        FINISHED,
        READY,
        WAITING,
        RUNNING,
        IDLE,
        SLEEPING
    };
    
    ~TCB() { delete[] stack; }

    bool isFinished() const { return status == FINISHED; }

    void setStatus(Status status) { this->status = status; }
    
    Status getStatus() const { return status; }

    uint64 getTimeSlice() const { return timeSlice; }

    using Body = void (*)(void*);

    static TCB *createThread(Body, uint64*, void*, bool);

    static TCB *idleThread(Body, uint64*);
    
    static void yield();

    static TCB *running;
    
    static TCB *idle;

    static List<TCB> all_tcbs;

private:
    TCB(Body body, uint64 timeSlice, uint64 *stack_space, void *arg, bool init) :
            body(body),
            stack(body != nullptr ? stack_space : nullptr),
            context({(uint64) &threadWrapper,
                     stack != nullptr ? (uint64) &stack[DEFAULT_STACK_SIZE] : 0
                    }),
            timeSlice(timeSlice),
            arg(body != nullptr ? arg : nullptr)
    {
        if (body == nullptr) { // radi se o kernel main niti
            status = RUNNING;
        }
        else if (init) { // nit je inicijalizovana ali ne startovana
            status = CREATED;
        }
        else { // nit je startovana čim se napravi
            status = READY;
            Scheduler::put(this);
        }
        all_tcbs.addLast(this);
    }
    
    TCB(Body body, uint64 *stack_space) :
        TCB(body, DEFAULT_TIME_SLICE, stack_space, nullptr, true)
    {
        status = IDLE;
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
    
    friend class _thread;

    friend class SleepList;

    static void threadWrapper();

    static void contextSwitch(Context *oldContext, Context *runningContext);

    static void dispatch();

    static int exit();

    int start();
    
    int sleep(time_t);
    
    int wake();

    static uint64 timeSliceCounter;
};

#endif //OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP
