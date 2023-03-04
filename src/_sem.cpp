//
// Created by djafere on 6/23/22.
//

#include "../h/_sem.hpp"

kmem_cache_t *_sem::cachep = nullptr;

void *_sem::operator new(size_t size)
{
    if (cachep == nullptr)
    {
        cachep = kmem_cache_create("SEMPHR\0", sizeof(_sem), nullptr, nullptr);
    }
    return kmem_cache_alloc(cachep);
}

void _sem::operator delete(void *obj)
{
    kmem_cache_free(cachep, obj);
}

_sem::~_sem()
{
    close();
}

_sem::_sem(sem_t *handle, unsigned init)
{
    *handle = this;
    val = init;
    closed = false;
}

int _sem::close()
{
    if (closed) {
        return -1;
    }

    closed = true;
    val = 0;
    while (waiting.size() > 0) {
        TCB *tcb = waiting.removeFirst();
        tcb->release();
    }
    return 0;
}

int _sem::wait()
{
    if (closed) {
        return -1;
    }

    if (--val < 0) {
        waiting.addLast(TCB::running);
        TCB::wait();
    }
    return (closed) ? -1 : 0;
}

int _sem::signal()
{
    if (closed) {
        return -1;
    }

    if (++val <= 0) {
        TCB *tcb = waiting.removeFirst();
        tcb->release();
    }
    return 0;
}
