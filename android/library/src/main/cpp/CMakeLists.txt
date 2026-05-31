cmake_minimum_required(VERSION 3.22.1)
project(cspice_jni C CXX)

set(CSPICE_COMMON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../common)

add_subdirectory(${CSPICE_COMMON_DIR} ${CMAKE_BINARY_DIR}/cspice_core)

add_library(cspice SHARED jni_bridge.cpp)

target_link_libraries(cspice PRIVATE cspice_core)

target_include_directories(cspice PRIVATE
    ${CSPICE_COMMON_DIR}/include
)

target_compile_features(cspice PRIVATE cxx_std_17)
