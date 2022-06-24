
#include "../h/riscv.hpp"
#include "../h/tcb.hpp"
#include "../lib/console.h"
#include "../h/printing.hpp"
#include "../h/opcodes.hpp"
#include "../h/_thread.hpp"
#include "../h/_sem.hpp"
#include "../h/_sleeplist.hpp"

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

        uint64 opcode = args[0];
        if (opcode == MEM_ALLOC)
        {
            size_t volatile size = args[1] * MEM_BLOCK_SIZE;
            void *ret = __mem_alloc(size);
            w_retval((uint64)ret);
        }
        else if (opcode == MEM_FREE)
        {
            void *ptr = (void*)args[1];
            w_retval((uint64)__mem_free(ptr));
        }
        else if (opcode == THREAD_DISPATCH)
        {
            TCB::timeSliceCounter = 0;
            TCB::dispatch();
            sstatus = restorePrivilege(sstatus);
        }
        else if (opcode == THREAD_CREATE)
        {
            thread_t *handle = (thread_t*)args[1];
            Body routine     = (Body)args[2];
            void *arg        = (void*)args[3];
            uint64 *stack    = (uint64*)args[4];

            _thread *t = new _thread(handle, routine, arg, stack);
            t->start();

            w_retval(0);
        }
        else if (opcode == THREAD_PREPARE)
        {
            thread_t *handle = (thread_t*)args[1];
            Body routine     = (Body)args[2];
            void *arg        = (void*)args[3];
            uint64 *stack    = (uint64*)args[4];

            new _thread(handle, routine, arg, stack);

            w_retval(0);
        }
        else if (opcode == THREAD_START)
        {
            thread_t handle = (thread_t)args[1];
            w_retval(handle->start());
        }
        else if (opcode == THREAD_EXIT)
        {
            int val = TCB::exit();
            sstatus = restorePrivilege(sstatus);
            w_retval(val);
        }
        else if (opcode == SEM_OPEN)
        {
            sem_t *handle = (sem_t*)args[1];
            unsigned init = (unsigned)args[2];

            new _sem(handle, init);

            w_retval(0);
        }
        else if (opcode == SEM_CLOSE)
        {
            sem_t handle = (sem_t)args[1];
            w_retval(handle->close());
        }
        else if (opcode == SEM_WAIT)
        {
            sem_t handle = (sem_t)args[1];
            int ret = handle->wait();
            sstatus = restorePrivilege(sstatus);
            w_retval(ret);
        }
        else if (opcode == SEM_SIGNAL)
        {
            sem_t handle = (sem_t)args[1];
            w_retval(handle->signal());
        }
        else if (opcode == TIME_SLEEP)
        {
            time_t timeout = (time_t)args[1];
            int ret = TCB::sleep(timeout);
            sstatus = restorePrivilege(sstatus);
            w_retval(ret);
        }

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
