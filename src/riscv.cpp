
#include "../h/riscv.hpp"
#include "../h/tcb.hpp"
#include "../lib/console.h"
#include "../test/printing.hpp"
#include "../h/opcodes.hpp"
#include "../h/_sem.hpp"
#include "../h/_sleeplist.hpp"
#include "../h/_kernel.hpp"

void Riscv::popSppSpie()
{
    __asm__ volatile ("csrw sepc, ra");
    __asm__ volatile ("sret");
}

enum Interrupts: uint64 {
    SOFTWARE    = 0x8000000000000001UL,
    ECALL_SUPER = 0x0000000000000009UL,
    ECALL_USER  = 0x0000000000000008UL,
    HARDWARE    = 0x8000000000000009UL
};

inline void Riscv::unexpectedTrap()
{
//    uint64 scause = r_scause();
//    uint64 stval = r_stval();
//    uint64 stvec = r_stvec();
//    uint64 sepc = r_sepc();

    printString("scause: ");
//    printLong(scause);
    printString("\n");

    printString("stval: ");
//    printLong(stval);
    printString("\n");

    printString("stvec: ");
//    printLong(stvec);
    printString("\n");

    printString("sepc: ");
//    printLong(sepc);
    printString("\n");
}

using Body = void(*)(void*);

void Riscv::handleSupervisorTrap()
{
    // argumenti se ovde učitavaju jer if-ovi pregaze neke registre (npr. a3,a4)
    uint64 args[5];
    loadParams(args);

    uint64 scause = r_scause();
    if (scause == ECALL_USER || scause == ECALL_SUPER)
    {
        // interrupt: no; cause code: environment call from U-mode(8) or S-mode(9)
        uint64 sepc = r_sepc() + 4;
        uint64 sstatus = r_sstatus();

        uint64 ret = Kernel::exec(args);
        if (args[0] == USER_MODE)  { sstatus &= ~(SSTATUS_SPP); }
        if (args[0] == SUPER_MODE) { sstatus |= SSTATUS_SPP; }

        w_retval(ret);
        w_sstatus(sstatus);
        w_sepc(sepc);
    }
    else if (scause == SOFTWARE)
    {
        // interrupt: yes; cause code: supervisor software interrupt (CLINT; machine timer interrupt)
        TCB::timeSliceCounter++;
        _sleeplist::tick(); // proverava da li je vreme da se neke niti probude, i ako jeste budi ih
        if (TCB::timeSliceCounter >= TCB::running->getTimeSlice())
        {
            uint64 sepc = r_sepc();
            uint64 sstatus = r_sstatus();

            uint64 arg = THREAD_DISPATCH;
            Kernel::exec(&arg);

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
