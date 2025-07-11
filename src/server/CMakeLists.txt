#把.指定路径（当前路径）下的所有源文件名字放入SRC_LIST中
aux_source_directory(. SRC_LIST)
aux_source_directory(./db DB_LIST)
aux_source_directory(./model MODEL_LIST)
aux_source_directory(./redis REDIS_LIST)

#生成执行文件sercer，由SRC_LIST变量所定义的源文件编译而来
add_executable(ChatServer ${SRC_LIST} ${DB_LIST} ${MODEL_LIST} ${REDIS_LIST})

#表示server这个程序需要链接哪些库
target_link_libraries(ChatServer muduo_net muduo_base mysqlclient hiredis pthread)