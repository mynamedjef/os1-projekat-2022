//
// Created by diafere on 6/16/22.
//

#include "../h/tcb.hpp"
#include "../h/workers.hpp"
#include "../h/print.hpp"
#include "../h/riscv.hpp"
#include "../h/userMain.hpp"

// postavlja prekidnu rutinu na userModeTrap i aktivira je za prelazak u korisnički režim
inline void userMode() {
	Riscv::w_stvec((uint64) &Riscv::userModeTrap);
	__asm__ volatile ("ecall");
}

TCB *kernel_thread()
{
    return TCB::createThread(nullptr, nullptr, nullptr, false);
}

TCB *user_thread()
{
    uint64 *stack = new uint64[DEFAULT_STACK_SIZE];
    return TCB::createThread(userMain, stack, nullptr, false);
}

void idling(void*) { while(true) {} }

TCB *idle_thread()
{
    uint64 *stack = new uint64[DEFAULT_STACK_SIZE];
    return TCB::idleThread(idling, stack);
}


int main()
{
    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);
    
    TCB *kernel = TCB::running = kernel_thread();
    TCB *idle = TCB::idle = idle_thread();
    TCB *user = user_thread();

    userMode();

    printString("user mode initiated...\n");

    while (!user->isFinished()) {
        TCB::yield();
    }

    TCB::idle = nullptr;
    delete idle;
    delete user;
    TCB::running = nullptr;
    delete kernel;
    printString("main() finished\n");

    return 0;
}
