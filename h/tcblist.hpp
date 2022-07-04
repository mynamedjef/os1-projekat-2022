//
// Created by djafere 6/27/22.
//

#ifndef _tcblist_hpp
#define _tcblist_hpp

#include "tcb.hpp"

class TCBList {
public:
    struct Elem
    {
        TCB *data;
        Elem *next;

        Elem(TCB *data, Elem *next) : data(data), next(next) {}

        void *operator new(size_t size) { return __mem_alloc(size); }
        void operator delete(void *ptr) { __mem_free(ptr); }
    };

    Elem *head, *tail;
    unsigned count;

    TCBList() : head(nullptr), tail(nullptr), count(0) {}

    unsigned size() const { return count; }

    void insert(TCB *data);

    TCB *pop();

    TCB *pop_priority(bool(*)(TCB*, TCB*));
};

#endif //_tcblist_hpp
