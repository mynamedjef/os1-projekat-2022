//
// Created by djafere on 7/7/22.
//

#ifndef _buffer_hpp
#define _buffer_hpp

#include "_sem.hpp"
#include "slab.hpp"

/*
* Veličina char buffer-a u bajtovima.
* Ideja je da sizeof(_buffer) stane tačno u jedan mali memorijski bafer
* tj. da idealno sizeof(_buffer) bude jednak stepenu dvojke 2^n.
* Sobzirom da su strukture poravnate na 8 bajtova, a kmalloc ima dodatno
* zaglavlje od jednog bajta, veličina sizeof(_buffer) ne sme biti veća od (2^n - 8).
* Onda od te veličine oduzimamo veličine ostalih polja
* da dobijemo veličinu char buffer-a: (2^n - 8 - 4*sizeof(sem_t) - 3*sizeof(int)).
*
* Da bi važilo BUFFER_SIZE => 0, mora da važi 2^n >= 64. U trenutnoj impl. uzeto (2^n = 256)
*/
#define BUFFER_SIZE ((256 - 8) - 4*sizeof(sem_t) - 3*sizeof(int))

class _buffer {
private:
    sem_t mutex_get, mutex_put, free, available;

    char buf[BUFFER_SIZE];

    int head, tail, size;

public:
    void *operator new(size_t size);

    void operator delete(void *ptr);

    _buffer();

    ~_buffer();

    // get() koji se koristi samo tokom prekidne rutine (kernel kod)
    char kernel_get();

    // get() koji se koristi u korisničkim funkcijama
    char get();

    // put() koji se koristi samo tokom prekidne rutine (kernel kod)
    void kernel_put(char x);

    int count() const { return size; }

    friend class Riscv;
};

#endif //_buffer_hpp
