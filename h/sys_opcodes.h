//
// Created by djafere on 6/17/22.
//

#ifndef _sys_opcodes
#define _sys_opcodes

enum Opcodes {
	MEM_ALLOC       = 0x01,
    MEM_FREE        = 0x02,
    THREAD_CREATE   = 0x11,
    THREAD_EXIT     = 0x12,
    THREAD_DISPATCH = 0x13,
    SEM_OPEN        = 0x21,
    SEM_CLOSE       = 0x22,
    SEM_WAIT        = 0x23,
    SEM_SIGNAL      = 0x24,
    TIME_SLEEP      = 0x31,
    GETC            = 0x41,
    PUTC            = 0x42,
	YIELD           = 0x50,
};

#endif //_sys_opcodes
