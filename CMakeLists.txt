cmake_minimum_required(VERSION 3.10)
project(cpk VERSION 1.0)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(CURL REQUIRED)

set(CPK_SOURCES
    download.cpp
    cpk_structs.h
)

add_executable(cpk cpk.cpp ${CPK_SOURCES})
include_directories(${CURL_INCLUDE_DIR})
target_link_libraries(cpk ${CURL_LIBRARIES})
