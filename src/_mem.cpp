//
// Created by djafere on 6/27/22.
//

#include "../lib/mem.h"
#include "../h/MemoryAllocator.hpp"
#include "../h/syscall_c.h"

using size_t = decltype(sizeof(0));

void *__mem_alloc(size_t size)
{
    return MemoryAllocator::alloc(size);
}

int __mem_free(void *ptr)
{
    return MemoryAllocator::mem_free(ptr);
}

void *operator new(size_t n)
{
    return mem_alloc(n);
}

void *operator new[](size_t n)
{
    return mem_alloc(n);
}

void operator delete(void *p) noexcept
{
    mem_free(p);
}

void operator delete[](void *p) noexcept
{
    mem_free(p);
}
