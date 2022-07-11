//
// Created by djafere on 5/8/22.
//

#ifndef _syscall_cpp
#define _syscall_cpp

#include "syscall_c.h"

void *operator new(size_t);
void operator delete(void*);

class Thread {
public:
    Thread (void (*body)(void*), void *arg);
    virtual ~Thread();

    int start();

    static void dispatch();
    static int sleep(time_t);
    static void wrapper(void*);
    static int getThreadId();

protected:
    Thread();
    virtual void run() {}

private:
    thread_t myHandle;
};

class Semaphore {
public:
    Semaphore(unsigned init=1);
    virtual ~Semaphore();

    int wait();
    int signal();

private:
    sem_t myHandle;
};

class PeriodicThread : public Thread {
protected:
    PeriodicThread(time_t period);
    virtual void periodicActivation() {}

public:
    static void wrapper(void*);
};

class Console {
public:
    static char getc();
    static void putc(char);
};

#endif //_syscall_cpp
