
#include "../h/slab.hpp"
#include "../h/buddy.hpp"
#include "../test/printing.hpp"

/*
* Koliko najviše moguće najviše imamo keševa.
* Samo za debagovanje, tj. za kmem_all_caches_info().
*/
#define MAX_CACHE_COUNT (BUFFER_COUNT + 10)

static kmem_cache_t *all_caches[MAX_CACHE_COUNT] = {0};

static int cache_id = 0;

// Mali memorijski baferi
static kmem_cache_t *buffers[BUFFER_COUNT] = {0};

/*
* Izračunava koliko najviše objekata može da stane u
* jednu ploču keša "cachep"
*/
inline uint max_objects(kmem_cache_t *cachep)
{
    return cachep->slab_size / cachep->obj_size;
}

// ================ slab-ovi =================
/*
* Zaokružuje veličinu "size" na prvi stepen dvojke
* koji je veći ili jednak BLOCK_SIZE
*/
inline size_t get_slab_size(uint64 size)
{
    // alternativa: https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
    size_t block_size = BLOCK_SIZE;
    while (block_size < size)
    {
        block_size <<= 1;
    }
    return block_size;
}

/*
* Inicijalizuje sva polja ploče na lokaciji "slab" koja pripada
* kešu "cache".
*/
kmem_slab_t *slab_init(kmem_cache_t *cache)
{
    if (cache == nullptr)
    {
        return nullptr;
    }

    kmem_slab_t *slab = (kmem_slab_t*)buddy::buddy_alloc(sizeof(kmem_slab_t));
    if (slab == nullptr)
    {
        return nullptr;
    }

    list_t *file = (list_t*)buddy::buddy_alloc(cache->slab_size);
    if (file == nullptr)
    {
        buddy::buddy_free(slab, sizeof(kmem_slab_t));
        return nullptr;
    }

    slab->count = 0;
    slab->next = slab->prev = nullptr;

    uint8 *base = (uint8*)file;
    list_t *ptr = file;
    slab->free = file;
    slab->file = file;

    list_t *prev = nullptr;

    for (uint i = 1; i <= max_objects(cache); i++)
    {
        ptr->prev = prev;
        ptr->next = (list_t*)(base + i * cache->obj_size);
        prev = ptr;
        ptr = ptr->next;
    }
    prev->next = nullptr;
    return slab;
}

// ================= keš-evi =================

/*
* Inicijalizuje buddy alokator
*/
void kmem_init(void *space, int block_num)
{
    uint8 *end_ptr = (uint8*)space + (block_num << BLOCK_SIZE_LOG2);
    for (int i = 0; i < BUFFER_COUNT; buffers[i++] = nullptr);
    for (int i = 0; i < MAX_CACHE_COUNT; all_caches[i++] = nullptr);
    buddy::init(space, (void*)end_ptr);
}

void kmem_init(void *heap_start, void *heap_end)
{
    buddy::init(heap_start, heap_end);
}

/*
* Pravi keš sa imenom "name" koji će sadržavati objekte
* veličine "obj_size" i koristiti "ctor/dtor" kao konstruktor/destruktor.
*/
kmem_cache_t *kmem_cache_create(const char *name, size_t obj_size,
                                void (*ctor)(void *),
                                void (*dtor)(void *))
{
    // alociranje prostora za keš
    kmem_cache_t *cache = (kmem_cache_t*)buddy::buddy_alloc(sizeof(kmem_cache_t));
    if (cache == nullptr)
    {
        return nullptr;
    }

    // najmanja veličina objekta je sizeof(list_t) jer je list_t zaglavlje u slobodnim parčićima
    obj_size = (obj_size < sizeof(list_t)) ?
        sizeof(list_t) :
        obj_size;

    // postavljanje imena
    int i = 0;
    for (; name[i] && i+1 < KMEM_CACHE_NAME_SIZE; i++) { cache->name[i] = name[i]; }
    cache->name[i] = '\0';

    // postavljanje ostalih polja
    cache->empty = cache->full = cache->mixed = nullptr;
    cache->ctor = ctor;
    cache->dtor = dtor;
    cache->obj_size = obj_size;
    // alociranje prostora za ploču (veličina ploče + veličina objekta * minimum objekata po ploči)
    cache->slab_size = get_slab_size(obj_size << OBJECTS_PER_SLAB_LOG2);

    all_caches[cache_id++] = cache;
    return cache;
}

/*
* Oslobađa slobodan prostor keša "cachep" i vraća
* broj blokova veličine BLOCK_SIZE koji su oslobođeni.
*/
int kmem_cache_shrink(kmem_cache_t *cachep)
{
    if (cachep == nullptr)
    {
        return 0;
    }
    uint64 freed_slabs = 0;
    int status = 0;
    while (cachep->empty != nullptr && status == 0)
    {
        kmem_slab_t *curr = (kmem_slab_t*)list_pop((list_t**)&(cachep->empty));
        status |= buddy::buddy_free(curr->file, cachep->slab_size);
        status |= buddy::buddy_free(curr, sizeof(kmem_slab_t));
        freed_slabs++;
    }
    return (freed_slabs * cachep->slab_size) >> BLOCK_SIZE_LOG2;
}

/*
* Alocira objekat keša "cachep".
*/
void *kmem_cache_alloc(kmem_cache_t *cachep)
{
    if (cachep == nullptr)
    {
        return nullptr;
    }

    // podrazumevano uzmi ploču iz liste praznih
    kmem_slab_t *slab = cachep->empty;
    uint max = 0;

    // pokušaj da nađeš neku koja ima veći broj alociranih
    for (kmem_slab_t *curr = cachep->mixed; curr; curr = curr->next)
    {
        if (curr->count > max)
        {
            max = curr->count;
            slab = curr;
        }
    }

    // ako nisi našao, alociraj novu ploču
    if (slab == nullptr)
    {
        // ubacujemo u "empty" listu
        if ((slab = cachep->empty = slab_init(cachep)) == nullptr)
        {
            return nullptr;
        }
    }

    // ako je sa liste praznih, prebacuje se na listu polu-praznih
    if (slab->count++ == 0)
    {
        list_remove((list_t**)&(cachep->empty), (list_t*)slab);
        list_push((list_t**)&(cachep->mixed), (list_t*)slab);
    }
    // inače, ako je popunjena do kraja, prebacuje se na listu punih
    if (slab->count == max_objects(cachep))
    {
        list_remove((list_t**)&(cachep->mixed), (list_t*)slab);
        list_push((list_t**)&(cachep->full), (list_t*)slab);
    }

    void *ret = list_pop(&slab->free);
    if (cachep->ctor != nullptr) { cachep->ctor(ret); }
    return ret;
}

/*
* Traži pokazivač na ploču kojoj pripada objekat na lokaciji "objp".
*/
kmem_slab_t *find_slab(kmem_slab_t *slab, void *objp, uint64 slab_size)
{
    uint8 *loc = (uint8*)objp;
    for (kmem_slab_t *curr = slab; curr; curr = curr->next)
    {
        uint8 *slab_start = (uint8*)curr->file;
        uint8 *slab_end = slab_start + slab_size;
        if (loc >= slab_start && loc < slab_end)
        {
            return curr;
        }
    }
    return nullptr;
}

/*
* Oslobađa objekat na koji pokazuje "objp" iz keša "cachep"
*/
void kmem_cache_free(kmem_cache_t *cachep, void *objp)
{
    if (cachep == nullptr || objp == nullptr)
    {
        return;
    }

    kmem_slab_t *slab = find_slab(cachep->mixed, objp, cachep->slab_size);
    if (slab == nullptr)
    {
        if ((slab = find_slab(cachep->full, objp, cachep->slab_size)) == nullptr)
        {
            return;
        }
    }

    if (slab->count-- == max_objects(cachep))
    {
        list_remove((list_t**)&cachep->full, (list_t*)slab);
        list_push((list_t**)&cachep->mixed, (list_t*)slab);
    }
    if (slab->count == 0)
    {
        list_remove((list_t**)&cachep->mixed, (list_t*)slab);
        list_push((list_t**)&cachep->empty, (list_t*)slab);
    }

    list_push(&slab->free, (list_t*)objp);
    if (cachep->dtor != nullptr) { cachep->dtor(objp); }
}

/*
* Dohvata indeks malog bafera kome pripada alokacija veličine "alloc" bajtova
*/
uint8 get_idx(size_t alloc)
{
    uint8 idx = BUFFER_COUNT-1;
    size_t size = MIN_BUFFER;
    while (alloc > size)
    {
        idx--;
        size *= 2;
    }
    return idx;
}

/*
* Nalazi mali bafer koji je adekvatan za alokaciju
* veličine "size", alocira prostor te veličine i vraća
* pokazivač na njega.
*/
void *kmalloc(size_t size)
{
    // zaglavlje je neophodno da bi pri oslobađanju znali kom baferu pripada
    size_t gross = size + KMALLOC_HEADER_SIZE;
    uint8 idx = get_idx(gross);
    if (buffers[idx] == nullptr) // moramo da napravimo mali memorijski bafer
    {
        // upisivanje imena bafera
        const char *name = "BUF2^";
        char name_buf[KMEM_CACHE_NAME_SIZE];
        int i = 0;
        for (; i < 5; i++) { name_buf[i] = name[i]; }
        uint log2 = MAX_BUFFER_LOG2 - idx;
        name_buf[i++] = (log2 / 10) + '0';
        name_buf[i++] = (log2 % 10) + '0';
        name_buf[i] = '\0';
        // kreiranje bafera
        buffers[idx] = kmem_cache_create(name_buf, (size_t)1 << log2, nullptr, nullptr);
    }

    uint8 *ret = (uint8*)kmem_cache_alloc(buffers[idx]);
    if (ret == nullptr)
    {
        return nullptr;
    }
    *ret = idx;
    return (void*)(ret + KMALLOC_HEADER_SIZE);
}

/*
* Oslobađamo objekat alociran sa kmalloc
*/
void kfree(const void *objp)
{
    if (objp == nullptr)
    {
        return;
    }
    uint8 *true_loc = (uint8*)objp - KMALLOC_HEADER_SIZE;
    uint8 idx = *true_loc;
    kmem_cache_free(buffers[idx], (void*)true_loc);
}

/*
* Oslobađamo ceo keš "cachep" i sve njegove ploče.
*/
void kmem_cache_destroy(kmem_cache_t *cachep)
{
    if (cachep == nullptr)
    {
        return;
    }

    for (int i = 0; i < cache_id; i++)
    {
        if (all_caches[i] == cachep)
        {
            all_caches[i] = nullptr;
            break;
        }
    }

    const int N = 3;
    kmem_slab_t *slabs[N] = {cachep->empty, cachep->mixed, cachep->full};
    for (int i = 0; i < N; i++)
    {
        kmem_slab_t *curr = slabs[i];
        kmem_slab_t *next = nullptr;
        while (curr)
        {
            next = curr->next;
            buddy::buddy_free(curr->file, cachep->slab_size);
            buddy::buddy_free(curr, sizeof(kmem_slab_t));
            curr = next;
        }
    }
    buddy::buddy_free(cachep, sizeof(kmem_cache_t));
}

/*
* Osobađamo svaki keš koji služi za male memorijske bafere
*/
void kmem_buffers_destroy()
{
    for (int i = 0; i < BUFFER_COUNT; i++)
    {
        if (buffers[i] != nullptr)
        {
            kmem_cache_destroy(buffers[i]);
        }
    }
}

/*
* Ispisujemo sledeće stvari vezane za keš "cachep":
* 1. Veličinu jednog objekta u kešu;
* 2. Veličinu celog keša u blokovima veličine BLOCK_SIZE;
* 3. Broj ploča alociranih;
* 4. Broj objekata po ploči;
* 5. Procentualnu popunjenost keša.
*/
void kmem_cache_info(kmem_cache_t *cachep)
{
    if (cachep == nullptr)
    {
        return;
    }

    const int N = 3;
    kmem_slab_t *slabs[N] = {cachep->empty, cachep->mixed, cachep->full};

    uint slab_cnt = 0;
    uint total_objects = 0;
    for (int i = 0; i < N; i++)
    {
        for (kmem_slab_t *curr = slabs[i]; curr; curr = curr->next)
        {
            total_objects += curr->count;
            slab_cnt++;
        }
    }

    uint maximum_objects = slab_cnt * max_objects(cachep);
    uint block_cnt = (slab_cnt * cachep->slab_size) >> BLOCK_SIZE_LOG2;

    printString("======== "); printString(cachep->name); printString(" ========\n");
    ///*
    printString("full: ");
    for (kmem_slab_t *curr = cachep->full; curr; curr = curr->next) { printHexa((uint64)curr); putc(' '); } putc('\n');
    printString("mixed: ");
    for (kmem_slab_t *curr = cachep->mixed; curr; curr = curr->next) { printHexa((uint64)curr); putc(' '); } putc('\n');
    printString("empty: ");
    for (kmem_slab_t *curr = cachep->empty; curr; curr = curr->next) { printHexa((uint64)curr); putc(' '); } putc('\n');
    //*/
    printString("Veličina jednog objekta: "); printInt(cachep->obj_size); putc('B'); putc('\n');
    printString("Veličina celog keša: "); printInt(block_cnt); printString(" blok(ova)");
    printString(" ["); printInt(slab_cnt * cachep->slab_size); printString("B]\n");
    printString("Broj ploča: "); printInt(slab_cnt); putc('\n');
    printString("Broj objekata u jednoj ploči: "); printInt(max_objects(cachep)); putc('\n');
    if (slab_cnt != 0)
    {
        uint percent_used = 100 * total_objects / maximum_objects;
        printString("Procentualna popunjenost keša: "); printInt(percent_used); printString("% (");
        printInt(total_objects); putc('/'); printInt(maximum_objects); printString(" objekata alocirano)\n");
    }
    printString("=========================\n");
}

/*
* Isto kao i kmem_cache_info - samo za mali memorijski bafer
* kome pripada veličina "size"
*/
void kmem_buf_info(size_t size)
{
    uint idx = get_idx(size);
    kmem_cache_info(buffers[idx]);
}

/*
* TODO: nisam još smislio šta ovo radi
*/
void kmem_cache_error(kmem_cache_t *cachep)
{
    if (cachep == nullptr)
    {
        return;
    }
}

void kmem_all_caches_info()
{
    for (int i = 0; i < cache_id; kmem_cache_info(all_caches[i++]));
}
