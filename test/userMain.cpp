
#include "../h/syscall_cpp.hpp"
#include "../test/printing.hpp"
#include "../h/_thread.hpp"

unsigned short rand() {
    static unsigned short lfsr = 0xACE1u;
    unsigned bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1;
    return lfsr = (lfsr >> 1) | (bit << 15);
}

class Worker : public Thread {
public:
    Worker(sem_t s) : sem(s) {}
protected:
    void run() override
    {
        unsigned id = myHandle->id();
        while (true) {
            sem_wait(sem);
            
            printInt(id);
            printString(" - ušao u kritičnu sekciju.\n");
            // ------------------------------
            unsigned sleep = rand() % 1000;
            time_sleep(sleep);
            // ------------------------------
//            printInt(id);
//            printString(" - izašao iz kritične sekcije.\n");
            sem_prio(sem);
        }
    }
    sem_t sem;
};

void userMain() {
    sem_t s;
    sem_open(&s, 5);
    Thread *threads[50];
    for (auto &thr : threads) {
        thr = new Worker(s);
        thr->start();
    }
    
    while (true) { }

}