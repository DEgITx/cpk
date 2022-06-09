cmake_minimum_required(VERSION 3.10)
project(cpk VERSION 1.0)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_definitions(-DZIP_STATIC)

#find_package(CURL REQUIRED)
add_subdirectory(external/mbedtls/library)
add_subdirectory(external/curl/lib)
add_subdirectory(external/zlib)
add_subdirectory(external/libzip/lib)

set(CURL_INCLUDE_DIR 
    ${CMAKE_SOURCE_DIR}/external/curl/include
    ${CMAKE_SOURCE_DIR}/external/libzip/lib
    ${CMAKE_BINARY_DIR}
)
set(CURL_LIBRARIES 
    libcurl
    zip
)

message(STATUS "${CURL_INCLUDE_DIR}")

FILE(GLOB CPK_CPP_FLES src/*.cpp)
FILE(GLOB CPK_H_FLES src/*.h)

set(CPK_SOURCES
    ${CPK_CPP_FLES}
    ${CPK_H_FLES}
)

include_directories(${CURL_INCLUDE_DIR})
add_executable(cpk ${CPK_SOURCES})
target_link_libraries(cpk ${CURL_LIBRARIES})
