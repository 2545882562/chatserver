cmake_minimum_required(VERSION 3.0)#最低版本Cmake
project(chat)#工程名称

#编译选项
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g) 

#指定搜索的子目录
#add_subdirectory(testmuduo)

#设置可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#设置头文件搜索路径
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/server)
include_directories(${PROJECT_SOURCE_DIR}/include/server/db)
include_directories(${PROJECT_SOURCE_DIR}/include/server/model)
include_directories(${PROJECT_SOURCE_DIR}/include/server/redis)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty)

#加载子目录
add_subdirectory(src)