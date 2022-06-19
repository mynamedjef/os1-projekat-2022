//
// Created by djafere on 6/18/22.
//

#ifndef _thread_hpp
#define _thread_hpp

#include "../h/tcb.hpp"

class _thread {
    
    friend class Riscv;

public:
    
    using Body = void(*)(void*);
    
    using thread_t = _thread*;
    
    _thread(thread_t*, Body, uint64*, void*);

    ~_thread();

    bool isFinished() const { return tcb->isFinished(); }
    
    static int exit();
    
    static void dispatch();
    
private:
    TCB *tcb = nullptr;
    
    int start();
    
};

#endif //_thread_hpp
