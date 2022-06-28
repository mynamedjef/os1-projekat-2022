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

void invoke(uint64 opcode)
{
    __asm__ volatile ("mv a0, %0" : : "r" (opcode));
    __asm__ volatile ("ecall");
}

void user_wrapper(void*)
{
    printString("userMain() started\n");
    userMain();
    printString("userMain() finished\n");
}

int main()
{
    MemoryAllocator::init_memory();

    TCB *kernel = TCB::kernelThread();
    TCB *idle = TCB::idleThread();

    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);
    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);

    printString("main() started\n");

    // prelazak u korisnički režim
    invoke(USER_MODE);

    thread_t user;
    thread_create(&user, user_wrapper, nullptr);

    while (!user->isFinished()) {
        thread_dispatch();
    }

    invoke(SUPER_MODE);
    // povratak u administratorski režim

    Riscv::mc_sstatus(Riscv::SSTATUS_SIE);

    delete kernel;
    delete user;
    delete idle;
    printString("main() finished\n");

    return 0;
}
