//
// Created by djafere on 6/23/22.
//

#ifndef __buffer_hpp
#define __buffer_hpp

#include "list.hpp"

#define BUFFER_SIZE 4096

class _buffer {
//private:
    static char* buf;

    static int head, tail, size, cap;

public:
    static void insert(char c);

    static char get();
};

#endif //__buffer_hpp
