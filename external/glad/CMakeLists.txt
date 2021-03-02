cmake_minimum_required(VERSION 3.15.0 FATAL_ERROR)
project(glad CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    glad
    URL "${CMAKE_CURRENT_SOURCE_DIR}/glad.zip"
    TMP_DIR ${CMAKE_SOURCE_DIR}/build/external/tmp
    STAMP_DIR ${CMAKE_SOURCE_DIR}/build/external/src/glad-stamp
    LOG_DIR ${CMAKE_SOURCE_DIR}/build/external/src/glad-stamp
    DOWNLOAD_DIR ${CMAKE_SOURCE_DIR}/build/external/src
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/build/external/src/glad
    BINARY_DIR ${CMAKE_SOURCE_DIR}/build/external/src/glad-build
)
FetchContent_MakeAvailable(glad)

set(glad_extract_dir "${CMAKE_SOURCE_DIR}/build/external/src/glad")

set(header_files
    "${glad_extract_dir}/include/glad/glad.h"
    "${glad_extract_dir}/include/KHR/khrplatform.h"
)
source_group("Header Files" FILES ${header_files})

set(source_files
    "${glad_extract_dir}/src/glad.c"
)
source_group("Source Files" FILES ${source_files})

set(all_code_files
    ${header_files}
    ${source_files}
)

set_source_files_properties(${source_files} PROPERTIES LANGUAGE CXX)
add_library(glad STATIC ${all_code_files})
target_include_directories(glad SYSTEM PUBLIC
    "$<BUILD_INTERFACE:${glad_extract_dir}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
set_target_properties(glad PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/external/src/glad-build"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/external/src/glad-build"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/external/src/glad-build"
)
