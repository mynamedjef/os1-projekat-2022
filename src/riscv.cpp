
#include "../h/riscv.hpp"
#include "../h/tcb.hpp"
#include "../lib/console.h"
#include "../h/printing.hpp"
#include "../h/opcodes.hpp"

void Riscv::popSppSpie()
{
    mc_sstatus(SSTATUS_SPP);
    __asm__ volatile ("csrw sepc, ra");
    __asm__ volatile ("sret");
}

enum Interrupts: uint64 {
    SOFTWARE    = 0x8000000000000001UL,
    ECALL_SUPER = 0x0000000000000009UL,
    ECALL_USER  = 0x0000000000000008UL,
    HARDWARE    = 0x8000000000000009UL
};

uint64 Riscv::restorePrivilege(uint64 sstatus)
{
    return (TCB::kernel == TCB::running) ?
        sstatus | SSTATUS_SPP :
        sstatus & ~(SSTATUS_SPP);
}

inline void Riscv::unexpectedTrap()
{
    uint64 scause = r_scause();
    uint64 stval = r_stval();
    uint64 stvec = r_stvec();
    uint64 sepc = r_sepc();

    printString("scause: ");
    printLong(scause);
    printString("\n");

    printString("stval: ");
    printLong(stval);
    printString("\n");

    printString("stvec: ");
    printLong(stvec);
    printString("\n");

    printString("sepc: ");
    printLong(sepc);
    printString("\n");
}

void Riscv::handleSupervisorTrap()
{
    uint64 scause = r_scause();
    if (scause == ECALL_USER || scause == ECALL_SUPER)
    {
        // interrupt: no; cause code: environment call from U-mode(8) or S-mode(9)
        uint64 sepc = r_sepc() + 4;
        uint64 sstatus = r_sstatus();

        uint64 opcode = r_opcode();
        if (opcode == THREAD_DISPATCH)
        {
            TCB::timeSliceCounter = 0;
            TCB::dispatch();
            sstatus = restorePrivilege(sstatus);
        }
        w_sstatus(sstatus);
        w_sepc(sepc);
    }
    else if (scause == SOFTWARE)
    {
        // interrupt: yes; cause code: supervisor software interrupt (CLINT; machine timer interrupt)
        TCB::timeSliceCounter++;
        if (TCB::timeSliceCounter >= TCB::running->getTimeSlice())
        {
            uint64 sepc = r_sepc();
            uint64 sstatus = r_sstatus();
            TCB::timeSliceCounter = 0;
            TCB::dispatch();
            sstatus = restorePrivilege(sstatus);
            w_sstatus(sstatus);
            w_sepc(sepc);
        }
        mc_sip(SIP_SSIP);
    }
    else if (scause == HARDWARE)
    {
        // interrupt: yes; cause code: supervisor external interrupt (PLIC; could be keyboard)
        console_handler();
    }
    else
    {
        // unexpected trap cause
        unexpectedTrap();
    }
}
