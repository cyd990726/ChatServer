aux_source_directory(. SRC_LIST)
aux_source_directory(${PROJECT_SOURCE_DIR}/thirdparty/db DB_SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/thirdparty/redis REDIS_SRC)
aux_source_directory(../../pb/repo PB_SRC)

#生成制定的可执行文件
add_executable(RepoService ${SRC_LIST} ${DB_SRC} ${REDIS_SRC} ${PB_SRC})
target_link_libraries(RepoService mprpc muduo_net muduo_base mysqlclient hiredis pthread protobuf zookeeper_mt)