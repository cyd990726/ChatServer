aux_source_directory(. SRC_LIST)
aux_source_directory(./db DB_SRC)
aux_source_directory(./model MODEL_SRC)
aux_source_directory(./redis REDIS_SRC)

#生成制定的可执行文件
add_executable(ChatServer ${SRC_LIST} ${DB_SRC} ${MODEL_SRC} ${REDIS_SRC})
target_link_libraries(ChatServer muduo_net muduo_base mysqlclient hiredis pthread)