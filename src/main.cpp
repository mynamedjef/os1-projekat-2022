//
// Created by diafere on 6/16/22.
//

#include "../h/tcb.hpp"
#include "../h/workers.hpp"
#include "../h/print.hpp"
#include "../h/riscv.hpp"
#include "../h/userMain.hpp"
#include "../h/syscall_c.h"
#include "../lib/mem.h"

// postavlja prekidnu rutinu na userModeTrap i aktivira je za prelazak u korisnički režim
inline void userMode() {
	Riscv::w_stvec((uint64) &Riscv::userModeTrap);
	__asm__ volatile ("ecall");
}

TCB *kernel_thread()
{
    return TCB::createThread(nullptr, nullptr, nullptr);
}

TCB *user_thread()
{
    uint64 *stack = new uint64[DEFAULT_STACK_SIZE];
    return TCB::createThread(userMain, stack, nullptr);
}

int main()
{
    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);
    
    TCB *kernel = TCB::running = kernel_thread();
    TCB *user = user_thread();

    userMode();

    printString("user mode initiated...\n");

    while (!user->isFinished()) {
        TCB::yield();
    }

    delete user;
    delete kernel;
    printString("main() finished\n");

    return 0;
}
