//
// Created by djafere on 5/9/22.
//

#ifndef _memory_allocator_hpp
#define _memory_allocator_hpp

#include "../lib/hw.h"
#include "MemDescr.hpp"


class MemoryAllocator {
public:
    static MemDescr *free, *occupied;

    static uint64 allocd;

    static uint64 deallocd;

    static void *alloc(size_t);

    static int mem_free(void*);

    static void init_memory();

};


#endif //_memory_allocator_hpp
