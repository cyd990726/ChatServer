cmake_minimum_required(VERSION 3.0)
project(CHAT_PROJECT)

#配置编译选项
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})
#配置头文件的搜索路径
include_directories(/usr/include)
include_directories(/usr/local/include)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/mrpc)
include_directories(${PROJECT_SOURCE_DIR}/include/server)
include_directories(${PROJECT_SOURCE_DIR}/include/service)
include_directories(${PROJECT_SOURCE_DIR}/include/service/chat)
include_directories(${PROJECT_SOURCE_DIR}/include/service/repo)
include_directories(${PROJECT_SOURCE_DIR}/include/thirdparty)
include_directories(${PROJECT_SOURCE_DIR}/include/thirdparty/db)
include_directories(${PROJECT_SOURCE_DIR}/include/thirdparty/redis)
include_directories(${PROJECT_SOURCE_DIR}/include/public)
include_directories(${PROJECT_SOURCE_DIR}/include/proxy)
include_directories(${PROJECT_SOURCE_DIR}/include/pb)

# 设置项目库文件搜索路径
link_directories(${PROJECT_SOURCE_DIR}/lib)

#配置可执行文件的输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#加载子目录
add_subdirectory(src)

