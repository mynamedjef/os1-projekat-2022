
#ifndef _scheduler_hpp
#define _scheduler_hpp

#include "list.hpp"

class TCB;

/*
 * najobičniji FIFO scheduler implementiran jednostruko ulančanom listom sa head/tail pokazivačima
 * prednost je što nema nikakvo dinamičko zauzeće memorije, jedino što TCB mora da ima polje `TCB *next`.
 */
class Scheduler
{
private:
    static TCB *head;

    static TCB *tail;

    static int count;  // za debagovanje

public:
    static TCB *get();

    static void put(TCB*);

    static int size();

    static void flush_user_threads(); // Izbacuje sve niti koje je korisnik napravio iz scheduler-a (nasilno ih gasi)
};

#endif //_scheduler_hpp
