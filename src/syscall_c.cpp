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

// učitava prva tri argumenta na svoja mesta za ABI poziv
inline void load_args() {
    __asm__ volatile ("mv a3, a2");
    __asm__ volatile ("mv a2, a1");
    __asm__ volatile ("mv a1, a0");
}

// dohvata povratnu vrednost sis. poziva
inline uint64 retval() {
	uint64 ret;
	__asm__ volatile ("mv %0, a0" : "=r" (ret));
	return ret;
}

// ---------- sistemski pozivi ----------

void *mem_alloc_wrapper(size_t block_cnt) {
    load_args();
    load_opcode(MEM_ALLOC);
    syscall();
    return (void*)retval();
}

void *mem_alloc(size_t size) {
    size_t block_cnt = size / MEM_BLOCK_SIZE;
    if (size % MEM_BLOCK_SIZE != 0) {
        block_cnt++;
    }
    return mem_alloc_wrapper(block_cnt);
}

int mem_free(void *ptr) {
    load_args();
    load_opcode(MEM_FREE);
    syscall();
    return (retval() == 0) ? 0 : -1;
}

void yield() {
	TCB::yield();
}


