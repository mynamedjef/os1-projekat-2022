
#include "../h/syscall_c.h"
#include "../test/printing.hpp"

struct args {
    sem_t user_sem, mutex;
    char id;
};

const int worker_cnt = 3;
const int print_cnt = 10;
const int timeout = 5;

void worker(void *arg)
{
    args *a = (args*)arg;
    for (int i = 0; i < print_cnt; i++) {
         time_sleep(timeout);
         sem_wait(a->mutex);

         int id = get_thread_id();
         char worker = a->id;
         putc(worker);
         putc(':');
         printInt(id);
         putc(',');

         sem_signal(a->mutex);
    }
    sem_signal(a->user_sem);
    delete a;
}

void userMain()
{
    sem_t user_sem, mutex;
    sem_open(&user_sem, 0);
    sem_open(&mutex, 1);
    thread_t workers[worker_cnt];
    for (int i = 0; i < worker_cnt; i++) {
        args *a = new args{user_sem, mutex, (char)('A' + i)};
        thread_create(&workers[i], worker, a);
    }

    sem_wait(user_sem);
    sem_wait(user_sem);
    sem_wait(user_sem);
    putc('\n');
}
