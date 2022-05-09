//
// Created by djafere on 5/9/22.
//

#include "../h/MemoryAllocator.hpp"
#include "../lib/console.h"

MemDescr *MemoryAllocator::free = nullptr;
MemDescr *MemoryAllocator::occupied = nullptr;

void MemoryAllocator::init_memory() {
	// inicijalizacija početnog slobodnog segmenta
	free = (MemDescr*)HEAP_START_ADDR;
	MemDescr *mem = free;
	
	mem->prev = mem->next = nullptr;
	mem->size = (size_t)((char*)HEAP_END_ADDR - (char*)HEAP_START_ADDR - MEM_BLOCK_SIZE);
}

void *MemoryAllocator::alloc(size_t sz) {
	// padding for block size
	sz += (sz % MEM_BLOCK_SIZE);
	
	
	for (MemDescr *curr = free; curr; curr = curr->next) {
		if (curr->size == sz) { // ako je tačna veličina segmenta, samo prebaci iz free u occupied listu
			remove(&free, curr);
			insert(&occupied, curr, ALLOCATED);
			return (void*)((char*)(curr) + MEM_BLOCK_SIZE);
		}
		else if (curr->size > sz) { // ako nije tačna, napravi novi segment posle sz prostora, i ubaci ga u free listu
			remove(&free, curr);
			
			MemDescr *n = (MemDescr*)(MEM_BLOCK_SIZE + curr + sz);
			n->size = (curr->size - sz - MEM_BLOCK_SIZE);
			n->next = n->prev = nullptr;
			
			curr->size = sz;
			insert(&free, n, FREE);
			insert(&occupied, curr, ALLOCATED);
			return (void*)((char*)(curr) + MEM_BLOCK_SIZE);
		}
	}
	
	// nije našao adekvatan segment, vrati nullptr
	return nullptr;
}

int MemoryAllocator::mem_free(void *ptr)
{
	if (!ptr)
		return -1;
	MemDescr *mem = (MemDescr*)((char*)(ptr) - MEM_BLOCK_SIZE);
	if (mem->status != ALLOCATED) // nije alociran segment
		return -1;
	
	// skloni segment iz okupiranih, ubaci u slobodne i squash-uj
	remove(&occupied, mem);
	insert(&free, mem, FREE);
	squash(mem);
	
	return 0;
}


// for debugging purposes
void MemoryAllocator::print_descr(MemDescr *mem)
{
	for (MemDescr *curr = mem; curr; curr = curr->next) {
		print_node(curr);
		__putc('\n');
	}
}
void MemoryAllocator::prfree() { print_descr(free); }
void MemoryAllocator::proccupied() { print_descr(occupied); }
