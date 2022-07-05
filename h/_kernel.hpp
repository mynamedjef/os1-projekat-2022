//
// Created by djafere on 7/5/22.
//

#ifndef OS1_2022__KERNEL_HPP
#define OS1_2022__KERNEL_HPP

#include "../lib/hw.h"

class Kernel {
public:
    static uint64(**calls)(uint64*);

    static uint64 exec(uint64*);

    static void init();

private:
    static uint64 __malloc(uint64*);
    static uint64 __free(uint64*);
    static uint64 __thr_dispatch(uint64*);
    static uint64 __thr_create(uint64*);
    static uint64 __thr_prep(uint64*);
    static uint64 __thr_start(uint64*);
    static uint64 __thr_exit(uint64*);
    static uint64 __sem_open(uint64*);
    static uint64 __sem_close(uint64*);
    static uint64 __sem_wait(uint64*);
    static uint64 __sem_sig(uint64*);
    static uint64 __sleep(uint64*);
    static uint64 __getchar(uint64*);
    static uint64 __putchar(uint64*);
    static uint64 __user(uint64*);
    static uint64 __super(uint64*);

};

#endif //OS1_2022__KERNEL_HPP
