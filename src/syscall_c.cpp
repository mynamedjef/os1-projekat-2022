//
// Created by djafere on 6/22/22.
//

#include "../h/syscall_c.h"
#include "../h/opcodes.hpp"

inline void invoke(uint64 opcode)
{
    __asm__ volatile ("mv a0, %0" : : "r" (opcode));
    __asm__ volatile ("ecall");
}

inline void load_args()
{
    __asm__ volatile ("mv a4, a3");
    __asm__ volatile ("mv a3, a2");
    __asm__ volatile ("mv a2, a1");
    __asm__ volatile ("mv a1, a0");
}

inline uint64 retval()
{
    uint64 volatile ret;
    __asm__ volatile ("mv %0, a0" : "=r" (ret));
    return ret;
}

// ============= sistemski pozivi ==============

// ----------------- memorija ------------------

void *mem_alloc(size_t size)
{
    if (!size) { return nullptr; }
    size_t blocks = (size % MEM_BLOCK_SIZE == 0) ?
                    size / MEM_BLOCK_SIZE :
                    1 + size / MEM_BLOCK_SIZE;
    __asm__ volatile ("mv a1, %0" : : "r" (blocks));
    invoke(MEM_ALLOC);
    return (void*)retval();
}

int mem_free(void *ptr)
{
    if (!ptr) { return 0; }
    load_args();
    invoke(MEM_FREE);
    return (retval() == 0) ? 0 : -1;
}

// ------------------- niti --------------------

int thread_create(thread_t *handle, void (*start_routine)(void*), void *arg)
{
    if (!handle) { return -1; }
    if (!start_routine) { return -2; }
    load_args();
    invoke(THREAD_CREATE);
    return (retval() == 0) ? 0 : -3;
}

int thread_prepare(thread_t *handle, void (*start_routine)(void*), void *arg)
{
    if (!handle) { return -1; }
    if (!start_routine) { return -2; }
    load_args();
    invoke(THREAD_PREPARE);
    return (retval() == 0) ? 0 : -3;
}

int thread_start(thread_t handle)
{
    if (!handle) { return -1; }
    load_args();
    invoke(THREAD_START);
    return (retval() == 0) ? 0 : -2;
}

int thread_exit()
{
    invoke(THREAD_EXIT);
    return (retval() == 0) ? 0 : -1;
}

void thread_dispatch()
{
    invoke(THREAD_DISPATCH);
}

// ovo nije zadato zadatkom, ali u C++ API postoji ~Thread() destruktor koji ne znam kako drugačije da se odradi.
// u suštini thread_delete čeka završetak niti koja se briše i onda je briše. može se i implementirati da npr. nasilno gasi nit
int thread_delete(thread_t handle)
{
    if (!handle) { return -1; }
    load_args();
    invoke(THREAD_DELETE);
    return (retval() == 0) ? 0 : -2;
}

// ---------------- semafori -------------------

int sem_open(sem_t *handle, unsigned init)
{
    if (!handle) { return -1; }
    load_args();
    invoke(SEM_OPEN);
    return (retval() == 0) ? 0 : -2;
}

int sem_close(sem_t handle)
{
    if (!handle) { return -1; }
    load_args();
    invoke(SEM_CLOSE);
    return (retval() == 0) ? 0 : -2;
}

int sem_wait(sem_t handle)
{
    if (!handle) { return -1; }
    load_args();
    invoke(SEM_WAIT);
    return (retval() == 0) ? 0 : -2;
}

int sem_signal(sem_t handle)
{
    if (!handle) { return -1; }
    load_args();
    invoke(SEM_SIGNAL);
    return (retval() == 0) ? 0 : -2;
}

// ------------------ ostalo -------------------

int time_sleep(time_t timeout)
{
    load_args();
    invoke(TIME_SLEEP);
    return (retval() == 0) ? 0 : -1;
}

char getc()
{
    invoke(GETC);
    return (char)retval();
}

void putc(char c)
{
    load_args();
    invoke(PUTC);
}
