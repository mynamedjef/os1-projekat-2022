//
// Created by djafere on 6/18/22.
//

#ifndef _thread_hpp
#define _thread_hpp

#include "../h/tcb.hpp"

class _thread {
public:
    using Body = void(*)(void*);
    
    using thread_t = _thread*;
    
    _thread(thread_t*, Body, void*);
    
    static int exit();
    
    static void dispatch();
    
private:
    TCB *tcb = nullptr;
};

#endif //_thread_hpp
