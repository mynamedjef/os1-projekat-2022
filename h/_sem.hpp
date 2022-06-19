//
// Created by djafere on 6/19/22.
//

#ifndef _sem_hpp
#define _sem_hpp

#include "list.hpp"
#include "tcb.hpp"

class _sem {
public:
    _sem(_sem **handle, int value);
    
    _sem(_sem **handle);
    
    ~_sem();
    
    int wait();
    
    int signal();
    
    int val() const;
    
    int close();
    
private:
    int volatile value;
    
    bool volatile closed;
    
    void unblock();
    
    List<TCB> waiting;
};

#endif //_sem_hpp
