//
// Created by djafere
//

#include "../h/tcb.hpp"
#include "../h/printing.hpp"
#include "../h/riscv.hpp"
#include "../h/userMain.hpp"

int main()
{
    printString("main() started\n");

    TCB *kernel = TCB::kernelThread();
    TCB *user = TCB::createThread(userMain, nullptr, new uint64[DEFAULT_STACK_SIZE]);

    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);
    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);

    while (!user->isFinished()) {
        thread_dispatch();
    }

    delete kernel;
    delete user;
    printString("main() finished\n");

    return 0;
}
