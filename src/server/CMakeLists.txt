aux_source_directory(. SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/thirdparty/db DB_SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/thirdparty/redis REDIS_SRC)
aux_source_directory(../proxy PROXY_SRC)
aux_source_directory(../pb/repo PB_SRC)
#生成制定的可执行文件
add_executable(ChatServer ${SRC_LIST} ${DB_SRC} ${MODEL_SRC} ${REDIS_SRC} ${PROXY_SRC} ${PB_SRC})
target_link_libraries(ChatServer muduo_net muduo_base mysqlclient hiredis pthread protobuf mprpc zookeeper_mt)