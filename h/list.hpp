
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

    // Ubacuje element data posle elementa node. ako je node null, ne radi ništa.
    void insertAfter(Elem *node, T *data);

private:
    Elem *head, *tail;

    unsigned count;

public:
    List() : head(0), tail(0), count(0) {}

    List(const List<T> &) = delete;

    List<T> &operator=(const List<T> &) = delete;

    unsigned size() const { return count; }

    void addFirst(T *data);

    void addLast(T *data);

    T *removeFirst();

    T *peekFirst() const;

    T *removeLast();

    T *peekLast() const;

    /*
     * U listu ubacuje element na koji pokazuje 'data'.
     * Pozicija ubacivanja zavisi od funkcije za poređenje comparator(), koja treba
     * da vraća 'true' ako element na koji pokazuje 't1' u listi treba da ide pre elementa na koji pokazuje 't2'.
     */
    void priorityInsert(T *data, bool (*comparator)(T *t1, T *t2));
};

template<typename T>
void List<T>::insertAfter(Elem *node, T *data)
{
    if (!node) return;
    count++;
    Elem *next = node->next;
    node->next = new Elem(data, next);
}

template<typename T>
void List<T>::addFirst(T *data)
{
    Elem *elem = new Elem(data, head);
    count++;
    head = elem;
    if (!tail) { tail = head; }
}

template<typename T>
void List<T>::addLast(T *data)
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

template<typename T>
T *List<T>::removeFirst()
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

template<typename T>
T *List<T>::peekFirst() const
{
    if (!head) { return 0; }
    return head->data;
}

template<typename T>
T *List<T>::removeLast()
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

template<typename T>
T *List<T>::peekLast() const
{
    if (!tail) { return 0; }
    return tail->data;
}

template<typename T>
void List<T>::priorityInsert(T *data, bool (*comparator)(T *t1, T *t2))
{
    if (!head || comparator(data, head->data))
    {
        addFirst(data);
    }
    else
    {
        Elem *prev = head;
        for (Elem *curr = head->next; curr; curr = curr->next) {
            if (comparator(data, curr->data)) {
                insertAfter(prev, data);
                return;
            }
            prev = curr;
        }
        addLast(data);
    }
}

#endif //_list_hpp
