//
// Created by djafere on 5/9/22.
//


#include "../h/MemDescr.hpp"
#include "../lib/console.h"


void insert(MemDescr **head_ptr, MemDescr *node, MemChunkStatus st)
{
	// ako je node=NULL, odmah se vrati
	if (!node) return;
	node->status = st;
	
	// ako je head=NULL (odnosno ako nije bilo čvorova u listi do sad), head postaje node
	if (!(*head_ptr)) {
		*head_ptr = node;
		return;
	}
	
	// ako node treba da stoji ispred head-a, preveži ih i node postaje head
	MemDescr *head = *head_ptr;
	if (node < head) {
		node->next = head;
		head->prev = node;
		
		*head_ptr = node;
		return;
	}
	
	// ako node treba da stoji negde u sred liste:
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
	
	// ako node treba da stoji na kraju liste:
	prev->next = node;
	node->prev = prev;
	return;
}

void remove(MemDescr **head_ptr, MemDescr *node)
{
	if (!head_ptr || !node) // ako ne postoji lista u pitanju ili ako ne postoji node u pitanju
		return;
	
	MemDescr *head = *head_ptr;
	if (head == node) {     // ako je node koji se briše početni u listi, njegov sledbenik postaje početni
		*head_ptr = head->next;
		node->next = node->prev = nullptr;
		return;
	}
	
	MemDescr *prev = node->prev,
	         *next = node->next;
	
	// ako je na bilo kom drugom mestu u listi, samo preveži članove
	prev->next = next;
	next->prev = prev;
	
	// restovanje pointera
	node->next = node->prev = nullptr;
	return;
}

void squash(MemDescr *free)
{
	if (!free || free->status != FREE)
		return;
	
	MemDescr *next, *curr;
	curr = (free->prev) ?
	       free->prev :
	       free;
	
	// ako sledeći slobodni segment postoji, i ako se trenutni segment proteže sve do sledećeg, spoji ih
	while (curr->next && (((char*)curr + curr->size + MEM_BLOCK_SIZE) >= ((char*)(curr->next)))) {
		next = curr->next;
		
		curr->size += MEM_BLOCK_SIZE + next->size;
		curr->next = curr->next->next;
		if (curr->next)
			curr->next->prev = curr;
	}
	return;
}

void print_node(MemDescr *node)
{
	print_string("nnode:\t\0");
	print_int((unsigned long)node);
	print_string("\nnext:\t\0");
	print_int((unsigned long)node->next);
	print_string("\nprev:\t\0");
	print_int((unsigned long)node->prev);
	print_string("\nsize:\t\0");
	print_int(node->size);
	print_string("\nstat:\t\0");
	print_int(node->status);
	__putc('\n');
}
void print_int(size_t s) {
	const int SIZE = 64;
	char c[SIZE];
	int i = 0;
	while (s) {
		c[SIZE-i-1] = (s % 10) + '0';
		i++;
		
		s /= 10;
	}
	
	for (int j = 0; j < i; j++) {
		__putc(c[SIZE-i+j]);
	}
	
	__putc('\n');
}
void print_string(const char *c) {
	while (*(c++)) {
		__putc(*c);
	}
}
