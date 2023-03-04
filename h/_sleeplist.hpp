//
// Created by djafere on 6/23/22.
//

#ifndef __sleeplist_hpp
#define __sleeplist_hpp

#include "tcb.hpp"
#include "slab.hpp"

struct SleepNode {
    TCB *tcb;
    time_t timeout;
    SleepNode(TCB *_tcb, time_t _timeout) : tcb(_tcb), timeout(_timeout) { }

    static kmem_cache_t *cachep;
    
    void *operator new(size_t size)
    {
        if (cachep == nullptr)
        {
            cachep = kmem_cache_create("SLPNODE\0", sizeof(SleepNode), nullptr, nullptr);
        }
        return kmem_cache_alloc(cachep);
    }

    void operator delete(void *ptr)
    {
        kmem_cache_free(cachep, ptr);
    }
};

class _sleeplist : public List<SleepNode> {
public:
    static void insert(SleepNode *s);

    static void insert(TCB *tcb, time_t timeout);

    static void tick();
    
    static _sleeplist Sleeping;

private:
    static bool Comparator(SleepNode *t1, SleepNode *t2) { return t1->timeout < t2->timeout; }

    static TCB *pop();

    static SleepNode *pop_node();

    static time_t passed;
    
    static time_t total_passed;
    
    static bool ready();
};

#endif //__sleeplist_hpp
