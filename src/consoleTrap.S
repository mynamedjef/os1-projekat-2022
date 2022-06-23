.extern _ZN5Riscv17handleConsoleTrapEv

.align 4
.global _ZN5Riscv11consoleTrapEv
.type _ZN5Riscv11consoleTrapEv, @function
_ZN5Riscv11consoleTrapEv:

    call _ZN5Riscv17handleConsoleTrapEv

    sret
