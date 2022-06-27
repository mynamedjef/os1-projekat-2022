
#ifndef _scheduler_hpp
#define _scheduler_hpp

#include "tcblist.hpp"

class TCB;

class Scheduler
{
private:
    static TCBList readyThreadQueue;

public:
    static TCB *get();

    static void put(TCB *ccb);

    static int size();

};

#endif //_scheduler_hpp
