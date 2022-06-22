//
// Created by marko on 20.4.22..
//

#include "../h/tcb.hpp"
#include "../h/printing.hpp"
#include "../h/riscv.hpp"
#include "../h/userMain.hpp"

int main()
{
    printString("main() started\n");

    TCB *kernel = TCB::kernel = TCB::running =
            TCB::createThread(nullptr);
    TCB *user = TCB::createThread(userMain);

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
