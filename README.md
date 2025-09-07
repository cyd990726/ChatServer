# ChatServer

#### 介绍

一个简单的分布式聊天服务器

#### 软件架构

此项目使用c++语言基于muduo网络库进行开发，数据库使用mysql，使用redis是消息队列，可通过nginx进行负载均衡，可部署多台形成服务器集群，从而大大提高并发量。
![alt text](image.png)

#### 安装教程

1. 所需环境：linux + cmake + g++，并且安装了boost和muduo网络库
2. 在项目根目录下创建build目录，cd build进入build目录，执行cmake ..,再执行make即可编译成功。
3. 切换到bin目录下。执行./ChatServer port启动服务器，或者执行。./ChatClient ip port启动测试客户端
