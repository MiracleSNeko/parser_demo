cmake_minimum_required(VERSION 3.0)

project(parser_demo)

# FLAGS
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -std=c++20")
set(CMAKE_CXX_FLAGS_RELEASE "-W -Wall -std=c++20 -o2")

# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# SOURCE FILE
file(
    GLOB_RECURSE SRC_DIR_LIST
    "test/*.cpp"
    "test/*.c"
    "src/fmt/test/*.cpp"
)

message(STATUS "source files as follows...")

foreach(source ${SRC_DIR_LIST})
    message(STATUS "\t${source}")
endforeach()

add_executable(

    # 编译 .a 静态库
    # ${PROJECT_NAME} STATIC
    # 编译 .so 动态库
    test_${PROJECT_NAME} ${SRC_DIR_LIST}
)

target_link_libraries(
    test_${PROJECT_NAME} gtest pthread
)

# INCLUDE FILE
macro(find_include_dir result curdir)
    file(
        GLOB_RECURSE children
        "${curdir}/*.hpp"
        "${curdir}/*.h"
    )
    message(STATUS "header files as follows...")
    set(dirlist "")

    foreach(child ${children})
        message(STATUS "\t${child}")
        string(REGEX REPLACE "(.*)/.*" "\\1" LIB_NAME ${child})

        if(IS_DIRECTORY ${LIB_NAME})
            list(APPEND dirlist ${LIB_NAME})
        endif()
    endforeach()

    set(${result} ${dirlist})
endmacro()

# 添加 .h/.hpp 文件目录，每个目录建议写一个变量
find_include_dir(SELF_INCLUDE_DIR_LIST "inc")
include_directories(
    ${SELF_INCLUDE_DIR_LIST}
)