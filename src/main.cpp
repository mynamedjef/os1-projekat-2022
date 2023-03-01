//
// Created by djafere
//

#include "../h/tcb.hpp"
#include "../test/printing.hpp"
#include "../h/riscv.hpp"
#include "../h/_sleeplist.hpp"
#include "../h/MemoryAllocator.hpp"

extern void userMain();

void user_wrapper(void *sem)
{
    printString("userMain() started\n");
    userMain();
    printString("userMain() finished\n");
    sem_signal((sem_t)sem);
}

int main()
{
    MemoryAllocator::init_memory();
    Riscv::init();

    TCB *kernel = TCB::kernelThread();
    TCB *idle = TCB::idleThread();
    TCB *output = TCB::outputThread();

    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);
    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);

    printString("main() started\n");

    // paljenje korisnika
    sem_t user_sem;
    sem_open(&user_sem, 0);

    thread_t user;
    thread_create(&user, user_wrapper, user_sem);

    // čekanje korisnika
    sem_wait(user_sem);

    printString("main() cleaning up\n");
    while (Riscv::bufout->count() > 0) { thread_dispatch(); } // čekanje da se ispiše sve iz bafera ako već nije
    Riscv::mc_sstatus(Riscv::SSTATUS_SIE);

    // oslobađamo semafor na kome je glavna kernel nit čekala korisnika
    delete user_sem;

    // oslobađamo bafere za standardni i/o
    delete Riscv::bufout;
    delete Riscv::bufin;

    // oslobađamo sve niti koje smo napravili
    delete user;
    delete idle;
    delete output;

    // oslobađamo glavnu nit na ovaj način jer bi običan delete pokušao da obriše i stek glavne niti
    __mem_free((void*)kernel);
    return 0;
}
