//
// Created by marko on 20.4.22..
//

#ifndef OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_RISCV_HPP
#define OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_RISCV_HPP

#include "../lib/hw.h"
#include "../h/_buffer.hpp"
#include "../h/_sleeplist.hpp"

class Riscv
{
public:

    // pop sstatus.spp and sstatus.spie bits (has to be a non inline function)
    static void popSppSpie();

    // read register scause
    static uint64 r_scause();

    // write register scause
    static void w_scause(uint64 scause);

    // read register sepc
    static uint64 r_sepc();

    // write register sepc
    static void w_sepc(uint64 sepc);

    // read register stvec
    static uint64 r_stvec();

    // write register stvec
    static void w_stvec(uint64 stvec);

    // read register stval
    static uint64 r_stval();

    // write register stval
    static void w_stval(uint64 stval);

    enum BitMaskSip: uint64
    {
        SIP_SSIP = (1UL << 1),
        SIP_STIP = (1UL << 5),
        SIP_SEIP = (1UL << 9),
    };

    // mask set register sip
    static void ms_sip(uint64 mask);

    // mask clear register sip
    static void mc_sip(uint64 mask);

    // read register sip
    static uint64 r_sip();

    // write register sip
    static void w_sip(uint64 sip);

    enum BitMaskSstatus: uint64
    {
        SSTATUS_SIE = (1UL << 1),
        SSTATUS_SPIE = (1UL << 5),
        SSTATUS_SPP = (1UL << 8),
    };

    // mask set register sstatus
    static void ms_sstatus(uint64 mask);

    // mask clear register sstatus
    static void mc_sstatus(uint64 mask);

    // read register sstatus
    static uint64 r_sstatus();

    // write register sstatus
    static void w_sstatus(uint64 sstatus);

    // supervisor trap
    static void supervisorTrap();

    static void restorePrivilege(); // Restaurira ispravne privilegije nakon povratka iz prekidne rutine.

    static _buffer *bufin, *bufout; // Ulazni i izlazni baferi.

    static void init(); // Inicijalizacija bafera za ulaz i izlaz.

    static _sleeplist sleeplist; // Lista uspavanih niti

private:
    static void w_retval(uint64); // upis u registar a0 (preko koga se prenosi povratna vrednost sis. poziva)

    static void handleSupervisorTrap(); // funkcija kojom obrađujemo sve prekide

    static void handleTimer(); // obrada prekidne rutine prouzrukovane tajmerom

    static void handleHardware(); // obrada prekidne rutine prouzrokovane tastaturom

    static void handleUnexpectedTrap(); // obrada prekidne rutine prouzrokovane izuzetkom

    static void loadParams(uint64*); // učitavanje parametara sistemskog poziva u niz

    static uint64 syscall(uint64*); // izvršavanje sistemskog poziva (parametri su prosleđeni nizom)

};

inline uint64 Riscv::r_scause()
{
    uint64 volatile scause;
    __asm__ volatile ("csrr %[scause], scause" : [scause] "=r"(scause));
    return scause;
}

inline void Riscv::w_scause(uint64 scause)
{
    __asm__ volatile ("csrw scause, %[scause]" : : [scause] "r"(scause));
}

inline uint64 Riscv::r_sepc()
{
    uint64 volatile sepc;
    __asm__ volatile ("csrr %[sepc], sepc" : [sepc] "=r"(sepc));
    return sepc;
}

inline void Riscv::w_sepc(uint64 sepc)
{
    __asm__ volatile ("csrw sepc, %[sepc]" : : [sepc] "r"(sepc));
}

inline uint64 Riscv::r_stvec()
{
    uint64 volatile stvec;
    __asm__ volatile ("csrr %[stvec], stvec" : [stvec] "=r"(stvec));
    return stvec;
}

inline void Riscv::w_stvec(uint64 stvec)
{
    __asm__ volatile ("csrw stvec, %[stvec]" : : [stvec] "r"(stvec));
}

inline uint64 Riscv::r_stval()
{
    uint64 volatile stval;
    __asm__ volatile ("csrr %[stval], stval" : [stval] "=r"(stval));
    return stval;
}

inline void Riscv::w_stval(uint64 stval)
{
    __asm__ volatile ("csrw stval, %[stval]" : : [stval] "r"(stval));
}

inline void Riscv::ms_sip(uint64 mask)
{
    __asm__ volatile ("csrs sip, %[mask]" : : [mask] "r"(mask));
}

inline void Riscv::mc_sip(uint64 mask)
{
    __asm__ volatile ("csrc sip, %[mask]" : : [mask] "r"(mask));
}

inline uint64 Riscv::r_sip()
{
    uint64 volatile sip;
    __asm__ volatile ("csrr %[sip], sip" : [sip] "=r"(sip));
    return sip;
}

inline void Riscv::w_sip(uint64 sip)
{
    __asm__ volatile ("csrw sip, %[sip]" : : [sip] "r"(sip));
}

inline void Riscv::ms_sstatus(uint64 mask)
{
    __asm__ volatile ("csrs sstatus, %[mask]" : : [mask] "r"(mask));
}

inline void Riscv::mc_sstatus(uint64 mask)
{
    __asm__ volatile ("csrc sstatus, %[mask]" : : [mask] "r"(mask));
}

inline uint64 Riscv::r_sstatus()
{
    uint64 volatile sstatus;
    __asm__ volatile ("csrr %[sstatus], sstatus" : [sstatus] "=r"(sstatus));
    return sstatus;
}

inline void Riscv::w_sstatus(uint64 sstatus)
{
    __asm__ volatile ("csrw sstatus, %[sstatus]" : : [sstatus] "r"(sstatus));
}

inline void Riscv::w_retval(uint64 ret)
{
    __asm__ volatile ("mv a0, %0" : : "r" (ret));
}

inline void Riscv::loadParams(uint64 *arr)
{
    __asm__ volatile ("mv %0, a0" : "=r" (arr[0]));
    __asm__ volatile ("mv %0, a1" : "=r" (arr[1]));
    __asm__ volatile ("mv %0, a2" : "=r" (arr[2]));
    __asm__ volatile ("mv %0, a3" : "=r" (arr[3]));
    __asm__ volatile ("mv %0, a4" : "=r" (arr[4]));
}

#endif //OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_RISCV_HPP
