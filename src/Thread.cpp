//
// Created by djafere on 6/24/22.
//

#include "../h/syscall_cpp.hpp"

Thread::Thread(void (*body)(void*), void *arg)
{
    myHandle = nullptr;
    thread_prepare(&myHandle, body, arg);
}

Thread::Thread() : Thread(Thread::wrapper, this) { }

Thread::~Thread()
{
    thread_delete(myHandle);
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
