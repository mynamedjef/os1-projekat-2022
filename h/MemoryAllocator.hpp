//
// Created by djafere on 5/9/22.
//

#ifndef _memory_allocator_hpp
#define _memory_allocator_hpp

#include "../lib/hw.h"
#include "MemDescr.hpp"

struct alloc_info
{
    uint64 bytes;
    uint64 times;
};

class MemoryAllocator {
public:
    static MemDescr *free, *occupied;

    static alloc_info allocd;

    static alloc_info deallocd;

    static void *alloc(size_t);

    static int mem_free(void*);

    static void init_memory();

private:
    static uint64 offset;

    static uint8 *base_ptr;

    static uint8 *end_ptr;
};


#endif //_memory_allocator_hpp
