# cmake minimum version
cmake_minimum_required(VERSION 3.11)

# project name
project(project1)

# c++ setting
# for ubuntu
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.1 -march=native")

# build mode
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE Release)
endif()

# seal
# for ubuntu
SET(SEAL_DIR "/home/ubuntu/project1/SEAL/build/cmake")
find_package(SEAL REQUIRED)
if(SEAL_FOUND)
    message(STATUS "Using SEAL ${SEAL_VERSION}")
endif()

# openssl
# for ubuntu
find_package(PkgConfig REQUIRED)
pkg_search_module(OPENSSL REQUIRED openssl)
if(OPENSSL_FOUND)
   message(STATUS "Using OPENSSL ${OPENSSL_VERSION}")
endif()

# include .h file = g++ -I
include_directories("${PROJECT_SOURCE_DIR}/src")

# link directories of library = g++ -L
link_directories("${PROJECT_SOURCE_DIR}/src")

# compile source code generation target
aux_source_directory(${PROJECT_SOURCE_DIR}/src/encode source_encode)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/key_binding_and_retrieval key_binding_and_retrieval)

add_executable(test_server src/test/test_server.cc ${source_encode} ${key_binding_and_retrieval})
add_executable(test_client src/test/test_client.cc ${source_encode} ${key_binding_and_retrieval})

# target library
target_link_libraries(test_server ${OPENSSL_LIBRARIES} libemp-tool.so SEAL::seal)
target_link_libraries(test_client ${OPENSSL_LIBRARIES} libemp-tool.so SEAL::seal)