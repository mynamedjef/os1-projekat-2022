.extern _ZN5Riscv18handleUserModeTrapEv

.align 4
.global _ZN5Riscv12userModeTrapEv
.type _ZN5Riscv12userModeTrapEv, @function
_ZN5Riscv12userModeTrapEv:
    call _ZN5Riscv18handleUserModeTrapEv
    # csrs sstatus, 0x10 # nije potrebno jer U-mode ima automatski dozvoljene prekide
    sret
