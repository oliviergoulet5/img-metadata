cmake_minimum_required(VERSION 3.31)
project(metaimg LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# Pull Google Test
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)
FetchContent_MakeAvailable(googletest)

add_library(libmetaimg src/file.cpp src/image.cpp)
target_include_directories(libmetaimg PUBLIC src/)

add_executable(metaimg src/main.cpp)
target_link_libraries(metaimg PRIVATE libmetaimg)

enable_testing()
add_executable(tests tests/test_image.cpp)
target_link_libraries(tests GTest::gtest_main libmetaimg)
include(GoogleTest)
gtest_discover_tests(tests WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
