//
// Created by djafere on 6/23/22.
//

#ifndef __sem_hpp
#define __sem_hpp

#include "syscall_c.h"
#include "list.hpp"
#include "tcb.hpp"

class _sem {
public:
    ~_sem();

    void *operator new(size_t size) { return __mem_alloc(size); }

    void operator delete(void *ptr) { __mem_free(ptr); }

    _sem(sem_t *handle, unsigned init);

    // oslobađa sve niti sa semafora i onemogućava dalje korišćenje semafora
    int close();

    int wait();

    int signal();

private:
    List<TCB> waiting;

    int val;

    bool closed;
};

#endif //__sem_hpp
