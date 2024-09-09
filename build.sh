prepare_build_ninja() {
  cat ./src/build_configs/$1.ninja ./build.ninja.in  > build.ninja
}

case $1 in

  kernel)
    prepare_build_ninja $2
    ninja $2.elf
    mv -f $2.elf swwk.elf
    ;;

  initrd)
    cd initrd && tar -cvf ../initrd.tar * && cd ..
    ;;

  run)
    mv -f ./swwk.elf ./vfs/
    mv -f ./initrd.tar ./vfs/
    sh run.sh $2
    ;;

  *)
    echo "No command provided. Available:"
    echo "kernel <arch>"
    echo "initrd"
    echo "run"
    ;;
esac
