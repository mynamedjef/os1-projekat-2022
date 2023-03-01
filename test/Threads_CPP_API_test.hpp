#ifndef XV6_THREADS_CPP_API_TEST_HPP
#define XV6_THREADS_CPP_API_TEST_HPP

#include "../h/syscall_cpp.hpp"

#include "printing.hpp"

bool finishedA = false;
bool finishedB = false;
bool finishedC = false;
bool finishedD = false;

const int N = 1;
bool finished[4] = {0};

uint64 fibonacci(uint64 n) {
    if (n == 0 || n == 1) { return n; }
    if (n % 10 == 0) { thread_dispatch(); }
    return fibonacci(n - 1) + fibonacci(n - 2);
}

class WorkerA: public Thread {
    void workerBodyA(void* arg);
public:
    WorkerA():Thread() {}

    void run() override {
        workerBodyA(nullptr);
    }
};

class WorkerB: public Thread {
    void workerBodyB(void* arg);
public:
    WorkerB():Thread() {}

    void run() override {
        workerBodyB(nullptr);
    }
};

class WorkerC: public Thread {
    void workerBodyC(void* arg);
public:
    WorkerC():Thread() {}

    void run() override {
        workerBodyC(nullptr);
    }
};

class WorkerD: public Thread {
    void workerBodyD(void* arg);
public:
    WorkerD():Thread() {}

    void run() override {
        workerBodyD(nullptr);
    }
};

void WorkerA::workerBodyA(void *arg) {
    for (uint64 i = 0; i < 10; i++) {
        printString("A: i="); printInt(i); printString("\n");
        for (uint64 j = 0; j < 10000; j++) {
            for (uint64 k = 0; k < 4000; k++) { /* busy wait */ }
            thread_dispatch();
        }
    }
    printString("A finished!\n");
    finishedA = true;
    finished[0] = true;
}

void WorkerB::workerBodyB(void *arg) {
    for (uint64 i = 0; i < 16; i++) {
        printString("B: i="); printInt(i); printString("\n");
        for (uint64 j = 0; j < 10000; j++) {
            for (uint64 k = 0; k < 4000; k++) { /* busy wait */ }
            thread_dispatch();
        }
    }
    printString("B finished!\n");
    finishedB = true;
    finished[1] = true;
    thread_dispatch();
}

void WorkerC::workerBodyC(void *arg) {
    uint8 i = 0;
    for (; i < 3; i++) {
        printString("C: i="); printInt(i); printString("\n");
    }

    printString("C: dispatch\n");
    __asm__ ("li t1, 7");
    thread_dispatch();

    uint64 t1 = 0;
    __asm__ ("mv %[t1], t1" : [t1] "=r"(t1));

    printString("C: t1="); printInt(t1); printString("\n");

    uint64 result = fibonacci(12);
    printString("C: fibonaci="); printInt(result); printString("\n");

    for (; i < 6; i++) {
        printString("C: i="); printInt(i); printString("\n");
    }

    printString("A finished!\n");
    thread_dispatch();
    finished[2] = true;
}

void WorkerD::workerBodyD(void* arg) {
    uint8 i = 10;
    for (; i < 13; i++) {
        printString("D: i="); printInt(i); printString("\n");
    }

    printString("D: dispatch\n");
    __asm__ ("li t1, 5");
    thread_dispatch();

    uint64 result = fibonacci(16);
    printString("D: fibonaci="); printInt(result); printString("\n");

    for (; i < 16; i++) {
        printString("D: i="); printInt(i); printString("\n");
    }

    printString("D finished!\n");
    thread_dispatch();
    finished[3] = true;
}


void Threads_CPP_API_test() {
    Thread* threads[4];

    int j = 0;
    threads[j++] = new WorkerA();
    threads[j++] = new WorkerB();
//    threads[j++] = new WorkerC();
//    threads[j++] = new WorkerD();
    
    threads[0]->start();
    threads[1]->start();
    while (!finished[1] || !finished[0]) {
        thread_dispatch();
    }
    delete threads[0];
    delete threads[1];
    return;
    for (int i = 0; i < N; i++) { threads[i]->start(); }
    for (int i = 0; i < N; i++)
    {
        if (!finished[i]) {
            i = 0;
            thread_dispatch();
        }
    }
    for (int i = 0; i < N; i++) { delete threads[i]; }
    return;

    for (int i = 0; i < 4; i++) {
        printHexa((uint64)threads[i]);
        putc('\n');
    }

    for(int i=0; i<4; i++) {
        threads[i]->start();
    }

    while (!(finishedA && finishedB && finishedC && finishedD)) {
        Thread::dispatch();
    }

    for (auto thread: threads)
    {
        delete thread;
    }
}

#endif //XV6_THREADS_CPP_API_TEST_HPP
