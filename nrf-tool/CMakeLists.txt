cmake_minimum_required (VERSION 3.21)
project(nrf-tool)

add_executable(nrf-tool "nrf-tool.cpp" "dist/jsoncpp.cpp")
include_directories(CMAKE_SOURCE_DIR "include/json" "dist/json")
add_subdirectory(include)
