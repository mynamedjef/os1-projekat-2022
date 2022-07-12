//
// Created by djafere on 6/27/22.
//

#include "../lib/mem.h"
#include "../h/MemoryAllocator.hpp"
#include "../h/locking.hpp"

void *__mem_alloc(size_t size)
{
    Locking::lock();
    void *ptr = MemoryAllocator::alloc(size);
    Locking::unlock();
    return ptr;
}

int __mem_free(void *ptr)
{
    Locking::lock();
    int ret = MemoryAllocator::mem_free(ptr);
    Locking::unlock();
    return ret;
}
