
#ifndef _scheduler_hpp
#define _scheduler_hpp

#include "list.hpp"

class TCB;

class Scheduler
{
private:
    static List<TCB> readyThreadQueue;

public:
    static TCB *get();

    static void put(TCB *ccb);

    static int size();

};

#endif //_scheduler_hpp
