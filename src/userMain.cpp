//
// Created by djafere on 6/16/22.
//

#include "../h/tcb.hpp"
#include "../h/print.hpp"
#include "../h/syscall_cpp.hpp"
#include "../h/workers.hpp"
#include "../h/_thread.hpp"

void userMain1() {
    Thread *threads[5];

    threads[0] = new Thread(workerBodyA, nullptr);
    printString("ThreadA created\n");
    threads[1] = new Thread(workerBodyB, nullptr);
    printString("ThreadB created\n");
    threads[2] = new Thread(workerBodyC, nullptr);
    printString("ThreadC created\n");
    threads[3] = new Thread(workerBodyD, nullptr);
    printString("ThreadD created\n");
    threads[4] = new ThreadWorkerE();
    printString("ThreadE created\n");
    
    for (auto &thread : threads) {
        thread->start();
    }

    while (true) {
        bool finished = true;
        for (auto &thread : threads) {
            if (!thread->isFinished()) {
                finished = false;
                break;
            }
        }
        if (finished) { break; }
        
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