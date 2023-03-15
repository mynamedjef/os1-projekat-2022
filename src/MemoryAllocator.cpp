//
// Created by djafere on 5/9/22.
//

#include "../h/MemoryAllocator.hpp"

MemDescr *MemoryAllocator::free = nullptr;

MemDescr *MemoryAllocator::occupied = nullptr;

uint8 *MemoryAllocator::base_ptr = nullptr;

uint8 *MemoryAllocator::end_ptr = nullptr;

alloc_info MemoryAllocator::allocd = {0, 0};

alloc_info MemoryAllocator::deallocd = {0, 0};

void MemoryAllocator::init_memory() {
    const uint64 offset = 0;  // isključivo za debagovanje
    base_ptr = (uint8*)HEAP_START_ADDR + offset;
    end_ptr = (uint8*)HEAP_END_ADDR;

    // poravnanje na MEM_BLOCK_SIZE
    uint64 remainder = (uint64)base_ptr % MEM_BLOCK_SIZE;
    if (remainder != 0)
    {
        base_ptr += MEM_BLOCK_SIZE - remainder;
    }
    end_ptr = (uint8*)((uint64)end_ptr & ~(MEM_BLOCK_SIZE-1));

    // inicijalizacija početnog slobodnog segmenta
    free = (MemDescr*)base_ptr;
    MemDescr *mem = free;

    mem->prev = mem->next = nullptr;
    mem->size = (size_t)(end_ptr - base_ptr);  // zagarantovano poravnato jer su end_ptr i base_ptr poravnati već
    mem->status = FREE;
}

void *MemoryAllocator::alloc(size_t sz) {
    // padding for block size
    sz = (sz % MEM_BLOCK_SIZE == 0) ?
         sz :
         (sz / MEM_BLOCK_SIZE + 1) * MEM_BLOCK_SIZE;

    allocd.bytes += sz;
    allocd.times++;

    for (MemDescr *curr = free; curr; curr = curr->next) {
        if (curr->size == sz) { // ako je tačna veličina segmenta, samo prebaci iz free u occupied listu
            remove(&free, curr);
            insert(&occupied, curr, ALLOCATED);
            return (void*)((char*)(curr) + MEM_BLOCK_SIZE);
        }
        else if (curr->size > sz) { // ako nije tačna, napravi novi segment posle sz prostora, i ubaci ga u free listu
            remove(&free, curr);

            MemDescr *n = (MemDescr*)((char*)curr + MEM_BLOCK_SIZE + sz);
            n->size = (curr->size - sz - MEM_BLOCK_SIZE);
            n->next = n->prev = nullptr;

            curr->size = sz;
            insert(&free, n, FREE);
            insert(&occupied, curr, ALLOCATED);
            return (void*)((char*)(curr) + MEM_BLOCK_SIZE);
        }
    }

    // nije našao adekvatan segment, vrati nullptr
    allocd.bytes -= sz;
    allocd.times--;
    return nullptr;
}

int MemoryAllocator::mem_free(void *ptr)
{
    if (!ptr)
        return -1;
    MemDescr *mem = (MemDescr*)((char*)(ptr) - MEM_BLOCK_SIZE);
    if (mem->status != ALLOCATED) // nije alociran segment
        return -1;

    deallocd.bytes += mem->size;
    deallocd.times++;

    // skloni segment iz okupiranih, ubaci u slobodne i squash-uj
    remove(&occupied, mem);
    insert(&free, mem, FREE);
    squash(mem);

    return 0;
}
