//
// Created by diafere on 6/16/22.
//

#include "../h/tcb.hpp"
#include "../h/workers.hpp"
#include "../h/print.hpp"
#include "../h/riscv.hpp"
#include "../h/userMain.hpp"
#include "../h/syscall_c.h"

// postavlja prekidnu rutinu na userModeTrap i aktivira je za prelazak u korisnički režim
inline void userMode() {
	Riscv::w_stvec((uint64) &Riscv::userModeTrap);
	__asm__ volatile ("ecall");
}

int main()
{
    TCB *kernel = TCB::createThread(nullptr);
    TCB *user = TCB::createThread(userMain);
    TCB::running = kernel;
	
    userMode();

    printString("ok...\n");

    while (!user->isFinished()) {
        TCB::yield();
    }

    delete user;
    delete kernel;
    printString("main() finished\n");

    return 0;
}
