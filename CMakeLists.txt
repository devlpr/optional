cmake_minimum_required(VERSION 2.6)
project(optional)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -std=c++0x -ggdb")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -std=c++0x")

include_directories(${PROJECT_SOURCE_DIR})

add_executable(optionaltest optionaltest.cpp)
