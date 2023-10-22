#!/bin/bash

current_directory=$(basename "$(pwd)")
ramdisk_path=~/ics2020/nanos-lite/build/ramdisk.img

if [ "$current_directory" != "dummy" ] || [ "$current_directory" != "hello" ]; then
    echo "当前目录不在 dummy 或 hello 文件夹下，退出脚本"
    exit 1  # 退出脚本，可根据需要使用不同的退出状态码
fi

if [ "$1" = "x86" ]; then
    # 执行针对 x86 架构的命令
    make ISA=x86 -j8 && cp "build/${current_directory}-x86" $ramdisk_path
    # 进入 nanos-lite 目录，编译并运行
    cd ~/ics2020/nanos-lite/
    make ARCH=x86-nemu run
elif [ "$1" = "native" ]; then
    # 执行针对 native 架构的命令
    make ISA=am_native -j8 && cp "build/${current_directory}-am_native" $ramdisk_path
    # 进入 nanos-lite 目录，编译并运行
    cd ~/ics2020/nanos-lite/
    make ARCH=native run
else
    echo "Usage: ./run.sh [x86|native]"
fi
