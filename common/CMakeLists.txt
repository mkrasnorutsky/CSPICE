cmake_minimum_required(VERSION 3.22.1)

# Shared NAIF CSPICE + AstrologerXP asteroid wrapper sources.
# Used by the Android NDK build; iOS continues to compile the same files via Xcode.

set(CSPICE_REPO_ROOT ${CMAKE_CURRENT_LIST_DIR}/..)
set(CSPICE_NAIF_DIR ${CSPICE_REPO_ROOT}/CSPICE/CSPICE/src/cspice)

file(GLOB CSPICE_NAIF_SOURCES ${CSPICE_NAIF_DIR}/*.c)

add_library(cspice_core STATIC
    src/Asteroids.cpp
    src/Directory.cpp
    src/SPKFileInfo.cpp
    ${CSPICE_NAIF_SOURCES}
)

target_include_directories(cspice_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/CSPICE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CSPICE_NAIF_DIR}
)

target_compile_definitions(cspice_core
    PUBLIC
        CSPICE_SKIP_PLATFORM_CHECK
)

target_compile_features(cspice_core PUBLIC cxx_std_17)
set_target_properties(cspice_core PROPERTIES POSITION_INDEPENDENT_CODE ON)

if(ANDROID)
    # NDK CMake does not map C_STANDARD 89; apply C-only flags explicitly.
    target_compile_options(cspice_core PRIVATE
        "$<$<COMPILE_LANGUAGE:C>:-std=gnu89>"
        "$<$<COMPILE_LANGUAGE:C>:-Wno-unused-parameter>"
        "$<$<COMPILE_LANGUAGE:C>:-Wno-sign-compare>"
    )
else()
    set_target_properties(cspice_core PROPERTIES
        C_STANDARD 89
        C_STANDARD_REQUIRED ON
    )
endif()
