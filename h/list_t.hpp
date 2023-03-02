
#ifndef _list_t_hpp
#define _list_t_hpp

/*
* dvostruko ulančana lista sa o(1) insert/remove
* koristi se za buddy alokator: ovom strukturom se ulančavaju slobodni
* parčići memorije. list_t se upisuje na početak tih slobodnih
* parčića memorije.
*
* ova lista se može implementirati i kao kružna dvostruko ulančana lista
* što je i dosta efikasnije jer nema grananja.
*/

struct list_t
{
    list_t *next;
    list_t *prev;
};

void list_push(list_t **list, list_t *entry);

void list_remove(list_t **list, list_t *entry);

list_t *list_pop(list_t **list);

#endif // _list_t_hpp
