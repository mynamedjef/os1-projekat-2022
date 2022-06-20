//
// Created by djafere on 6/20/22.
//

#ifndef _sleep_list_hpp
#define _sleep_list_hpp

#include "list.hpp"
#include "tcb.hpp"
#include "syscall_c.h"

struct Sleeping {
    TCB *tcb;
    time_t time;
    Sleeping(TCB *tcb, time_t time) : tcb(tcb), time(time) {}
};

class SleepList : public List<Sleeping> {
public:
    bool Comparator(Sleeping *t1, Sleeping *t2)
    {
        return (t1->time < t2->time);
    }

    void tick()
    {
        passed = (size() < 1) ? 0 : passed+1;
    }

    bool is_ready()
    {
        return (peekFirst()->time <= passed);
    }

    time_t time_passed()
    {
        return passed;
    }

    void addPriority(Sleeping *data) {
        if (!head || data->time < head->data->time)
        {
            addFirst(data);
            return;
        }
        else
        {
            Elem *elem = new Elem(data, nullptr);
            count++;

            Elem *prev = head;
            for (Elem *curr = head->next; curr; curr = curr->next) {
                if (data->time < curr->data->time) {
                    prev->next = elem;
                    elem->next = curr;
                    return;
                }
            }
            prev->next = elem;
            tail = elem;
        }
    }

private:
    time_t passed;
};

#endif //_sleep_list_hpp
