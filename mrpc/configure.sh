#!/bin/bash

set -e

# 清空 build 目录
rm -rf $(pwd)/build/*
# 进入 build 目录并构建项目
cd $(pwd)/build &&
    cmake .. &&
    make
# 返回项目根目录
cd ..

# 复制生成的库文件到 lib 目录
# 如果 lib 目录不存在则创建
if [ ! -d "$(pwd)/../lib" ]; then
    mkdir -p $(pwd)/../lib
fi

# 复制生成的库文件到 lib 目录
cp $(pwd)/lib/* $(pwd)/../lib

# 复制头文件到 include 目录
# 如果 include 目录不存在则创建
if [ ! -d "$(pwd)/../include" ]; then
    mkdir -p $(pwd)/../include
fi
cp -r $(pwd)/include $(pwd)/../include/mrpc

