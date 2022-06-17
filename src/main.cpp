//
// Created by diafere on 6/16/22.
//

#include "../h/tcb.hpp"
#include "../h/workers.hpp"
#include "../h/print.hpp"
#include "../h/riscv.hpp"
#include "../h/userMain.hpp"

inline void sys_call() {
    __asm__ volatile ("ecall");
}

int main()
{
    TCB *kernel = TCB::createThread(nullptr);
    TCB *user = TCB::createThread(userMain);
    TCB::running = kernel;

    Riscv::w_stvec((uint64) &Riscv::userModeTrap);
    sys_call();

    printString("ok...\n");

    while (!user->isFinished()) {
        TCB::yield();
    }

    delete user;
    delete kernel;
    printString("main() finished\n");

    return 0;
}
