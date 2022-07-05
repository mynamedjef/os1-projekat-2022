//
// Created by marko on 20.4.22..
//

#ifndef OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP
#define OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP

#include "../lib/hw.h"
#include "../lib/mem.h"

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

    ~TCB() { delete[] stack; }

    bool isFinished() const { return status == FINISHED; }

    void setStatus(Status st) { status = st; }

    uint64 getTimeSlice() const { return timeSlice; }

    using Body = void (*)(void*);

    // pravi nit i započinje je odmah
    static TCB *createThread(Body, void*, uint64*);

    // pravi nit koja čeka da bude pokrenuta sa .start()
    static TCB *initThread(Body, void*, uint64*);

    static TCB *idleThread();

    static TCB *kernelThread();

    static TCB *running;

    static TCB *kernel;

    static TCB *idle;

    int start();

    void *operator new(size_t size) { return __mem_alloc(size); }

    void operator delete(void *ptr) { __mem_free(ptr); }

private:
    TCB(Body body, void *arg, uint64 *stack, bool main=false) :
            body(body),
            arg(arg),
            stack(stack),
            timeSlice(DEFAULT_TIME_SLICE)
    {
        if (!main)
        {
            context = {(uint64) &threadWrapper,
                       (uint64) &stack[DEFAULT_STACK_SIZE]};
            status = CREATED;
        }
        else
        {
            context = {0, 0};
            status = RUNNING;
        }
    }

    TCB() : TCB(nullptr, nullptr, nullptr, true) { }

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

    friend class Kernel;

    friend class Riscv;

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

    static void idleWrapper(void*);

    static uint64 timeSliceCounter;
};

#endif //OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP
