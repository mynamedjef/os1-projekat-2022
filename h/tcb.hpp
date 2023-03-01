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
    enum Status
    {
        FINISHED,
        RUNNING,
        READY,
        CREATED,
        WAITING,
        SLEEPING,
        IDLE
    };

    ~TCB()
    {
        __mem_free(stack);
    }

    bool isFinished() const { return status == FINISHED; }

    bool is_systhread() const { return sys_thread; }

    void setStatus(Status st) { status = st; }

    uint64 getTimeSlice() const { return timeSlice; }

    using Body = void (*)(void*);

    // pravi nit i započinje je odmah
    static TCB *createThread(Body, void*, uint64*);

    // pravi nit koja čeka da bude pokrenuta sa .start()
    static TCB *initThread(Body, void*, uint64*);

    static TCB *idleThread();

    static TCB *kernelThread();

    static TCB *outputThread();

    static TCB *running;

    static TCB *kernel;

    static TCB *idle;

    static TCB *output;

    int start();

    void *operator new(size_t size) { return __mem_alloc(size); }

    void operator delete(void *ptr) { __mem_free(ptr); }

private:
    // ako je body == nullptr, nit u pitanju je main nit. samim tim automatski je RUNNING. isto tako kontekst/stek su joj null
    TCB(Body body, void *arg, uint64 *stack) :
            body(body),
            arg(arg),
            stack(stack),
            timeSlice(DEFAULT_TIME_SLICE),
            status(CREATED),
            sys_thread(false),
            next(nullptr)
    {
        if (body != nullptr)
        {
            context = {(uint64) &threadWrapper,
                       (uint64) &stack[DEFAULT_STACK_SIZE]};
        }
        else
        {
            context = {0, 0};
            status = RUNNING;
        }
    }

    struct Context
    {
        uint64 ra;
        uint64 sp;
    };

    Body body;
    void *arg;
    uint64 *stack;
    Context context;
    uint64 timeSlice;
    Status status;
    bool sys_thread;

    TCB *next;  // potrebno za scheduler

    friend class Riscv;

    friend class Scheduler;

    friend class _sem;

    friend class _sleeplist;

    static void threadWrapper();

    static void contextSwitch(Context *oldContext, Context *runningContext);

    static void dispatch();

    // gasi trenutnu nit
    static int exit();

    // trenutna nit čeka na semaforu
    static int wait();

    // nit se otpušta sa čekanja sa semafora
    int release();

    // uspavljuje trenutnu nit na time_t otkucaja tajmera
    static int sleep(time_t);
    
    // budi nit
    int wake();

    // ubacuje nit u scheduler tj. sprema je za rad
    void ready();

    static void idleWrapper(void*);

    static uint64 timeSliceCounter;
};

#endif //OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP
