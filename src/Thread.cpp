//
// Created by djafere on 6/24/22.
//

#include "../h/syscall_cpp.hpp"
#include "../h/_thread.hpp"

Thread::Thread(void (*body)(void*), void *arg)
{
    thread_prepare(&myHandle, body, arg);
}

Thread::Thread()
{
    thread_prepare(&myHandle, Thread::wrapper, this);
}

Thread::~Thread()
{
    delete myHandle;
}

int Thread::start()
{
    return thread_start(myHandle);
}

void Thread::dispatch()
{
    thread_dispatch();
}

int Thread::sleep(time_t time)
{
    return time_sleep(time);
}

void Thread::wrapper(void *thr)
{
    ((Thread*)thr)->run();
}
