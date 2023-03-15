//
// Created by djafere on 6/23/22.
//

#ifndef __sleeplist_hpp
#define __sleeplist_hpp

#include "tcb.hpp"

/*
 * Struktura koja u sebi čuva TCB i koliko on dugo čeka.
 */
struct SleepNode {
    TCB *tcb;
    time_t timeout;

    SleepNode(TCB *_tcb, time_t _timeout) : tcb(_tcb), timeout(_timeout) { }

    void *operator new(size_t size) { return __mem_alloc(size); }

    void operator delete(void *ptr) { __mem_free(ptr); }
};

/*
 * Kernel klasa koja prati sve niti koje spavaju.
 * Predstavlja prioritetnu listu (na početku su TCB-ovi koji kraće spavaju).
 * Na svaki otkucaj tajmera (tick()), inkrementira se brojač i proverava se da li je vreme da se probudi nit.
 */
class _sleeplist : private List<SleepNode> {
public:
    _sleeplist() : passed(0), total_passed(0) { }

    void insert(TCB *tcb, time_t timeout); // Ubacivanje niti 'tcb' u listu sa vremenom čekanja 'timeout' otkucaja.

    void tick(); // Predstavlja jedan otkucaj. Budi niti u slučaju da je vreme za to.

private:
    TCB *pop();

    time_t passed;

    time_t total_passed;

    bool ready() const; // Proverava da li ima niti spremne za buđenje.

    // Poredi niti po vremenu buđenja zbog unosa u prioritetnu listu.
    static bool Comparator(SleepNode *t1, SleepNode *t2) { return t1->timeout < t2->timeout; }
};

#endif //__sleeplist_hpp
