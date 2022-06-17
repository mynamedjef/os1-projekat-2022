//
// Created by marko on 20.4.22..
//

#include "../h/riscv.hpp"
#include "../h/tcb.hpp"
#include "../lib/console.h"
#include "../h/sys_opcodes.h"

uint64 Riscv::EXCEPTION_TIMER       = 0x8000000000000001UL;
uint64 Riscv::EXCEPTION_HARDWARE    = 0x8000000000000009UL;
uint64 Riscv::EXCEPTION_USER_ECALL  = 0x0000000000000008UL;
uint64 Riscv::EXCEPTION_SUPER_ECALL = 0x0000000000000009UL;

void Riscv::popSppSpie() {
    __asm__ volatile ("csrw sepc, ra");
    __asm__ volatile ("sret");
}

inline void Riscv::processSyscall() {
    uint64 volatile a0 = r_opcode();
    if (a0 == YIELD) {
        TCB::timeSliceCounter = 0;
        TCB::dispatch();
    }
}

inline void Riscv::genericException() {
    uint64 volatile scause = r_scause();
    uint64 volatile sepc = r_sepc();
//        uint64 stvec = r_stvec();
    uint64 volatile stval = r_stval();
    printString("scause:\t");
    printInteger(scause);
    printString("\n");
    printString("sepc:\t");
    printInteger(sepc);
    printString("\n");
    printString("stvec:\t");
    printInteger(stval);
    printString("\n");
}

void Riscv::handleSupervisorTrap() {
    uint64 scause = r_scause();
    if (scause == EXCEPTION_USER_ECALL) {
        // interrupt: no; cause code: environment call from U-mode(8)
        uint64 volatile sepc = r_sepc() + 4;
        uint64 volatile sstatus = r_sstatus();

        processSyscall();

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
