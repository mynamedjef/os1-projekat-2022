//
// Created by djafere on 6/16/22.
//

#include "../h/tcb.hpp"
#include "../h/print.hpp"
#include "../h/workers.hpp"

void userMain() {
    TCB *threads[4];

    threads[0] = TCB::createThread(workerBodyA);
    printString("ThreadA created\n");
    threads[1] = TCB::createThread(workerBodyB);
    printString("ThreadB created\n");
    threads[2] = TCB::createThread(workerBodyC);
    printString("ThreadC created\n");
    threads[3] = TCB::createThread(workerBodyD);
    printString("ThreadD created\n");

    while (!(threads[0]->isFinished() &&
             threads[1]->isFinished() &&
             threads[2]->isFinished() &&
             threads[3]->isFinished()))
    {
        TCB::yield();
    }

    for (auto &thread: threads)
    {
        delete thread;
    }
    printString("userMain() finished\n");
}