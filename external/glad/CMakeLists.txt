cmake_minimum_required(VERSION 3.15.0 FATAL_ERROR)
project(glad CXX)

include(FetchContent)

FetchContent_Declare(
    glad
    URL "${CMAKE_CURRENT_SOURCE_DIR}/glad.zip"
)
FetchContent_MakeAvailable(glad)

set(header_files
    "${glad_SOURCE_DIR}/include/glad/glad.h"
    "${glad_SOURCE_DIR}/include/KHR/khrplatform.h"
)
source_group("Header Files" FILES ${header_files})

set(source_files
    "${glad_SOURCE_DIR}/src/glad.c"
)
source_group("Source Files" FILES ${source_files})

set(all_code_files
    ${header_files}
    ${source_files}
)

set_source_files_properties(${all_code_files} PROPERTIES LANGUAGE CXX)
add_library(glad STATIC ${all_code_files})
target_include_directories(glad SYSTEM PUBLIC
    "$<BUILD_INTERFACE:${glad_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
set_target_properties(glad PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${glad_BINARY_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${glad_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${glad_BINARY_DIR}"
)
