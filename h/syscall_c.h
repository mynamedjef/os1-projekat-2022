//
// Created by djafere on 5/8/22.
//

#ifndef _syscall_c
#define _syscall_c

#include "../lib/hw.h"

// ============== memorija ===============

void *mem_alloc(size_t size);
int mem_free(void*);

// ============== niti ===================

class _thread;
typedef _thread *thread_t;
int thread_create(
	thread_t *handle,
	void(*start_routine)(void*),
	void *arg
);
int thread_prepare(
    thread_t *handle,
    void(*start_routine)(void*),
    void *arg
);

int thread_start(thread_t handle);
int thread_exit();
void thread_dispatch();

// ============== semafori ===============

class _sem;
typedef _sem *sem_t;
int sem_open(
	sem_t *handle,
	unsigned init
);

int sem_close(sem_t handle);
int sem_wait(sem_t id);
int sem_signal(sem_t id);
int sem_prio(sem_t id);

// ============== utils ==================

typedef unsigned long time_t;
int time_sleep(time_t);

const int EOF = -1;
char getc();
void putc(char);


#endif //_syscall_c
