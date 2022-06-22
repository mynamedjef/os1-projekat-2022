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

inline uint64 retval()
{
    uint64 volatile ret;
    __asm__ volatile ("mv %0, a0" : "=r" (ret));
    return ret;
}

// ============= sistemski pozivi ==============

void thread_dispatch()
{
    invoke(THREAD_DISPATCH);
}
