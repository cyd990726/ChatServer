aux_source_directory(. SRC_LIST)

#生成制定的可执行文件
add_executable(ChatClient ${SRC_LIST})
target_link_libraries(ChatClient muduo_net muduo_base pthread)
