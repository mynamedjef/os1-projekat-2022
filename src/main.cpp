//
// Created by djafere
//

#include "../h/tcb.hpp"
#include "../h/printing.hpp"
#include "../h/riscv.hpp"
#include "../h/userMain.hpp"
#include "../h/_thread.hpp"

int main()
{
    printString("main() started\n");

    TCB *kernel = TCB::kernelThread();

    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);
    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);

    thread_t user;
    thread_create(&user, userMain, (void*)1337);

    while (!user->isFinished()) {
        thread_dispatch();
    }

    delete kernel;
    delete user;
    printString("main() finished\n");

    return 0;
}
