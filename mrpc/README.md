# MRPC远程服务网络通信框架
## 项目介绍
这是一个基于C++的远程服务网络通信框架，支持跨平台，支持Linux和Windows。
支持的功能：
1. 支持服务端和客户端的注册和调用
2. 支持服务端和客户端的负载均衡
3. 支持服务端和客户端的故障转移
4. 支持服务端和客户端的高可用
5. 支持服务端和客户端的高并发
6. 支持服务端和客户端的高扩展性

## 需要安装的依赖
1. 安装protobuf
输入以下命令安装protobuf
```
sudo apt update
sudo apt install protobuf-compiler libprotobuf-dev
```
检查是否安装成功
```
protoc --version
```
2. 安装zookeeper开发库
```
sudo apt update && sudo apt install libzookeeper-mt-dev
```




## 安装
输入`sh configure.sh`一键编译安装。将include文件夹下的头文件和lib下的库文件复制到你的项目中去即可使用。