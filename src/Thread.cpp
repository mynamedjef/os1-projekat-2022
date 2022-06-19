//
// Created by djafere on 6/19/22.
//

#include "../h/syscall_cpp.hpp"
#include "../h/_thread.hpp"

Thread::Thread(void (*body)(void*), void *arg)
{
    thread_init(&myHandle, body, arg);
}

Thread::~Thread()
{
    delete myHandle;
}

int Thread::start()
{
    return myHandle->start();
}

bool Thread::isFinished() const
{
    return myHandle->isFinished();
}

void Thread::dispatch()
{
    _thread::dispatch();
}

void Thread::runWrapper(void *thread)
{
    ((Thread*)thread)->run();
}

Thread::Thread()
{
    thread_init(&myHandle, &(Thread::runWrapper), this);
}
