//
// Created by djafere on 7/7/22.
//

#ifndef _buffer_hpp
#define _buffer_hpp

#include "../h/_sem.hpp"
#define BUFFER_SIZE 256

class _buffer {
private:
    sem_t mutex_get, mutex_put, free, available;

    char buf[BUFFER_SIZE];

    int head, tail, cap, size;

    void add(char c);

    char remove();

public:
    void *operator new(size_t size) { return __mem_alloc(size); }

    void operator delete(void *ptr) { __mem_free(ptr); }

    _buffer(int capacity);

    _buffer() : _buffer(BUFFER_SIZE) { }

    ~_buffer();

    // get() koji se koristi samo tokom prekidne rutine (kernel kod)
    char kernel_get();

    // get() koji se koristi u korisničkim funkcijama
    char get();

    // put() koji se koristi samo tokom prekidne rutine (kernel kod)
    void kernel_put(char x);

    // put() koji se koristi u korisničkim funkcijama
    void put(char x);

    int count() const { return size; }

    friend class Riscv;
};

#endif //_buffer_hpp
