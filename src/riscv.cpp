
#include "../h/riscv.hpp"
#include "../h/tcb.hpp"
#include "../lib/console.h"
#include "../test/printing.hpp"
#include "../h/opcodes.hpp"
#include "../h/_thread.hpp"
#include "../h/_sem.hpp"
#include "../h/_sleeplist.hpp"

_buffer *Riscv::bufin = nullptr;

_buffer *Riscv::bufout = nullptr;

void Riscv::init()
{
    bufin = new _buffer;
    bufout = new _buffer;
}

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

void Riscv::restorePrivilege()
{
    if (TCB::running->is_systhread()) {
        ms_sstatus(SSTATUS_SPP);
    } else {
        mc_sstatus(SSTATUS_SPP);
    }
}

inline void Riscv::unexpectedTrap()
{
    uint64 scause = r_scause();
    uint64 stval = r_stval();
    uint64 stvec = r_stvec();
    uint64 sepc = r_sepc();

    printString("scause: ");
    printInt(scause);
    printString("\n");

    printString("stval: ");
    printInt(stval);
    printString("\n");

    printString("stvec: ");
    printInt(stvec);
    printString("\n");

    printString("sepc: ");
    printInt(sepc);
    printString("\n");
}

using Body = void(*)(void*);

uint64 Riscv::syscall(uint64 *args)
{
    uint64 opcode = args[0];
    uint64 retval = 0;
    if (opcode == MEM_ALLOC)
    {
        size_t volatile size = args[1] * MEM_BLOCK_SIZE;
        retval = (uint64)__mem_alloc(size);
    }
    else if (opcode == MEM_FREE)
    {
        void *ptr = (void*)args[1];
        retval = (uint64)__mem_free(ptr);
    }
    else if (opcode == THREAD_DISPATCH)
    {
        TCB::timeSliceCounter = 0;
        TCB::dispatch();
    }
    else if (opcode == THREAD_CREATE || opcode == THREAD_PREPARE)
    {
        thread_t *handle = (thread_t*)args[1];
        Body routine     = (Body)args[2];
        void *arg        = (void*)args[3];
        uint64 *stack    = (uint64*)args[4];

        _thread *t = new _thread(handle, routine, arg, stack);
        if (opcode == THREAD_CREATE) { retval = t->start(); }
    }
    else if (opcode == THREAD_START)
    {
        thread_t handle = (thread_t)args[1];
        retval = handle->start();
    }
    else if (opcode == THREAD_EXIT)
    {
        int val = TCB::exit();
        retval = val;
    }
    else if (opcode == SEM_OPEN)
    {
        sem_t *handle = (sem_t*)args[1];
        unsigned init = (unsigned)args[2];

        new _sem(handle, init);
    }
    else if (opcode == SEM_CLOSE)
    {
        sem_t handle = (sem_t)args[1];
        retval = handle->close();
    }
    else if (opcode == SEM_WAIT)
    {
        sem_t handle = (sem_t)args[1];
        retval = handle->wait();
    }
    else if (opcode == SEM_SIGNAL)
    {
        sem_t handle = (sem_t)args[1];
        retval = handle->signal();
    }
    else if (opcode == TIME_SLEEP)
    {
        time_t timeout = (time_t)args[1];
        retval = TCB::sleep(timeout);
    }
    else if (opcode == GETC)
    {
        char c = bufin->kernel_get();
        retval = ((uint64)c);
    }
    else if (opcode == PUTC)
    {
        bufout->kernel_put((char)args[1]);
    }
    return retval;
}

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

        w_retval(syscall(args));
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
            TCB::timeSliceCounter = 0;
            TCB::dispatch();
            w_sepc(sepc);
        }
        mc_sip(SIP_SSIP);
    }
    else if (scause == HARDWARE)
    {
        // interrupt: yes; cause code: supervisor external interrupt (PLIC; could be keyboard)
        static int IRQ_CONSOLE = 10;
        int irq = plic_claim();
        if (irq == IRQ_CONSOLE)
        {
            volatile char status = *((char*)CONSOLE_STATUS);
            while (status & CONSOLE_RX_STATUS_BIT){
                char c = (*(char*)CONSOLE_RX_DATA);
                bufin->kernel_put(c);
                status = *((char*)CONSOLE_STATUS);
            }
        }
        plic_complete(irq);
    }
    else
    {
        // unexpected trap cause
        unexpectedTrap();
    }
}
