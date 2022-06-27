//
// Created by djafere on 6/27/22.
//

#include "../h/syscall_cpp.hpp"

char Console::getc()
{
    return ::getc();
}
void Console::putc(char c)
{
    return ::putc(c);
}
