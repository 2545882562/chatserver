#配置头文件搜索路径
#include_directories()
#配置库文件搜索路径
#link_directories()

#设置需要编译的源文件列表
set(SRC_LIST testmuduo.cpp)

#设置可执行文件的存放位置
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#把.指定路径（当前路径）下的所有源文件名字放入SRC_LIST中
#aux_source_directory(. SRC_LIST)

#生成执行文件sercer，由SRC_LIST变量所定义的源文件编译而来
add_executable(server ${SRC_LIST})

#表示server这个程序需要链接哪些库
target_link_libraries(server muduo_net muduo_base pthread)