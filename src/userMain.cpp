//
// Created by djafere on 6/16/22.
//

#include "../h/tcb.hpp"
#include "../h/print.hpp"
#include "../h/workers.hpp"
#include "../h/syscall_c.h"
#include "../h/_thread.hpp"

void userMain1() {
    thread_t threads[4];

    thread_create(&threads[0], workerBodyA, nullptr);
    printString("ThreadA created\n");
    thread_create(&threads[1], workerBodyB, nullptr);
    printString("ThreadB created\n");
    thread_create(&threads[2], workerBodyC, nullptr);
    printString("ThreadC created\n");
    thread_create(&threads[3], workerBodyD, nullptr);
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

void userMain(void *) {
    userMain1();
    return;
}