//
// Created by os on 2/1/23.
//

#ifndef OS1_2022_SLAB_HPP
#define OS1_2022_SLAB_HPP

#include "../lib/hw.h"
#include "../h/list_t.hpp"

// veličina blokova (zadato u zadatku)
#define BLOCK_SIZE_LOG2 (12)
#define BLOCK_SIZE ((size_t)1 << BLOCK_SIZE_LOG2)

// koliko najmanje objekata će biti u svakoj ploči
#define OBJECTS_PER_SLAB_LOG2 (6)
#define OBJECTS_PER_SLAB ((size_t)1 << OBJECTS_PER_SLAB_LOG2)

// minimalna veličina malih memorijskih bafera (zadato u zadatku)
#define MIN_BUFFER_LOG2 5
#define MIN_BUFFER ((size_t)1 << MIN_BUFFER_LOG2)

// maksimalna veličina malih memorijskih bafera (zadato u zadatku)
#define MAX_BUFFER_LOG2 17
#define MAX_BUFFER ((size_t)1 << MAX_BUFFER_LOG2)

// koliko različitih keševa za male memorijske bafere imamo
#define BUFFER_COUNT (MAX_BUFFER_LOG2 - MIN_BUFFER_LOG2 + 1)

// u zaglavlje malih memorijskih bafera upisujemo indeks bafera kome pripada alociran prostor
#define KMALLOC_HEADER_SIZE 1

#define KMEM_CACHE_NAME_SIZE (8)

typedef struct kmem_slab_s
{
    // *next i *prev moraju da budu u ovom rasporedu da bi bili isti kao kod list_t
    kmem_slab_s *next;
    kmem_slab_s *prev;
    list_t *free;
    uint count;
} kmem_slab_t;

typedef struct kmem_cache_s
{
    char name[KMEM_CACHE_NAME_SIZE];
    kmem_slab_t *empty;
    kmem_slab_t *full;
    kmem_slab_t *mixed;
    size_t obj_size;
    size_t slab_size; // derived od obj_size i OBJECTS_PER_SLAB. Uvek umnožak BLOCK_SIZE
    void (*ctor)(void*);
    void (*dtor)(void*);
    // uint max_obj_cnt; // derived od slab_size i obj_size (ne koristimo ga da bi sizeof(kmem_cache_s)=2^6)
} kmem_cache_t;

void kmem_init(void *space, int block_num);

void kmem_init(void *heap_start, void *heap_end);

kmem_cache_t *kmem_cache_create(const char *name, size_t size,
                                    void (*ctor)(void*),
                                    void (*dtor)(void*)); // Allocate cache

int kmem_cache_shrink(kmem_cache_t *cachep); // Shrink cache

void *kmem_cache_alloc(kmem_cache_t *cachep); // Allocate one object from cache

void kmem_cache_free(kmem_cache_t *cachep, void *objp); // Deallocate one object from cache

void *kmalloc(size_t size); // Allocate one small memory buffer

void kfree(const void *objp); // Deallocate one small memory buffer

void kmem_cache_destroy(kmem_cache_t *cachep); // Deallocate cache

void kmem_cache_info(kmem_cache_t *cachep); // Print cache info
void kmem_cache_error(kmem_cache_t *cachep); // Print error message

void kmem_buf_info(size_t size);

void kmem_all_caches_info();

#endif //OS1_2022_SLAB_HPP
