#!/bin/bash

set -e

# 清空 build 目录
rm -rf $(pwd)/build/*
# 进入 build 目录并构建项目
cd $(pwd)/build &&
    cmake .. &&
    make

