//
// Created by djafere on 5/9/22.
//

#include "../h/MemoryAllocator.hpp"
#include "../lib/console.h"

MemDescr *MemoryAllocator::free = nullptr;
MemDescr *MemoryAllocator::occupied = nullptr;

void MemoryAllocator::init_memory() {
	free = (MemDescr*)HEAP_START_ADDR;
	MemDescr *mem = free;
	
	mem->prev = mem->next = nullptr;
	mem->size = (size_t)((char*)HEAP_END_ADDR - (char*)HEAP_START_ADDR - MEM_BLOCK_SIZE);
}

void *MemoryAllocator::alloc(size_t sz) {
	// padding for block size
	sz += (sz % MEM_BLOCK_SIZE);
	
	
	for (MemDescr *curr = free; curr; curr = curr->next) {
		if (curr->size == sz) {
			remove(&free, curr);
			insert(&occupied, curr);
			return (void*)((char*)(curr) + MEM_BLOCK_SIZE);
		}
		else if (curr->size > sz) {
			remove(&free, curr);
			
			MemDescr *n = (MemDescr*)(MEM_BLOCK_SIZE + curr + sz);
			n->size = (curr->size - sz - MEM_BLOCK_SIZE);
			n->next = n->prev = nullptr;
			
			curr->size = sz;
			insert(&free, n);
			insert(&occupied, curr);
			return (void*)((char*)(curr) + MEM_BLOCK_SIZE);
		}
	}
	
	return nullptr;
}



