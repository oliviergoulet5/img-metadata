cmake_minimum_required(VERSION 3.31)
project(metaimg LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(metaimg src/main.cpp)
