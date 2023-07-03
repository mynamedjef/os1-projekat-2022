//
// Created by djafere
//

#include "../h/tcb.hpp"
#include "../test/printing.hpp"
#include "../h/riscv.hpp"
#include "../h/_sleeplist.hpp"

extern void os2_usermain2(); // pošto se pokreće u sistemskom modu

extern void userMain();

// testiranje 2. testa za os2
const bool TEST_OS2_2 = false;

void user_wrapper(void *sem)
{
    printString("userMain() started\n");
    userMain();
    printString("userMain() finished\n");
    sem_signal((sem_t)sem);
}

void wait_for_user_main()
{
    sem_t user_sem;
    sem_open(&user_sem, 0);

    thread_t user;
    thread_create(&user, user_wrapper, user_sem);

    // čekanje korisnika
    sem_wait(user_sem);

    // oslobađamo userMain() nit
    delete user;
    // oslobađamo semafor na kome je glavna kernel nit čekala korisnika
    delete user_sem;
}

int main()
{
    kmem_init((void*)HEAP_START_ADDR, (void*)HEAP_END_ADDR);

    Riscv::init();

    // pravljenje potrebnih sistemskih niti
    TCB::kernelThread();
    TCB::idleThread();
    TCB::outputThread();

    // Riscv::supervisorTrap se izvršava u slučaju prekida
    Riscv::w_stvec((uint64) &Riscv::supervisorTrap);
    // omogućavaju se prekidi
    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);

    printString("main() started\n");

    if (!TEST_OS2_2) {
        // paljenje korisnika i čekanje da završi
        wait_for_user_main();
    } else {
        os2_usermain2();
    }

    // onemogućavamo prekide od tajmera za slučaj da je neka korisnička nit zalutala
    Riscv::mc_sstatus(Riscv::SSTATUS_SIE);
    // u slučaju da nakon userMain() neka korisnička nit nije završila, nasilno je gasimo
    Scheduler::flush_user_threads();

    printString("main() cleaning up\n");

    // čekanje da se ispiše sve iz bafera ako već nije
    while (Riscv::bufout->count() > 0) { thread_dispatch(); }

    // oslobađanje sve memorije
    kmem_buffers_destroy();
    kmem_cache_destroy(SleepNode::cachep);
    kmem_cache_destroy(_sem::cachep);
    kmem_cache_destroy(List<TCB>::cachep);
    kmem_cache_destroy(TCB::cachep);
    kmem_cache_destroy(TCB::stack_cachep);

    return 0;
}
