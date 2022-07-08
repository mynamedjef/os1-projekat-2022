//
// Created by djafere on 7/8/22.
//

#include "../h/syscall_cpp.hpp"

char Console::getc()
{
    return ::getc();
}

void Console::putc(char c)
{
    ::putc(c);
}
