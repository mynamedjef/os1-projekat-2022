//
// Created by djafere on 7/8/22.
//

#include "../h/_buffer.hpp"

kmem_cache_t *_buffer::cachep = nullptr;

void *_buffer::operator new(size_t size)
{
    if (cachep == nullptr)
    {
        cachep = kmem_cache_create("IO-BFFR\0", sizeof(_buffer), nullptr, nullptr);
    }
    return kmem_cache_alloc(cachep);
}

void _buffer::operator delete(void *obj)
{
    kmem_cache_free(cachep, obj);
}

_buffer::_buffer() : head(0), tail(0), size(0)
{
    new _sem(&mutex_put, 1);
    new _sem(&mutex_get, 1);
    new _sem(&free, BUFFER_SIZE);
    new _sem(&available, 0);
}

_buffer::~_buffer()
{
    delete mutex_put;
    delete mutex_get;
    delete free;
    delete available;
}

// get() koji se koristi samo tokom prekidne rutine (kernel kod)
char _buffer::kernel_get()
{
    available->wait();
    mutex_get->wait();

    char ret = buf[head];
    head = (head + 1) % BUFFER_SIZE;
    size--;

    mutex_get->signal();
    free->signal();

    return ret;
}

// get() koji se koristi u korisničkim funkcijama
char _buffer::get()
{
    sem_wait(available);
    sem_wait(mutex_get);

    char ret = buf[head];
    head = (head + 1) % BUFFER_SIZE;
    size--;

    sem_signal(mutex_get);
    sem_signal(free);

    return ret;
}

void _buffer::kernel_put(char x)
{
    free->wait();
    mutex_put->wait();

    buf[tail] = x;
    tail = (tail + 1) % BUFFER_SIZE;
    size++;

    mutex_put->signal();
    available->signal();
}
