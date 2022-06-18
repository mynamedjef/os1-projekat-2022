//
// Created by djafere on 6/17/22.
//

#include "../h/syscall_c.h"
#include "../lib/mem.h"
#include "../h/sys_opcodes.h"
#include "../h/tcb.hpp"

// zove sistemski poziv
inline void syscall() { __asm__ volatile ("ecall"); }

// učitava opcode u registar a0
inline void load_opcode(uint64 opcode) { __asm__ volatile ("mv a0, %0" : : "r" (opcode)); }

// učitava prvi argument sis. poziva u registar a0
inline void load_arg1(uint64 arg1) { __asm__ volatile ("mv a1, %0" : : "r" (arg1)); }

// učitava drugi argument sis. poziva u registar a0
inline void load_arg2(uint64 arg2) { __asm__ volatile ("mv a2, %0" : : "r" (arg2)); }

// učitava treći argument sis. poziva u registar a0
inline void load_arg3(uint64 arg3) { __asm__ volatile ("mv a3, %0" : : "r" (arg3)); }

// dohvata povratnu vrednost sis. poziva
inline uint64 retval() {
	uint64 ret;
	__asm__ volatile ("mv %0, a0" : "=r" (ret));
	return ret;
}

// ---------- sistemski pozivi ----------

void *mem_alloc(size_t size) {
    uint64 volatile s = size;
    load_opcode(MEM_ALLOC);
    load_arg1(s);
    syscall();
    uint64 volatile ret;
    __asm__ volatile ("mv %0, a0" : "=r" (ret));
    return (void*)ret;
}

void yield() {
	TCB::yield();
}


