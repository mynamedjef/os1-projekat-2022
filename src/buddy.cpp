//
// Created by os on 2/14/23.
//

#include "../h/buddy.hpp"
#include "../test/printing.hpp"

list_t *buddy::free_list[BUCKET_COUNT];

uint8 buddy::is_used[(size_t)1 << BUCKET_COUNT];  // TODO: bitmapa umesto niza enuma

int buddy::tree_height = BUCKET_COUNT - 1;

uint8 *buddy::base_ptr = nullptr;

uint8 *buddy::end_ptr = nullptr;

alloc_info buddy::allocd = {0, 0};

alloc_info buddy::deallocd = {0, 0};

/*
* Postavlja početno stanje alokatora.
* Poravnava početak i kraj heap-a na veličinu bloka i
* označava jednu najmanju jedinicu alokacije kao dostupnu
*/
void buddy::init(const void *heap_start, const void *heap_end)
{
    base_ptr = (uint8*)heap_start;
    end_ptr = (uint8*)heap_end;

    // poravnanje na MEM_BLOCK_SIZE
    uint64 remainder = (uint64)base_ptr % MEM_BLOCK_SIZE;
    if (remainder != 0)
    {
        base_ptr += MEM_BLOCK_SIZE - remainder;
    }
    end_ptr = (uint8*)((uint64)end_ptr & ~(MEM_BLOCK_SIZE-1));

    // ubacivanje u listu slobodnih
    list_t *start = (list_t*)base_ptr;
    start->next = start->prev = nullptr;

    for (int i = 0; i < BUCKET_COUNT; free_list[i++] = nullptr);

    free_list[BUCKET_COUNT-1] = start;
}

/*
* Povećava stablo do visine "target". 0-ti nivo je koren stabla.
* Vraća false samo u slučaju da je nemoguće podići stablo više.
*/
bool buddy::grow_tree(int target)
{
    // stablo ne može više rasti
    if (target < 0)
    {
        return false;
    }
    // stablo je već na ovom nivou ili većem
    if (target >= tree_height)
    {
        return true;
    }

    /*
    * u slučaju da je koren prethodnog stabla slobodan, samo ga dižemo na veći nivo.
    * taj čvor sigurno kreće od base_ptr, jer je do tad bio koren prethodnog stabla
    */
    if (free_list[tree_height] != nullptr)
    {
        list_pop(&free_list[tree_height]);
        tree_height = target;
        list_push(&free_list[tree_height], (list_t*)base_ptr);
        return true;
    }

    /*
    * Koren prethodnog stabla nije slobodan.
    * Dok ne stignemo do ciljne visine stabla radimo sledeće:
    * 1. Povećavamo stablo
    * 2. Novi koren označavamo kao podeljen (jer mu je levo dete, dosadašnji koren, nije slobodno)
    * 3. Desno dete dodajemo u slobodnu listu nivoa ispod novog korena.
    */
    while (target < tree_height)
    {
        tree_height--;
        uint64 idx = get_idx((void*)base_ptr, tree_height);
        flip_is_used(idx, SPLIT);
        uint8 *ptr = get_ptr(RCHILD(idx), tree_height + 1);
        list_push(&free_list[tree_height + 1], (list_t*)ptr);
    }

    return true;
}

/*
* Dohvata kofu u zavisnosti od toga koliko bajtova je zahtevano.
* 0-ta kofa je za MAX_ALLOC, (BUCKET_COUNT-1)-ta kofa je za MIN_ALLOC
*/
uint buddy::get_bucket(size_t alloc)
{
    uint bucket = BUCKET_COUNT-1;
    size_t size = MIN_ALLOC;
    while (alloc > size)
    {
        bucket--;
        size *= 2;
    }
    return bucket;
}

/*
* Alocira memoriju veličine prvog stepena dvojke koji je veći ili jednak "size"
*/
void *buddy::buddy_alloc(size_t size)
{
    if (size > MAX_ALLOC)
    {
        return nullptr;
    }

    uint bucket = get_bucket(size);  // u kojoj kofi pripada alokacija
    list_t *ptr = nullptr;
    int height = bucket;  // ciljana visina stabla

    /*
    * Dok god ne nađemo odgovarajuće parče memorije: radimo sledeće
    * 1. Pokušavamo da povećamo stablo na traženu visinu. Neuspeh znači da je alokacija nemoguća.
    * 2. Proveravamo da li postoji kofa koja zadovoljava trenutnu veličinu ili veće
    * 3a. Našli smo parče: radimo split (split je no-op ako je parče tačne veličine) i izlazimo iz while petlje
    * 3b. Nismo našli kofu: pokušavamo da povećamo stablo za još jedan nivo.
    */
    while (ptr == nullptr)
    {
        if (!grow_tree(height))  // ako ne možemo da povećamo stablo, alokacija je neuspešna
        {
            return nullptr;
        }
        for (int i = bucket; i >= tree_height; i--)
        {
            if (free_list[i] != nullptr)
            {
                // ako je pronađeno parče memorije veće od traženog, delimo ga na više parčića
                ptr = (list_t*)split_until_available(bucket, i);
                break;
            }
        }
        height = tree_height-1;  // u slučaju da nismo našli u trenutnom stablu, moramo da ga povećamo
    }

    allocd.bytes += size;
    allocd.times++;

    uint64 idx = get_idx(ptr, bucket);
    flip_is_used(idx, ALLOCD);  // alociran čvor se označava kao korišćen
    return (void*)ptr;
}

/*
* Delimo veće parče memorije sve dok ne dobijemo parče koje smo tražili,
* na kraju čega vraćamo traženo parče.
*/
list_t *buddy::split_until_available(int target, int curr)
{
    list_t *ret = list_pop(&free_list[curr]);
    /*
    * Dok god nismo stigli do traženog nivoa radimo sledeće:
    * 1. Označavamo čvor kao podeljen
    * 2. Desno dete dodajemo u listu slobodnih
    * 3. Levo dete nastavljamo da delimo
    *
    * Provera za (ret != nullptr) nije potrebna jer se split poziva samo u slučaju da je (free_list[curr] != nullptr)
    */
    while (target > curr && ret)
    {
        uint64 idx = get_idx((void*)ret, curr);
        flip_is_used(idx, SPLIT);
        uint8 *rchild = get_ptr(RCHILD(idx), curr + 1);
        list_push(&free_list[curr + 1], (list_t*)rchild);

        curr++;
    }
    return ret;
}

/*
* Oslobađa memoriju sa lokacije loc veličine size.
* Prosleđivanje argumenta "size" omogućava da ne koristimo zaglavlja sa informacijom
* o veličini parčeta koje som alocirali.
* Vraća 0 u slučaju uspeha.
*/
int buddy::buddy_free(void *loc, size_t size)
{
    if ((uint8*)loc < base_ptr || (uint8*)loc >= end_ptr)
    {
        return 1;
    }

    int bucket = get_bucket(size);
    uint64 idx = get_idx(loc, bucket);

    if (is_used[idx] != ALLOCD)
    {
        return 2;
    }

    deallocd.bytes += size;
    deallocd.times++;

    while (true)
    {
        // označavamo čvor kao slobodan
        is_used[idx] = FREE;

        uint64 buddy_idx = SIBLING(idx);

        /*
        *  u slučaju da smo stigli do vrha stabla, ili da naš parnjak nije slobodan,
        *  upisujemo oslobođen segment u listu slobodnih i uspešno završavamo oslobađanje
        */
        if (bucket == tree_height || is_used[buddy_idx] != FREE)
        {
            list_push(&free_list[bucket], (list_t*)loc);
            return 0;
        }

        // spajamo parnjake - sklanjamo parnjaka iz slobodne liste da bi spojen čvor ubacili na viši nivo
        uint8 *buddy_ptr = get_ptr(buddy_idx, bucket);
        list_remove(&free_list[bucket], (list_t*)buddy_ptr);

        // spremanje za sledeću iteraciju
        loc = (idx < buddy_idx) ? loc : (void*)buddy_ptr;
        idx = get_idx(loc, --bucket);
    }
}

/*
* Isto kao običan free samo rekurzivno radi.
*/
int buddy::buddy_free_recursive(void *loc, size_t size)
{
    if ((uint8*)loc < base_ptr || (uint8*)loc >= end_ptr)
    {
        return 1;
    }

    uint bucket = get_bucket(size);
    uint64 idx = get_idx(loc, bucket);

    if (is_used[idx] == FREE)
    {
        return 2;
    }

    is_used[idx] = FREE;
    uint64 buddy_idx = SIBLING(idx);
    if (bucket == 0 || is_used[buddy_idx] != FREE)
    {
        list_push(&free_list[bucket], (list_t*)loc);
        return 0;
    }

    uint8 *buddy_ptr = get_ptr(buddy_idx, bucket);
    list_remove(&free_list[bucket], (list_t *)buddy_ptr);
    return buddy_free_recursive(
        (idx < buddy_idx) ? loc : (void*)buddy_ptr,
        size << 1
    );
}

/*
* Za lokaciju "ptr" i njenu kofu dohvata relevantni indeks u serijalizovanom binarnom stablu.
*/
uint64 buddy::get_idx(void *ptr, uint bucket)
{
    // pošto serijalizovano stablo izgleda kao 011222233333334444444444444444 u kome su brojevi "kofa" kojoj
    // pripadaju, ova promenljiva predstavlja "bazu". base(0)=0, base(1)=1, base(2)=3, base(3)=7, base(4)=15...
    uint64 base = (1 << bucket) - 1;

    // (ptr - heap_start): "sirova" vrednost pointera.
    // (1 << (MAX_ALLOC_LOG2 - bucket)): što je vrednost kofe manja (odnosi se na veće alokacije) to dobijamo veći
    // stepen dvojke. To je logično, jer kofe sa manjim vrednostima imaju manju količinu jedinica alokacije.
    // Kasnije ovu vrednost koristimo kao delilac da bi pronašli koji je indeks čvora "u okviru kofe".
    uint64 offset = ((uint8*)ptr - base_ptr) / (1 << (MAX_ALLOC_LOG2 - bucket));
    return base + offset;
}

/*
* Za indeks serijalizovanog binarnog stabla "idx" i njegovu kofu dohvatamo memorijsku lokaciju
*/
uint8 *buddy::get_ptr(uint64 idx, uint bucket)
{
    return base_ptr + (idx + 1 - (1 << bucket)) * (1 << (MAX_ALLOC_LOG2 - bucket));
}

void buddy::flip_is_used(uint64 idx, Status status)
{
    is_used[idx] = status;
}

void putc_n(char c, int n)
{
    for (int i = 0; i < n; i++) { putc(c); }
}

void buddy::print_tree()
{
    int height = BUCKET_COUNT;
//    int total = (1 << BUCKET_COUNT) - 1;
//    int nodeMax = 1 << (BUCKET_COUNT - 1);
    for (int i = 0; i < height; i++)
    {
        int nodeCnt = (1 << i);
        int lSpacing = (1 << (BUCKET_COUNT - (1 + i))) - 1;
        int intermediate = (1 << (BUCKET_COUNT - i)) - 1;

        putc_n(' ', lSpacing);

        int offset = (1 << i) - 1;
        for (int j = 0; j < nodeCnt; j++)
        {
            putc(is_used[offset+j] + '0');
            putc_n(' ', intermediate);
        }
        putc('\n');
    }
}

/*
* Ispis binarnog stabla alokacije i slobodnih lista
*/
void buddy::print()
{
    // print_tree(); putc('\n');
    printString("Allocated: "); printInt(allocd.bytes); printString("B (x"); printInt(allocd.times); printString(")\n");
    printString("Deallocated: "); printInt(deallocd.bytes); printString("B (x"); printInt(deallocd.times); printString(")\n");
    for (int i = 0; i < BUCKET_COUNT; i++)
    {
        if (free_list[i] == nullptr) { continue; }
        printString("Bucket ");
        printInt(i);
        printString(" (2**");
        printInt(MIN_ALLOC_LOG2 + BUCKET_COUNT - (i + 1));
        printString("): ");
        for (list_t *curr = free_list[i]; curr; curr = curr->next)
        {
            printString("\n");
            printHexa((uint64)curr);
            printString(":");
            printHexa((uint64)curr->next);
            printString(" ");
            printHexa((uint64)curr->prev);
            printString(" ");
        }
        printString("\n");
    }
}
