
#ifndef __stack_hpp
#define __stack_hpp

#include "../lib/hw.h"
#include "../h/slab.hpp"

/*
* Stek koji se koristi za TCB
*/
class _stack
{
public:
    uint64 stack[DEFAULT_STACK_SIZE];

    static kmem_cache_t *cachep;

    void *operator new(size_t size)
    {
        if (cachep == nullptr)
        {
            cachep = kmem_cache_create("TCB-STK\0", sizeof(_stack), nullptr, nullptr);
        }
        return kmem_cache_alloc(cachep);
    }

    void operator delete(void *loc)
    {
        kmem_cache_free(cachep, loc);
    }
};

#endif // __stack_hpp
