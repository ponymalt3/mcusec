cmake_minimum_required(VERSION 3.10)

project(Test)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(Test test.cpp sha-fast.cpp)

add_custom_target(
    genHeaderTarget
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/keygen/keygen.py -o AllKeys.h test1 test2 key1 key2
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/keygen/keygen.py -i AllKeys.h -o ClientKey1.h key1 test1 
)

add_dependencies(Test genHeaderTarget)
