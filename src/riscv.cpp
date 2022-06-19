//
// Created by marko on 20.4.22..
//

#include "../h/riscv.hpp"
#include "../h/tcb.hpp"
#include "../lib/console.h"
#include "../h/sys_opcodes.h"
#include "../lib/mem.h"
#include "../h/_thread.hpp"

uint64 Riscv::EXCEPTION_TIMER       = 0x8000000000000001UL;
uint64 Riscv::EXCEPTION_HARDWARE    = 0x8000000000000009UL;
uint64 Riscv::EXCEPTION_USER_ECALL  = 0x0000000000000008UL;
uint64 Riscv::EXCEPTION_SUPER_ECALL = 0x0000000000000009UL;

using Body = void (*)(void*);
using thread_t = _thread*;

void Riscv::popSppSpie() {
    __asm__ volatile ("csrw sepc, ra");
    __asm__ volatile ("sret");
}

inline void Riscv::genericException() {
    uint64 volatile scause = r_scause();
    uint64 volatile sepc = r_sepc();
    uint64 volatile stvec = r_stvec();
    uint64 volatile stval = r_stval();
    printString("scause:\t");
    printInteger(scause);
    printString("\n");
    printString("sepc:\t");
    printInteger(sepc);
    printString("\n");
    printString("stvec:\t");
    printInteger(stvec);
    printString("\n");
    printString("stval:\t");
    printInteger(stval);
    printString("\n");
}

void Riscv::handleSupervisorTrap() {
    uint64 *arg4 = (uint64*)r_arg4(); // iz nekog razloga se tokom if-a prebriše a4...pa se čuva ovde
    uint64 scause = r_scause();
    if (scause == EXCEPTION_USER_ECALL || scause == EXCEPTION_SUPER_ECALL) { // zvaće se iz super samo na početku kernel main-a, za alokaciju resursa
        // interrupt: no; cause code: environment call from U-mode(8)
        uint64 volatile sepc = r_sepc() + 4;
        uint64 volatile sstatus = r_sstatus();

        uint64 volatile opcode = r_opcode();
        if (opcode == YIELD || opcode == THREAD_DISPATCH) // void yield() || void thread_dispatch()
        {
            TCB::timeSliceCounter = 0;
            TCB::dispatch();
        }
        else if (opcode == MEM_ALLOC) // void *mem_alloc(size_t size)
        {
            size_t volatile a1 = r_arg1() * MEM_BLOCK_SIZE; // size_t size
            w_retval((uint64)__mem_alloc(a1));
        }
        else if (opcode == MEM_FREE) // int mem_free(void*)
        {
            size_t volatile a1 = r_arg1();
            w_retval((uint64)__mem_free((void*)a1));
        }
        else if (opcode == THREAD_CREATE || opcode == THREAD_INIT) // int thread_create(thread_t handle, void (*start_routine)(void*), void *arg, void *stack_space)
        {
            thread_t *handle = (thread_t*)r_arg1();
            Body start_routine = (Body)r_arg2();
            void *arg = (void*)r_arg3();
            uint64 *stack_space = arg4;

            _thread *t = new _thread(handle, start_routine, stack_space, arg);
            if (opcode == THREAD_CREATE) { t->start(); }
            w_retval(0);
        }
        else if (opcode == THREAD_EXIT) // int thread_exit()
        {
            w_retval(_thread::exit());
        }

        w_sstatus(sstatus);
        w_sepc(sepc);
    }
    else if (scause == EXCEPTION_TIMER) {
        // interrupt: yes; cause code: supervisor software interrupt (CLINT; machine timer interrupt)
        TCB::timeSliceCounter++;
        if (TCB::timeSliceCounter >= TCB::running->getTimeSlice())
        {
            uint64 sepc = r_sepc();
            uint64 sstatus = r_sstatus();
            TCB::timeSliceCounter = 0;
            TCB::dispatch();
            w_sstatus(sstatus);
            w_sepc(sepc);
        }
        mc_sip(SIP_SSIP);
    }
    else if (scause == EXCEPTION_HARDWARE) {
        // interrupt: yes; cause code: supervisor external interrupt (PLIC; could be keyboard)
        console_handler();
    }
    else {
        // unexpected trap cause
        genericException();
    }
}
void Riscv::handleUserModeTrap() {
    uint64 scause = r_scause();
    if (scause == EXCEPTION_SUPER_ECALL) {
        // interrupt: no; cause code: environment call from S-mode(9)
        uint64 volatile sepc = r_sepc() + 4;
        uint64 volatile sstatus = r_sstatus();

        sstatus |= SSTATUS_SPIE;
        sstatus &= ~(SSTATUS_SPP);

        w_sstatus((uint64)sstatus);
        w_stvec((uint64) &supervisorTrap);

        w_sepc(sepc);
    }
    else if (scause == EXCEPTION_TIMER) {
        mc_sip(SIP_SSIP);
    }
    else if (scause == EXCEPTION_HARDWARE) {
        // interrupt: yes; cause code: supervisor external interrupt (PLIC; could be keyboard)
        console_handler();
    }
    else {
        // unexpected trap cause, undefined behaviour
        genericException();
    }
}
