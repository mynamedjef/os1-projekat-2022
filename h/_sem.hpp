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

    static void cleanup();

private:
    List<TCB> waiting;

    static List<_sem> all_sems;

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
