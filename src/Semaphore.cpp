//
// Created by djafere on 6/20/22.
//

#include "../h/syscall_cpp.hpp"
#include "../h/_sem.hpp"

Semaphore::Semaphore(unsigned init)
{
    new _sem(&myHandle, init);
}

Semaphore::~Semaphore()
{
    myHandle->close();
}

int Semaphore::wait()
{
    return myHandle->wait();
}

int Semaphore::signal()
{
    return myHandle->signal();
}
