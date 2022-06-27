//
// Created by djafere on 6/27/22.
//

#include "../lib/mem.h"
#include "../h/MemoryAllocator.hpp"

void *__mem_alloc(size_t size)
{
    return MemoryAllocator::alloc(size);
}

int __mem_free(void *ptr)
{
    return MemoryAllocator::mem_free(ptr);
}
