cmake_minimum_required(VERSION 3.6.2)

project(VMAGC)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_compile_options(-g)

enable_testing()

FILE(GLOB SOURCE src/*.cpp)

add_library(VMAGC ${SOURCE})

target_include_directories(VMAGC PUBLIC src)

add_subdirectory(test)
add_subdirectory(main)
