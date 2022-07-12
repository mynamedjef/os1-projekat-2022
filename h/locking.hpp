//
// Created by djafere on 7/12/22.
//

#ifndef _locking_hpp
#define _locking_hpp

class Locking {
public:
    static volatile int lock_cnt;

    static void lock();

    static void unlock();

    static volatile bool dispatch_call;
};

#endif //_locking_hpp
