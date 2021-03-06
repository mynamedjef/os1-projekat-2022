//
// Created by djafere on 5/9/22.
//

#ifndef _mem_descr_h
#define _mem_descr_h

#include "../lib/hw.h"

enum MemChunkStatus {
	FREE,
	ALLOCATED
};

typedef struct MemDescr {
	size_t size;
	struct MemDescr *next, *prev;
	MemChunkStatus status;

} MemDescr;

void insert(MemDescr **head_ptr, MemDescr *node, MemChunkStatus st);

void remove(MemDescr **head_ptr, MemDescr *node);

void squash(MemDescr *free);

#endif //_mem_descr_h
