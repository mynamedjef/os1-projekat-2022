//
// Created by djafere
//

#include "../h/tcb.hpp"
#include "../test/printing.hpp"
#include "../h/riscv.hpp"
#include "../h/userMain.hpp"
#include "../h/_thread.hpp"
#include "../h/_sleeplist.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/opcodes.hpp"
#include "../h/_kernel.hpp"

void invoke(uint64 opcode)
{
    __asm__ volatile ("mv a0, %0" : : "r" (opcode));
    __asm__ volatile ("ecall");
}

void user_wrapper(void *sem)
{
    invoke(USER_MODE);
    printString("userMain() started\n");
    userMain();
    printString("userMain() finished\n");
    sem_signal((sem_t)sem);
    invoke(SUPER_MODE);
}

int main()
{
    MemoryAllocator::init_memory();
    Kernel::init();

    TCB *kernel = TCB::kernelThread();
    TCB *idle = TCB::idleThread();

    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);
    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);

    printString("main() started\n");

    // ------------- korisnički kod --------------
    sem_t user_sem;
    sem_open(&user_sem, 0);

    thread_t user;
    thread_create(&user, user_wrapper, user_sem);

    sem_wait(user_sem);
    sem_close(user_sem);
    // ----------- kraj korisničkog koda ---------

    Riscv::mc_sstatus(Riscv::SSTATUS_SIE);

    delete kernel;
    delete user;
    delete idle;
    printString("main() finished\n");

    return 0;
}
