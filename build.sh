case $1 in

  kernel)
    ninja $2.elf
    mv -f $2.elf swwk.elf
    ;;

  initrd)
    cd initrd && tar -cvf ../initrd.tar * && cd ..
    ;;

  run)
    mv -f ./swwk.elf ./vfs/
    mv -f ./initrd.tar ./vfs/
    sh run.sh
    ;;

  *)
    echo "No command provided. Available:"
    echo "kernel <arch>"
    echo "initrd"
    ;;
esac
