cmake_minimum_required(VERSION 3.15.0 FATAL_ERROR)
set(CMAKE_USER_MAKE_RULES_OVERRIDE "${CMAKE_CURRENT_LIST_DIR}/cmake/compiler_options.cmake")

project(rtek CXX)
set(PROJECT_NAME rtek)

message(STATUS "Build RTEK")

# TODO(sdsmith): check for WIN32 or MSVC?
# - WIN32 is true for a windows target
# - MSVC is true for MSVC or compilers emulating the msvc compiler syntax
#   (not necessarily gen-ing VS solution)
# - CMAKE_GENERATOR will contain substring "Visual Studio" if gen-ing VS solution

# ---------------------------------------------------------------------------------------
# Global configuration types
# ---------------------------------------------------------------------------------------
set(CMAKE_CONFIGURATION_TYPES
    "Debug"
    "Develop"
    "Release"
    CACHE STRING "Build configurations" FORCE
)

# Default build type
if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Develop" CACHE STRING "" FORCE)
endif()

if (NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

set(CMAKE_CXX_EXTENSIONS OFF)
# CYGWIN and MSYS require compile specific extensions to behave as expected.
# => -std=c++17 -> -std=gnu++17, for example
if(CMAKE_SYSTEM_NAME MATCHES "CYGWIN" OR CMAKE_SYSTEM_NAME MATCHES "MSYS")
    set(CMAKE_CXX_EXTENSIONS ON)
endif()

# ---------------------------------------------------------------------------------------
# Set RTEK_MASTER_PROJECT to ON if we are building rtek
# ---------------------------------------------------------------------------------------
# Check if rtek is being used directly or via add_subdirectory, but allow overriding
if (NOT DEFINED RTEK_MASTER_PROJECT)
    if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        set(RTEK_MASTER_PROJECT ON)
    else()
        set(RTEK_MASTER_PROJECT OFF)
    endif()
endif()

# ---------------------------------------------------------------------------------------
# User options
# ---------------------------------------------------------------------------------------
set(RK_LOG_LEVEL RK_LOG_LEVEL_INFO CACHE STRING "Log level compiled into the application. One of \
RK_LOG_LEVEL_{OFF, TRACE, DEBUG, INFO, WARN, ERROR, CRITICAL}. Levels listed in order of least to \
most precendence. Levels of equals or less precedence are removed.")
# Possible values displayed on GUI for RK_LOG_LEVEL
set_property(CACHE RK_LOG_LEVEL PROPERTY STRINGS
    RK_LOG_LEVEL_OFF
    RK_LOG_LEVEL_TRACE
    RK_LOG_LEVEL_DEBUG
    RK_LOG_LEVEL_INFO
    RK_LOG_LEVEL_WARN
    RK_LOG_LEVEL_ERROR
    RK_LOG_LEVEL_CRITICAL
)
option(RK_LOGGING_OFF "Remove all logging. Takes precedence over `RK_LOG_LEVEL`." OFF)
option(RK_LOGGING_PERF "Compile in minimal logging (for performance). Takes precedence over `RK_LOG_LEVEL`." OFF)
option(RK_LOG_FIXME "Print out fix me and the reason they should be fixed as they are encountered." OFF)

option(RK_OGL_DEBUG "Display debug messages from OpenGL, including errors." OFF)
set(RK_SHADER_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/data/shaders" CACHE STRING
    "Directory containing all the shaders. Prepended to the path of the shader being opened.")
option(RK_REQUEST_HIGH_PERF_RENDERER "Ask for a high performance renderer. For systems with an iGPU and dGPU, this typically means the dGPU." ON)

set(RK_DATA_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/data" CACHE STRING "Directory containing data for the engine to consume")

message(STATUS "Build options:")
message(STATUS "  Master project  : " ${RTEK_MASTER_PROJECT})
message(STATUS "  Build type      : " ${CMAKE_BUILD_TYPE})
message(STATUS "  C++ standard    : " ${CMAKE_CXX_STANDARD})
message(STATUS "General options:")
message(STATUS "  Data dir        : " ${RK_DATA_BASE_DIR})
message(STATUS "Rendering options:")
message(STATUS "  OpenGL debug ctx: " ${RK_OGL_DEBUG})
message(STATUS "  Shader base dir : " ${RK_SHADER_BASE_DIR})
message(STATUS "  High performance: " ${RK_REQUEST_HIGH_PERF_RENDERER})
message(STATUS "Logging options:")
message(STATUS "  RK_LOG_LEVEL    : " ${RK_LOG_LEVEL})
message(STATUS "  RK_LOGGING_OFF  : " ${RK_LOGGING_OFF})
message(STATUS "  RK_LOGGING_PERF : " ${RK_LOGGING_PERF})
message(STATUS "  RK_LOG_FIXME   : " ${RK_LOG_FIXME})

# ---------------------------------------------------------------------------------------
# Compile and link options
# ---------------------------------------------------------------------------------------

if (RTEK_MASTER_PROJECT)
    set(cxx_flags "")
    set(cxx_flags_debug "")
    set(cxx_flags_develop "")
    set(cxx_flags_release "")
    set(exe_linker_flags "")
    set(exe_linker_flags_debug "")
    set(exe_linker_flags_develop "")
    set(exe_linker_flags_release "")
    if(MSVC)
        # TODO: potentially set:
        # - CMAKE_MODULE_LINKER_FLAGS
        # - CMAKE_SHARED_LINKER_FLAGS
        # - CMAKE_STATIC_LINKER_FLAGS

        string(CONCAT cxx_flags
            "/DWIN32 "
            "/D_WINDOWS "
            "/permissive- " # use c++ standards conforming compiler behaviour
            "/sdl "                 # enable Security Development Lifecycle checks
            "/W3 "                  # warning level
            "/WX "                  # linker warnings as errors
            "/TP "                  # treat all files on command line as C++ source files
            "/Zi "                  # separate PDB file will all symbolic debug info
            # TODO: set the exception unwinding feature for now
            "/EHsc "                # standard C++ stack unwinding + assumes extern C functions don't throw
            # TODO: ban dynamic_cast and typeid and use /GR- to reduce code size
            "/GR "          # run-time type info. Needed for dynamic_cast or typeid
            )
        string(CONCAT cxx_flags_debug
            "/Od " # disable optimizations
            "/RTC1 " # runtime error check for value initialization
            )
        string(CONCAT cxx_flags_develop
            # TODO: /Od disables optimizations, making the edit-compile-run loop smaller, but could make the application slow.
            "/Od " # favor speed optimizations over size
            "/Oi " # generate intrinsic functions
            "/Gy " # function-level linking (COMDAT)
            "/RTC1 " # runtime error check for value initialization
            )
        string(CONCAT cxx_flags_release
            "/GL " # whole program optimization
            "/O2 " # maximize speed optimization
            "/Oi " # generate intrinsic functions
            "/Gy " # function-level linking
            "/DNDEBUG "
            )

        string(CONCAT exe_linker_flags
            "/DEBUG:FULL " # generate full debug info in single PDB
            "/SUBSYSTEM:WINDOWS " # Spawn as a window application, not a console app
            "/ENTRY:mainCRTStartup " # Use `main` instead of `WinMain`
            )
        string(CONCAT exe_linker_flags_debug
            "/INCREMENTAL " # incremental linking
            )
        string(CONCAT exe_linker_flags_develop
            "/INCREMENTAL " # incremental linking
            )
        string(CONCAT exe_linker_flags_release
            "/INCREMENTAL:NO " # no incremental linking
            "/OPT:REF " # eliminate functions and data the are never referenced
            "/OPT:ICF " # perform identical COMDAT folding
            )
    else()
        # TODO: non-msvc builds
        message(FATAL_ERROR "Compiler not supported")
    endif()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${cxx_flags}")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${cxx_flags_debug}")
    set(CMAKE_CXX_FLAGS_DEVELOP "${CMAKE_CXX_FLAGS_DEVELOP} ${cxx_flags_develop}")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${cxx_flags_release}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${exe_linker_flags}")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${exe_linker_flags_debug}")
    set(CMAKE_EXE_LINKER_FLAGS_DEVELOP "${CMAKE_EXE_LINKER_FLAGS_DEVELOP} ${exe_linker_flags_develop}")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} ${exe_linker_flags_release}")
endif()

message(STATUS "CMAKE_CXX_FLAGS               : " ${CMAKE_CXX_FLAGS})
message(STATUS "CMAKE_CXX_FLAGS_DEBUG         : " ${CMAKE_CXX_FLAGS_DEBUG})
message(STATUS "CMAKE_CXX_FLAGS_DEVELOP       : " ${CMAKE_CXX_FLAGS_DEVELOP})
message(STATUS "CMAKE_CXX_FLAGS_RELEASE       : " ${CMAKE_CXX_FLAGS_RELEASE})
message(STATUS "CMAKE_EXE_LINKER_FLAGS        : " ${CMAKE_EXE_LINKER_FLAGS})
message(STATUS "CMAKE_EXE_LINKER_FLAGS_DEBUG  : " ${CMAKE_EXE_LINKER_FLAGS_DEBUG})
message(STATUS "CMAKE_EXE_LINKER_FLAGS_DEVELOP: " ${CMAKE_EXE_LINKER_FLAGS_DEVELOP})
message(STATUS "CMAKE_EXE_LINKER_FLAGS_RELEASE: " ${CMAKE_EXE_LINKER_FLAGS_RELEASE})

# ---------------------------------------------------------------------------------------
# Sub-projects
# ---------------------------------------------------------------------------------------
# Set the install prefix
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/_deps" CACHE PATH "..." FORCE)
endif()
# Add install location to find_* search path
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_INSTALL_PREFIX}")

message(STATUS "Build subprojects...")
include(FetchContent)

# WAR(https://gitlab.kitware.com/cmake/cmake/-/issues/21889): Don't set the
# fetchcontent base directory outside of the build directory. It will lead to
# frequent downloads of these repos with the current cmake setup, but what am I
# to do other than change how dependencies are handled?
# set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/_deps" CACHE STRING "location prefix for downloaded/extracted content")

FetchContent_Declare(
    fmt
    GIT_REPOSITORY "https://github.com/fmtlib/fmt"
    GIT_TAG "7.1.3"
    GIT_SHALLOW 1
    GIT_PROGRESS 1
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    TEST_COMMAND ""
)

set(FMT_DOC OFF CACHE BOOL "fmt generate docs")
set(FMT_TEST OFF CACHE BOOL "fmt build tests")
set(FMT_INSTALL OFF CACHE BOOL "fmt generate install target")
FetchContent_MakeAvailable(fmt)
set_target_properties(fmt PROPERTIES
    MAP_IMPORTED_CONFIG_DEVELOP RelWithDebInfo
)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY "https://github.com/gabime/spdlog"
    GIT_TAG "v1.8.2"
    GIT_SHALLOW 1
    GIT_PROGRESS 1
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    TEST_COMMAND ""
)

set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "spdlog build example")
set(SPDLOG_BUILD_BENCH OFF CACHE BOOL "spdlog build benchmarks")
set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "spdlog build tests")
set(SPDLOG_INSTALL OFF CACHE BOOL "spdlog generate install target")
set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "spdlog Use external fmt library instead of bundled")
# TODO:
#set(SPDLOG_BUILD_WARNINGS ON CACHE BOOL "Enable spdlog compiler warnings")
#set(SPDLOG_NO_EXCEPTIONS ON CACHE BOOL "spdlog Compile with -fno-exceptions. Call abort() on any spdlog exceptions") # TODO: use at own peril!
FetchContent_MakeAvailable(spdlog)
add_dependencies(spdlog fmt)
set_target_properties(spdlog PROPERTIES
    MAP_IMPORTED_CONFIG_DEVELOP RelWithDebInfo
)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY "https://github.com/glfw/glfw.git"
    GIT_TAG "3.3.3"
    GIT_SHALLOW 1
    GIT_PROGRESS 1
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    TEST_COMMAND ""
)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "glfw build examples")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "glfw build tests")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "glfw build docs")
set(GLFW_INSTALL OFF CACHE BOOL "glfw generate install target")
FetchContent_MakeAvailable(glfw)
set_target_properties(glfw PROPERTIES
    MAP_IMPORTED_CONFIG_DEVELOP RelWithDebInfo
)

add_subdirectory(external/glad)

FetchContent_Declare(
    sdslib
    GIT_REPOSITORY "https://github.com/sdsmith/sdslib-cpp.git"
    GIT_TAG origin/master
    GIT_PROGRESS 1
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    TEST_COMMAND ""
)

set(SDSLIB_BUILD_TESTS OFF CACHE BOOL "sdslib build tests")
FetchContent_MakeAvailable(sdslib)
set_target_properties(sdslib PROPERTIES
    MAP_IMPORTED_CONFIG_DEVELOP Debug
)

# ---------------------------------------------------------------------------------------
# Project Files
# ---------------------------------------------------------------------------------------
set(no_group_source_files
    ".clang-format"
    ".clang-tidy"
)
source_group("" FILES ${no_group_source_files})

set(header_files
    "external/stb/include/stb/stb_image.h"
    "src/core/assert.h"
    "src/core/core.h"
    "src/core/ecs/components/component.h"
    "src/core/ecs/components/movement_component.h"
    "src/core/ecs/components/transform_component.h"
    "src/core/ecs/entity.h"
    "src/core/ecs/entity_manager.h"
    "src/core/ecs/systems/movement_system.h"
    "src/core/ecs/systems/system.h"
    "src/core/hid/input.h"
    "src/core/logging/logging.h"
    "src/core/math/vector.h"
    "src/core/platform/glfw.h"
    "src/core/platform/input_manager.h"
    "src/core/platform/platform.h"
    "src/core/platform/window.h"
    "src/core/platform/window_manager.h"
    "src/core/renderer/opengl/shader_program.h"
    "src/core/renderer/request_high_perf_renderer.h"
    "src/core/renderer/renderer.h"
    "src/core/rkmisc.h"
    "src/core/status.h"
    "src/core/types.h"
    "src/core/utility/fixme.h"
    "src/core/utility/no_exception.h"
    "src/core/utility/time.h"
    "src/core/version.h"
)
source_group("Header Files" FILES ${header_files})

set(source_files
    "src/core/assert.cpp"
    "src/core/core.cpp"
    "src/core/ecs/entity_manager.cpp"
    "src/core/logging/logging.cpp"
    "src/core/math/vector.cpp"
    "src/core/platform/glfw.cpp"
    "src/core/platform/input_manager.cpp"
    "src/core/platform/platform.cpp"
    "src/core/platform/window.cpp"
    "src/core/platform/window_manager.cpp"
    "src/core/renderer/opengl/shader_program.cpp"
    "src/core/renderer/renderer.cpp"
    "src/core/status.cpp"
    "src/core/utility/stb.cpp"
    "src/main.cpp"
)
source_group("Source Files" FILES ${source_files})

set(all_code_files
    ${header_files}
    ${source_files}
)

# ---------------------------------------------------------------------------------------
# rtek target
# ---------------------------------------------------------------------------------------
add_executable(${PROJECT_NAME} ${all_code_files})
target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/external/stb/include"
)

add_dependencies(${PROJECT_NAME} spdlog fmt glfw glad sdslib)
target_link_libraries(${PROJECT_NAME} PRIVATE
    spdlog::spdlog
    fmt::fmt
    glfw
    glad
    sdslib
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    DEBUG_POSTFIX d
)

# Build type configs
target_compile_definitions(${PROJECT_NAME} PRIVATE
    "$<$<CONFIG:Debug>:"
    "RK_ASSERTIONS_ENABLED"
    "RK_TARGET_ARCH_X86"
    ">"
    "$<$<CONFIG:Develop>:"
    "RK_ASSERTIONS_ENABLED"
    "RK_TARGET_ARCH_X86"
    ">"
    "$<$<CONFIG:Release>:"
    "RK_TARGET_ARCH_X86"
    ">"
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_DEBUG "FALSE"
    INTERPROCEDURAL_OPTIMIZATION_DEVELOP "TRUE"
    INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
)

# Exposed cmake option feature toggles
if (RK_LOG_LEVEL)
    target_compile_definitions(${PROJECT_NAME} PRIVATE RK_LOG_LEVEL=${RK_LOG_LEVEL})
endif()
if (RK_LOGGING_OFF)
    target_compile_definitions(${PROJECT_NAME} PRIVATE RK_LOGGING_OFF)
endif()
if (RK_LOGGING_PERF)
    target_compile_definitions(${PROJECT_NAME} PRIVATE RK_LOGGING_PERF)
endif()
if (RK_LOG_FIXME)
    target_compile_definitions(${PROJECT_NAME} PRIVATE RK_LOG_FIXME)
endif()
if (RK_OGL_DEBUG)
    target_compile_definitions(${PROJECT_NAME} PRIVATE RK_OGL_DEBUG)
endif()
if (RK_SHADER_BASE_DIR)
    target_compile_definitions(${PROJECT_NAME} PRIVATE RK_SHADER_BASE_DIR="${RK_SHADER_BASE_DIR}")
endif()
if (RK_REQUEST_HIGH_PERF_RENDERER)
    target_compile_definitions(${PROJECT_NAME} PRIVATE RK_REQUEST_HIGH_PERF_RENDERER)
endif()
if (RK_DATA_BASE_DIR)
    target_compile_definitions(${PROJECT_NAME} PRIVATE RK_DATA_BASE_DIR="${RK_DATA_BASE_DIR}")
endif()

# ---------------------------------------------------------------------------------------
# Visual Studio configuration
# ---------------------------------------------------------------------------------------
if (CMAKE_GENERATOR MATCHES "Visual Studio")
    # Set target arch type if empty. Visual studio solution generator provides it.
    if(NOT CMAKE_VS_PLATFORM_NAME)
        set(CMAKE_VS_PLATFORM_NAME "x64")
    endif()
    message("${CMAKE_VS_PLATFORM_NAME} architecture in use")

    if(NOT ("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32"
                OR "${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64"))
        message(FATAL_ERROR "${CMAKE_VS_PLATFORM_NAME} arch is not supported!")
    endif()

    # Use solution folders
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    # Set rtek as the startup project in Visual Studio
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
endif()
