//
// Created by djafere on 6/23/22.
//

#ifndef __sem_hpp
#define __sem_hpp

#include "syscall_c.h"
#include "list.hpp"
#include "tcb.hpp"
#include "slab.hpp"

class _sem {
public:
    static kmem_cache_t *cachep;

    ~_sem();

    void *operator new(size_t size);

    void operator delete(void *ptr);

private:
    List<TCB> waiting;

    int val;

    bool closed;

    _sem(sem_t *handle, unsigned init);

    int close();

    int wait();

    int signal();

    friend class Riscv;

    friend class _buffer;
};

#endif //__sem_hpp
