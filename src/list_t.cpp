
#include "../h/list_t.hpp"

void list_push(list_t **list, list_t *entry)
{
    if (!list || !entry) { return; }
    entry->next = entry->prev = nullptr;

    list_t *head = *list;
    if (head == nullptr) {
        *list = entry;
        return;
    }

    head->prev = entry;
    entry->next = head;
    *list = entry;
}

void list_remove(list_t **list, list_t *entry)
{
    if (!list || !entry) { return; }

    list_t *head = *list;
    list_t *prev = entry->prev;
    list_t *next = entry->next;
    if (prev) { prev->next = next; }
    if (next) { next->prev = prev; }
    entry->prev = entry->next = nullptr;

    if (head == entry)
    {
        *list = next;
    }
}

list_t *list_pop(list_t **list)
{
    if (!list) { return nullptr; }
    list_t *head = *list;
    list_remove(list, head);
    return head;
}
