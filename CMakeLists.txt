cmake_minimum_required(VERSION 3.10)
project(cpk VERSION 1.0)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#find_package(CURL REQUIRED)
add_subdirectory(external/curl/lib)
set(CURL_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/external/curl/include)
set(CURL_LIBRARIES libcurl)

message(STATUS "${CURL_INCLUDE_DIR}")

set(CPK_SOURCES
    download.cpp
    cpk_structs.h
)

include_directories(${CURL_INCLUDE_DIR})
add_executable(cpk cpk.cpp ${CPK_SOURCES})
target_link_libraries(cpk ${CURL_LIBRARIES})
