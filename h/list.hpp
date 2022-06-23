
#ifndef _list_hpp
#define _list_hpp

#include "../lib/mem.h"

template<typename T>
class List
{
protected:
    struct Elem
    {
        T *data;
        Elem *next;

        Elem(T *data, Elem *next) : data(data), next(next) {}
        
        void *operator new(size_t size) { return __mem_alloc(size); }
        void operator delete(void *ptr) { __mem_free(ptr); }
    };

    Elem *head, *tail;
    unsigned count;
    
    // ubacuje element data posle elementa node. ako je node null, ne radi ništa.
    void insertAfter(Elem *node, T *data)
    {
        if (!node) return;
        count++;
        Elem *next = node->next;
        node->next = new Elem(data, next);
    }

public:
    List() : head(0), tail(0), count(0) {}

    List(const List<T> &) = delete;

    List<T> &operator=(const List<T> &) = delete;
    
    unsigned size() const { return count; }
    
    void addFirst(T *data)
    {
        Elem *elem = new Elem(data, head);
        count++;
        head = elem;
        if (!tail) { tail = head; }
    }

    void addLast(T *data)
    {
        Elem *elem = new Elem(data, 0);
        count++;
        if (tail)
        {
            tail->next = elem;
            tail = elem;
        } else
        {
            head = tail = elem;
        }
    }

    T *removeFirst()
    {
        if (!head) { return 0; }

        count--;
        Elem *elem = head;
        head = head->next;
        if (!head) { tail = 0; }

        T *ret = elem->data;
        delete elem;
        return ret;
    }

    T *peekFirst()
    {
        if (!head) { return 0; }
        return head->data;
    }

    T *removeLast()
    {
        if (!head) { return 0; }

        count--;
        Elem *prev = 0;
        for (Elem *curr = head; curr && curr != tail; curr = curr->next)
        {
            prev = curr;
        }

        Elem *elem = tail;
        if (prev) { prev->next = 0; }
        else { head = 0; }
        tail = prev;

        T *ret = elem->data;
        delete elem;
        return ret;
    }

    T *peekLast()
    {
        if (!tail) { return 0; }
        return tail->data;
    }
};

#endif //_list_hpp
