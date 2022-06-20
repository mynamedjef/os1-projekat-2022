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
    __asm__ volatile ("mv a4, a3");
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

void *mem_alloc_wrapper(size_t block_cnt) {
    load_args();
    load_opcode(MEM_ALLOC);
    syscall();
    return (void*)retval();
}

int thread_create_wrapper(thread_t *handle, void(*start_routine)(void*), void *arg, void *stack_space) {
    if (!handle) {
        return -1;
    }

    load_args();
    load_opcode(THREAD_CREATE);
    syscall();
    return (retval() == 0) ? 0 : -2;
}

int thread_init_wrapper(thread_t *handle, void(*start_routine)(void*), void *arg, void *stack_space) {
    if (!handle) {
        return -1;
    }

    load_args();
    load_opcode(THREAD_INIT);
    syscall();
    return (retval() == 0) ? 0 : -2;
}

// ========== sistemski pozivi ==========

// ---------- memorija ------------------

void *mem_alloc(size_t size) {
    size_t block_cnt = size / MEM_BLOCK_SIZE;
    if (size % MEM_BLOCK_SIZE != 0) {
        block_cnt++;
    }
    return mem_alloc_wrapper(block_cnt);
}

int mem_free(void *ptr) {
    if (ptr == nullptr) {
        return -1;
    }

    load_args();
    load_opcode(MEM_FREE);
    syscall();
    return (retval() == 0) ? 0 : -2;
}

// -------------- niti ------------------

int thread_create(thread_t *handle, void(*start_routine)(void*), void *arg) {
    if (handle == nullptr) { return -1; }
    void *stack_space = mem_alloc(DEFAULT_STACK_SIZE);
    if (stack_space == nullptr) { return -2; }

    return thread_create_wrapper(handle, start_routine, arg, stack_space);
}

int thread_init(thread_t *handle, void(*start_routine)(void*), void *arg) {
    if (handle == nullptr) { return -1; }
    void *stack_space = mem_alloc(DEFAULT_STACK_SIZE);
    if (stack_space == nullptr) { return -2; }
    
    return thread_init_wrapper(handle, start_routine, arg, stack_space);
}

int thread_exit() {
    load_opcode(THREAD_EXIT);
    syscall();
    return (retval() == 0) ? 0 : -1;
}

void thread_dispatch() {
    load_opcode(THREAD_DISPATCH);
    syscall();
}

// -------------- semafori --------------

int sem_open(sem_t *handle, unsigned init) {
    if (!handle) {
        return -1;
    }

    load_args();
    load_opcode(SEM_OPEN);
    syscall();
    return (retval() == 0) ? 0 : -2;
}

int sem_close(sem_t id) {
    if (!id) {
        return -1;
    }

    load_args();
    load_opcode(SEM_CLOSE);
    syscall();
    return (retval() == 0) ? 0 : -2;
}

int sem_signal(sem_t id) {
    if (!id) {
        return -1;
    }

    load_args();
    load_opcode(SEM_SIGNAL);
    syscall();
    return (retval() == 0) ? 0 : -2;
}
