#!/bin/sh

case $1 in
    #aarch64)
    #    qemu-system-aarch64 -M virt -cpu cortex-a72 -device ramfb \
    #    -drive file=fat:rw:vfs,if=none,id=drv -bios ./ovmf.fd \
    #    -device virtio-blk-device,drive=drv -m 1G -device qemu-xhci \
    #    -device usb-kbd -s -d int
    #;;

    rv64)
        mv ./out/kernel-rv64 ./vfs/
        qemu-system-riscv64 -M virt -cpu rv64 -device ramfb \
        -drive file=fat:rw:vfs,if=none,id=drv -drive if=pflash,unit=0,format=raw,file=rv-ovmf.fd,readonly=on \
        -device virtio-blk-device,drive=drv -m 1G -device qemu-xhci \
        -device usb-kbd -s -d int
    ;;
esac
