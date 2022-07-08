//
// Created by djafere on 7/8/22.
//

#include "../h/_buffer.hpp"

_buffer::_buffer(int capacity) : head(0), tail(0), cap(capacity), size(0)
{
    new _sem(&mutex_put, 1);
    new _sem(&mutex_get, 1);
    new _sem(&free, cap);
    new _sem(&available, 0);
}

// get() koji se koristi samo tokom prekidne rutine (kernel kod)
char _buffer::kernel_get()
{
    available->wait();
    mutex_get->wait();

    char ret = buf[head];
    head = (head + 1) % cap;
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
    head = (head + 1) % cap;
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
    tail = (tail + 1) % cap;
    size++;

    mutex_put->signal();
    available->signal();
}

// put() koji se koristi u korisničkim funkcijama
void _buffer::put(char x)
{
    sem_wait(free);
    sem_wait(mutex_put);

    buf[tail] = x;
    tail = (tail + 1) % cap;
    size++;

    sem_signal(mutex_put);
    sem_signal(available);
}
