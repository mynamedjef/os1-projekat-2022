//
// Created by os on 2/14/23.
//

#ifndef _buddy_hpp
#define _buddy_hpp

#include "../h/list_t.hpp"
#include "../lib/hw.h"

#define MIN_ALLOC_LOG2 5  // treba da bude najmanje sizeof(list_t) = 2*8 = 2^4
#define MIN_ALLOC ((size_t)1 << MIN_ALLOC_LOG2)

#define MAX_ALLOC_LOG2 26  // treba da bude 2^26
#define MAX_ALLOC ((size_t)1 << MAX_ALLOC_LOG2)

#define BUCKET_COUNT (MAX_ALLOC_LOG2 - MIN_ALLOC_LOG2 + 1)

#define PARENT(idx) ((idx) - 1) / 2
#define LCHILD(idx) ((idx) * 2) + 1
#define RCHILD(idx) ((idx) * 2) + 2
#define SIBLING(idx) (((idx) - 1) ^ 1) + 1

struct alloc_info
{
    size_t bytes;
    uint times;
};


class buddy
{
public:
    static void init(const void *heap_start, const void *heap_end);

    static void *buddy_alloc(size_t);

    static int buddy_free(void*, size_t);

    static int buddy_free_recursive(void*, size_t);

    static void print();

    static alloc_info allocd;

    static alloc_info deallocd;

private:
    enum Status {
        FREE   = 0,
        ALLOCD = 1,
        SPLIT  = 2
    };

    static list_t *free_list[BUCKET_COUNT];

    // serijalizovano binarno stablo u nizu.
    static uint8 is_used[(size_t)1 << BUCKET_COUNT];

    static int tree_height;

    static uint8 *base_ptr;

    static uint8 *end_ptr;

    static uint get_bucket(size_t);

    static void *get_buddy(uint64*);

    static bool grow_tree(int);

    static void flip_is_used(uint64, Status);

    static void split(void*, int, int);

    static uint64 get_idx(void *ptr, uint bucket);

    static uint8 *get_ptr(uint64 idx, uint bucket);

    static void print_tree();
};


#endif //_buddy_hpp
