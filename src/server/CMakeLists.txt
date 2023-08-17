aux_source_directory(. SRC_LIST)
aux_source_directory(./db DB_SRC)

#生成制定的可执行文件
add_executable(ChatServer ${SRC_LIST} ${DB_SRC})
target_link_libraries(ChatServer muduo_net muduo_base mysqlclient pthread)