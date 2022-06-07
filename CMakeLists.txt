cmake_minimum_required(VERSION 3.10)
project(cpk VERSION 1.0)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(cpk cpk.cpp)
target_link_libraries(cpk PUBLIC curl)
