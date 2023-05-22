# 这个是cmake最小版本要求
cmake_minimum_required(VERSION 3.1)

# Enable C++11，这一段是源码里的
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# 填写你的项目名称Project
project( RPC )

aux_source_directory(${CMAKE_SOURCE_DIR}/src/ DIR_SRCS)
aux_source_directory(${CMAKE_SOURCE_DIR}/example DIR_SRCS)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

message("${CMAKE_CURRENT_SOURCE_DIR}")

# Find all *.cpp files and store in list cpps
# GLOB这个参数不支持子目录， 用GLOB_RECURSE可以支持子目录。
file(GLOB cpps RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/example" "${CMAKE_CURRENT_SOURCE_DIR}/example/*.cpp")

foreach(mainfile IN LISTS cpps)
    # Get file name without directory
    get_filename_component(mainname ${mainfile} NAME_WE)
    add_executable(${mainname} ${CMAKE_CURRENT_SOURCE_DIR}/example/${mainfile})
    target_link_libraries(${mainname} zmq)
endforeach()
