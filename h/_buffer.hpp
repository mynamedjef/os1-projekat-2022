//
// Created by djafere on 7/7/22.
//

#ifndef _buffer_hpp
#define _buffer_hpp

#include "../h/_sem.hpp"
#define BUFFER_SIZE 256

/*
 * Kernel koristi dva ovakva bafera za ispis (putc()) i čitanje sa tastature (getc()).
 * producer za (izlazni) putc() bafer - sistemski poziv putc(char)
 * consumer za (izlazni) putc() bafer - sistemska nit koja periodično ispisuje šta god se pojavi u ovom baferu
 * producer za (ulazni) getc() bafer - hardverski prekid tj. kliktanje na tastaturu
 * consumer za (ulazni) getc() bafer - sistemski poziv getc()
 */
class _buffer {
private:
    sem_t mutex_get, mutex_put, free, available;

    char buf[BUFFER_SIZE];

    int head, tail, size;

public:
    void *operator new(size_t size) { return __mem_alloc(size); }

    void operator delete(void *ptr) { __mem_free(ptr); }

    _buffer();

    ~_buffer();

/*
 * get() koji se koristi samo tokom prekidne rutine.
 * Ovu funkciju će koristiti getc() da pokupi karakter iz ulaznog bafera.
 */
    char kernel_get();

/*
 * get() koji se koristi samo izvan prekidne rutine (tj. kernel koda).
 * Ovu funkciju će koristiti output nit da pokupi karakter iz izlaznog bafera i ispiše ga.
 */
    char get();

/*
 * put() koji se koristi samo tokom prekidne rutine.
 * Ovu funkciju će koristiti:
 *  putc(char) za upis u izlazni bafer;
 *  hardverski prekid (prilikom kliktanja tastature) za upis u ulazni bafer.
 */
    void kernel_put(char x);

    int count() const { return size; }

    friend class Riscv;
};

#endif //_buffer_hpp
