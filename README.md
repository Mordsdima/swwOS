# swwOS (Something Went Wrong OS)

## what is it?

its a swwOS (currently only swwk), an ARMv8(and v9?)/RISC-V(in future) os (currently only kernel)

## parts
 - swwk, microkernel
 - nothing more yet

## todo

- [x] PMM
- [ ] (W.I.P.) VFS
- [x] Interrupts
- [ ] SMBIOS
- [ ] ACPI 
- [ ] Modules

## how to build
literally to make do it.
 - install aarch64-linux-gnu-gcc
 - `make`
 - to run download aarch64 ovmf as ovmf.fd and run `make run`

## license

[ISC](LICENSE)