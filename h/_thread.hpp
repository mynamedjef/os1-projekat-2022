//
// Created by djafere on 6/22/22.
//

#ifndef __thread_hpp
#define __thread_hpp

#include "syscall_c.h"
#include "tcb.hpp"

class _thread {
public:
    using Body = void(*)(void*);

    ~_thread() { delete parent; };

    bool isFinished() const { return parent->isFinished(); }

    void *operator new(size_t size) { return __mem_alloc(size); }

    void operator delete(void *ptr) { __mem_free(ptr); }

    static void cleanup()
    {
        while (all_threads.size() > 0) {
            _thread *temp = all_threads.removeFirst();
            delete temp;
        }
    }

private:
    static List<_thread> all_threads;

    TCB *parent;

    int start() { return parent->start(); }

    _thread(thread_t *handle, Body start_routine, void *arg, uint64 *stack_space)
    {
        parent = TCB::initThread(start_routine, arg, stack_space);
        *handle = this;
        all_threads.addLast(this);
    }

    friend class Riscv;

    friend class Thread;

};

#endif //__thread_hpp
