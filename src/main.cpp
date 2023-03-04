//
// Created by djafere
//

#include "../h/tcb.hpp"
#include "../test/printing.hpp"
#include "../h/riscv.hpp"
#include "../h/_sleeplist.hpp"

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
    kmem_init((void*)HEAP_START_ADDR, (void*)HEAP_END_ADDR);

    Riscv::init();

    TCB::kernelThread();
    TCB::idleThread();
    TCB::outputThread();

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

    // onemogućavamo prekide od tajmera za slučaj da je neka korisnička nit zalutala
    Riscv::mc_sstatus(Riscv::SSTATUS_SIE);
    // u slučaju da nakon userMain() neka korisnička nit nije završila, nasilno je gasimo
    Scheduler::flush_user_threads();

    printString("main() cleaning up\n");
    while (Riscv::bufout->count() > 0) { thread_dispatch(); } // čekanje da se ispiše sve iz bafera ako već nije

    // oslobađanje sve memorije
    kmem_buffers_destroy();
    kmem_cache_destroy(SleepNode::cachep);
    kmem_cache_destroy(_sem::cachep);
    kmem_cache_destroy(_buffer::cachep);
    kmem_cache_destroy(List<TCB>::cachep);
    kmem_cache_destroy(TCB::cachep);
    kmem_cache_destroy(_stack::cachep);

    return 0;
}
