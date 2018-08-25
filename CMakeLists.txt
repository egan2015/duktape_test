cmake_minimum_required(VERSION 2.8)

set(CMAKE_VERBOSE_MAKEFILE ON)

project(duk_jse)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_C_FLAGS -std=c99)
set(CMAKE_CXX_FLAGS "-std=c++11")

set(LIST_SRC main.cpp
    duktape.c)

add_executable(${PROJECT_NAME} ${LIST_SRC})
