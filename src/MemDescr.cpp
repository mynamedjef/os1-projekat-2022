//
// Created by djafere on 5/9/22.
//


#include "../h/MemDescr.hpp"


void insert(MemDescr **head_ptr, MemDescr *node)
{
	if (!node) return;
	
	if (!(*head_ptr)) {
		*head_ptr = node;
		return;
	}
	
	MemDescr *head = *head_ptr;
	if (node < head) {
		node->next = head;
		head->prev = node;
		
		*head_ptr = node;
	}
	
	MemDescr *prev = head;
	for (MemDescr *curr = head->next; curr; curr = curr->next) {
		if (node < curr) {
			prev->next = node;
			node->prev = prev;
			
			curr->prev = node;
			node->next = curr;
			return;
		}
		prev = curr;
	}
	
	prev->next = node;
	node->prev = prev;
	return;
}

void remove(MemDescr **head_ptr, MemDescr *node)
{
	if (!head_ptr)
		return;
	
	MemDescr *head = *head_ptr;
	if (head == node) {
		*head_ptr = head->next;
		return;
	}
	
	MemDescr *prev = node->prev,
			*next = node->next;
	
	prev->next = next;
	return;
}

void squash(MemDescr *free)
{
	if (!free)
		return;
	
	MemDescr *next, *curr;
	curr = (free->prev) ?
	       free->prev :
	       free;
	next = curr->next;
	
	while (next && (((char*)curr + curr->size + MEM_BLOCK_SIZE) >= ((char*)next))) {
		curr->size += MEM_BLOCK_SIZE + next->size;
		curr->next = next->next;
		if (curr->next)
			curr->next->prev = curr;
		
		next = curr->next;
	}
	return;
}
