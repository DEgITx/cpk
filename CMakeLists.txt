cmake_minimum_required(VERSION 3.10)
project(cpk VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DCPK_DEBUG)
else()
    add_definitions(-DCPK_RELEASE)
endif()

if(WIN32)
    add_definitions(-DCPK_OS_WIN)
else()
    if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        add_definitions(-DCPK_OS_MACOS)
    else()
        add_definitions(-DCPK_OS_LINUX)
    endif()
endif()

set(WITH_TESTS True)
if(WITH_TESTS)
    enable_testing()
    add_definitions(-DWITH_TESTS)
    if(WITH_TESTS_CDASH)
        include(CTest)
    endif(WITH_TESTS_CDASH)
endif(WITH_TESTS)
add_definitions(-DZIP_STATIC)

#find_package(CURL REQUIRED)
add_subdirectory(external/mbedtls/library)
add_subdirectory(external/curl/lib)
add_subdirectory(external/zlib)
add_subdirectory(external/libzip/lib)

set(CURL_INCLUDE_DIR 
    ${CMAKE_SOURCE_DIR}/external/curl/include
    ${CMAKE_SOURCE_DIR}/external/libzip/lib
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}
)
set(CURL_LIBRARIES 
    libcurl
    zip
)

message(STATUS "${CURL_INCLUDE_DIR}")

FILE(GLOB CPK_CPP_FLES src/*.cpp)
list(FILTER CPK_CPP_FLES EXCLUDE REGEX ".*_test.cpp$")
FILE(GLOB CPK_H_FLES src/*.h)

set(CPK_SOURCES
    ${CPK_CPP_FLES}
    ${CPK_H_FLES}
)

include_directories(
    ${CURL_INCLUDE_DIR} 
    ${CMAKE_SOURCE_DIR}/external 
    ${CMAKE_SOURCE_DIR}/external/nlohmann
)
add_library(cpkl STATIC ${CPK_SOURCES})
target_link_libraries(cpkl ${CURL_LIBRARIES} pthread)
add_executable(cpk main.cpp)
target_link_libraries(cpk cpkl)
if(WIN32)
    install(TARGETS cpk DESTINATION .)
else()
    install(TARGETS cpk DESTINATION usr/bin)
endif()

FILE(GLOB TEST_FILES src/*_test.cpp)
foreach(TEST_FILE ${TEST_FILES})
    string(REGEX REPLACE ".+\/src\/(.+)(_test\\.cpp)" "\\1" TEST_NAME ${TEST_FILE})
    add_executable(test_${TEST_NAME} ${TEST_FILE})
    target_link_libraries(test_${TEST_NAME} cpkl)
    set_property(TARGET test_${TEST_NAME} PROPERTY CXX_STANDARD 14)
	set_property(TARGET test_${TEST_NAME} PROPERTY FOLDER "Tests")
    ADD_TEST(test_${TEST_NAME} test_${TEST_NAME})
endforeach()
