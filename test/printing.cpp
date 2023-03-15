//
// Created by os on 5/18/22.
//

#include "printing.hpp"

uint64 lockPrint = 0;

#define LOCK() while(copy_and_swap(lockPrint, 0, 1))
#define UNLOCK() while(copy_and_swap(lockPrint, 1, 0))

void printString(char const *string)
{
    LOCK();
    while (*string != '\0')
    {
        putc(*string);
        string++;
    }
    UNLOCK();
}

char* getString(char *buf, int max) {
    LOCK();
    int i, cc;
    char c;

    for(i=0; i+1 < max; ){
        cc = getc();
        if(cc < 1)
            break;
        c = cc;
        buf[i++] = c;
        if(c == '\n' || c == '\r')
            break;
    }
    buf[i] = '\0';

    UNLOCK();
    return buf;
}

int stringToInt(const char *s) {
    int n;

    n = 0;
    while ('0' <= *s && *s <= '9')
        n = n * 10 + *s++ - '0';
    return n;
}

char digits[] = "0123456789ABCDEF";

void printInt(int xx, int base, int sgn)
{
    LOCK();
    char buf[16];
    int i, neg;
    uint x;

    neg = 0;
    if(sgn && xx < 0){
        neg = 1;
        x = -xx;
    } else {
        x = xx;
    }

    i = 0;
    do{
        buf[i++] = digits[x % base];
    }while((x /= base) != 0);
    if(neg)
        buf[i++] = '-';

    while(--i >= 0)
        putc(buf[i]);

    UNLOCK();
}

void printHexa(uint64 xx, uint padding)
{
    LOCK();
    char buffer[16];
    for (int i = 7; i >= 0; i--)
    {
        uint8 byte = (xx >> (8 * i)) & 0xFF; // i-ti bajt
        buffer[15 - (i*2 + 1)] = ("0123456789ABCDEF"[byte >> 4]);
        buffer[15 - (i*2)] = ("0123456789ABCDEF"[byte & 0x0F]);
    }

    putc('0'); putc('x');

    int i = 0;
    while (i < 15 && buffer[i] == '0') i++;

    int len = 16 - i;
    int diff = padding - len;

    for (int j = 0; j < diff; j++) putc('0'); // padding
    while (i < 16) putc(buffer[i++]);
    UNLOCK();
}

// uzeto sa "https://gist.github.com/domnikl/af00cc154e3da1c5d965"
void hexDump(char *desc, void *addr, int len)
{
    LOCK();
    int i;
    unsigned char buff[17];
    unsigned char *pc = (unsigned char*)addr;

    // Output description if given.
    if (desc != nullptr)
    {
        printString((const char*)desc);
        printString(":\n");
    }

    // Process every byte in the data.
    for (i = 0; i < len; i++)
    {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0)
        {
            // Just don't print ASCII for the zeroth line.
            if (i != 0)
            {
                printString("  ");
                printString((const char*)buff);
                putc('\n');
            }

            // Output the offset.
            printString("  ");
            printHexa(i, 4);
            putc(' ');
        }

        // Now the hex code for the specific character.
        putc(' ');
        printHexa(pc[i], 2);

        // And store a printable ASCII character for later.
        buff[i % 16] = (pc[i] < 0x20 || pc[i] > 0x7e) ?
                '.' :
                pc[i];

        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0)
    {
        printString("   ");
        i++;
    }

    // And print the final ASCII bit.
    printString("  ");
    printString((const char*)buff);
    putc('\n');
    UNLOCK();
}
