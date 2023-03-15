//
// Created by djafere on 6/27/22.
//

#include "../lib/mem.h"
#include "../h/MemoryAllocator.hpp"
#include "../h/syscall_c.h"

using size_t = decltype(sizeof(0));

/*
 * Ukupno imamo dva načina alokacije - iz kernela i preko sistemskog poziva.
 * Operacije memorijskog alokatora nisu thread-safe, ali s obzirom da ovaj kernel nema preotimanje,
 * nemamo problem, tj. sve što se poziva iz kernel moda (prekidne rutine prouzrokovane ecall-om) je thread-safe.
 *
 * Zbog toga su __mem_alloc/__mem_free namenjene samo za korišćenje u kernelu.
 *
 * mem_alloc/mem_free su sistemski pozivi, tj. uz pomoć njih prelazimo u kernel iz koga
 * se pozivaju __mem_alloc/__mem_free.
 *
 * Globalni new/delete se prevezuju na sistemski poziv mem_alloc/mem_free.
 */

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
