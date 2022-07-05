//
// Created by os on 7/5/22.
//

#include "../h/_kernel.hpp"
#include "../lib/mem.h"
#include "../h/opcodes.hpp"
#include "../h/tcb.hpp"
#include "../h/_thread.hpp"
#include "../lib/console.h"
#include "../h/_sem.hpp"

using Calls = uint64(**)(uint64*);
using Call = uint64(*)(uint64*);
using Body = void(*)(void*);

uint64 (**Kernel::calls)(uint64*) = nullptr;

uint64 Kernel::exec(uint64 *args)
{
    return calls[args[0]](args);
}

void Kernel::init()
{
    calls = (Calls)__mem_alloc(sizeof(Call) * 100);
    calls[MEM_ALLOC]		= &Kernel::__malloc;
    calls[MEM_FREE]			= &Kernel::__free;
    calls[THREAD_CREATE]	= &Kernel::__thr_create;
    calls[THREAD_EXIT]		= &Kernel::__thr_exit;
    calls[THREAD_DISPATCH]	= &Kernel::__thr_dispatch;
    calls[THREAD_PREPARE]	= &Kernel::__thr_prep;
    calls[THREAD_START]		= &Kernel::__thr_start;
    calls[SEM_OPEN]			= &Kernel::__sem_open;
    calls[SEM_CLOSE]		= &Kernel::__sem_close;
    calls[SEM_WAIT]		    = &Kernel::__sem_wait;
    calls[SEM_SIGNAL]	    = &Kernel::__sem_sig;
    calls[TIME_SLEEP]	    = &Kernel::__sleep;
    calls[GETC]			    = &Kernel::__getchar;
    calls[PUTC]			    = &Kernel::__putchar;
    calls[USER_MODE]		= &Kernel::__user;
    calls[SUPER_MODE]		= &Kernel::__super;
}

uint64 Kernel::__malloc(uint64 *args)
{
    size_t volatile size = args[1] * MEM_BLOCK_SIZE;
    return (uint64)__mem_alloc(size);
}

uint64 Kernel::__free(uint64 *args)
{
    void *ptr = (void*)args[1];
    return (uint64)__mem_free(ptr);
}

uint64 Kernel::__thr_dispatch(uint64*)
{
    TCB::timeSliceCounter = 0;
    TCB::dispatch();
    return 0;
}

uint64 Kernel::__thr_create(uint64 *args)
{
    thread_t *handle = (thread_t*)args[1];
    Body routine     = (Body)args[2];
    void *arg        = (void*)args[3];
    uint64 *stack    = (uint64*)args[4];

    _thread *t = new _thread(handle, routine, arg, stack);
    t->start();

    return 0;
}

uint64 Kernel::__thr_prep(uint64 *args)
{
    thread_t *handle = (thread_t*)args[1];
    Body routine     = (Body)args[2];
    void *arg        = (void*)args[3];
    uint64 *stack    = (uint64*)args[4];
    
    new _thread(handle, routine, arg, stack);
    
    return 0;
}

uint64 Kernel::__thr_start(uint64 *args)
{
    thread_t handle = (thread_t)args[1];
    return handle->start();
}

uint64 Kernel::__thr_exit(uint64*)
{
    return TCB::exit();
}

uint64 Kernel::__sem_open(uint64 *args)
{
    sem_t *handle = (sem_t*)args[1];
    unsigned init = (unsigned)args[2];
    
    new _sem(handle, init);
    
    return 0;
}

uint64 Kernel::__sem_close(uint64 *args)
{
    sem_t handle = (sem_t)args[1];
    return handle->close();
}

uint64 Kernel::__sem_wait(uint64 *args)
{
    sem_t handle = (sem_t)args[1];
    return handle->wait();
}

uint64 Kernel::__sem_sig(uint64 *args)
{
    sem_t handle = (sem_t)args[1];
    return handle->signal();
}

uint64 Kernel::__sleep(uint64 *args)
{
    time_t timeout = (time_t)args[1];
    return TCB::sleep(timeout);
}

uint64 Kernel::__getchar(uint64*)
{
    return __getc();
}

uint64 Kernel::__putchar(uint64 *args)
{
    __putc((char)args[1]);
    return 0;
}

uint64 Kernel::__user(uint64*)
{
    return 0;
}

uint64 Kernel::__super(uint64*)
{
    return 0;
}
