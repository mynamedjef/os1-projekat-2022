//
// Created by djafere on 6/23/22.
//

#include "../h/_buffer.hpp"
#include "../h/syscall_c.h"

char *_buffer::buf = (char*)__mem_alloc(sizeof(char) * BUFFER_SIZE);

int _buffer::cap = BUFFER_SIZE;

int _buffer::size = 0;

int _buffer::head = 0;

int _buffer::tail = 0;

void _buffer::insert(char c)
{
    if (size == cap) { return; }
    
    size++;
    buf[tail] = c;
    tail = (tail + 1) % cap;
}

char _buffer::get()
{
    if (size == 0) { return EOF; }
    
    size--;
    char ret = buf[head];
    head = (head + 1) % cap;
    return ret;
}

