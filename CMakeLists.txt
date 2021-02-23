cmake_minimum_required(VERSION 3.15.0 FATAL_ERROR)

project(rtek CXX)
set(PROJECT_NAME rtek)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# CYGWIN and MSYS require compile specific extensions to behave as expected.
# => -std=c++17 -> -std=gnu++17, for example
if(CMAKE_SYSTEM_NAME MATCHES "CYGWIN" OR CMAKE_SYSTEM_NAME MATCHES "MSYS")
    set(CMAKE_CXX_EXTENSIONS ON)
endif()

# TODO: check for WIN32 or MSVC?
# - WIN32 is true for a windows target
# - MSVC is true for MSVC or compilers emulating the msvc compiler syntax
#   (not necessarily gen-ing VS solution)
# - CMAKE_GENERATOR will contain substring "Visual Studio" if gen-ing VS solution

################################################################################
# Global configuration types
################################################################################
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

################################################################################
# Sub-projects
################################################################################
# NOTE: Must be an absolute path
set(install_prefix ${CMAKE_SOURCE_DIR}/build/external)

# Add external projects directory to the prefix search path (for find_* functions)
list(APPEND CMAKE_PREFIX_PATH "${install_prefix}")

include(FetchContent)

FetchContent_Declare(
	fmt
	GIT_REPOSITORY "https://github.com/fmtlib/fmt"
	GIT_TAG "7.1.3"
	GIT_SHALLOW 1
	GIT_PROGRESS 1
	UPDATE_COMMAND ""
	PATCH_COMMAND ""
	TEST_COMMAND ""
	TMP_DIR ${install_prefix}/tmp
	STAMP_DIR ${install_prefix}/src/fmt-stamp
	LOG_DIR ${install_prefix}/src/fmt-stamp
	DOWNLOAD_DIR ${install_prefix}/src
	SOURCE_DIR ${install_prefix}/src/fmt
	BINARY_DIR ${install_prefix}/src/fmt-build
	INSTALL_DIR ${install_prefix}
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
	TMP_DIR ${install_prefix}/tmp
	STAMP_DIR ${install_prefix}/src/spdlog-stamp
	LOG_DIR ${install_prefix}/src/spdlog-stamp
	DOWNLOAD_DIR ${install_prefix}/src
	SOURCE_DIR ${install_prefix}/src/spdlog
	BINARY_DIR ${install_prefix}/src/spdlog-build
	INSTALL_DIR ${install_prefix}
)

set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "spdlog build example")
set(SPDLOG_BUILD_BENCH OFF CACHE BOOL "spdlog build benchmarks")
set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "spdlog build tests")
set(SPDLOG_INSTALL OFF CACHE BOOL "spdlog generate install target")

# TODO:
#set(SPDLOG_BUILD_WARNINGS ON CACHE BOOL "Enable spdlog compiler warnings")
#set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "spdlog Use external fmt library instead of bundled")
#set(SPDLOG_NO_EXCEPTIONS ON CACHE BOOL "spdlog Compile with -fno-exceptions. Call abort() on any spdlog exceptions") # TODO: use at own peril!
FetchContent_MakeAvailable(spdlog)
set_target_properties(spdlog PROPERTIES
	MAP_IMPORTED_CONFIG_DEVELOP RelWithDebInfo
)

################################################################################
# Project Files
################################################################################
set(no_group_source_files
    ".clang-format"
    ".clang-tidy"
)
source_group("" FILES ${no_group_source_files})

set(Header_Files
    "rtek.h"
    "src/core/core.h"
    "src/core/ecs/components/component.h"
    "src/core/ecs/components/movement_component.h"
    "src/core/ecs/components/transform_component.h"
    "src/core/ecs/entity.h"
    "src/core/ecs/entity_manager.h"
    "src/core/ecs/systems/movement_system.h"
    "src/core/ecs/systems/system.h"
    "src/core/logging/logging.h"
    "src/core/math/vector.h"
    "src/core/platform/platform.h"
    "src/core/rkmisc.h"
    "src/core/types.h"
    "src/core/utility/assert.h"
    "src/core/utility/status.h"
    "src/core/utility/time.h"
    "src/core/utility/version.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    "src/core/core.cpp"
    "src/core/ecs/entity_manager.cpp"
    "src/core/logging/logging.cpp"
    "src/core/math/vector.cpp"
    "src/core/platform/platform.cpp"
    "src/core/utility/assert.cpp"
    "src/test/main_test.cpp"
)
source_group("Source Files" FILES ${Source_Files})

set(ALL_CODE_FILES
    ${Header_Files}
    ${Source_Files}
)

################################################################################
# rtek target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_CODE_FILES})
add_dependencies(${PROJECT_NAME} spdlog fmt)

target_include_directories(${PROJECT_NAME} PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/src"
	"${install_prefix}/include"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
	spdlog::spdlog
	fmt::fmt
)

set_target_properties(${PROJECT_NAME} PROPERTIES
	DEBUG_POSTFIX d
)

################################################################################
# Compile and link options
################################################################################
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
		"/permissive- " # use c++ standards conforming compiler behaviour
		"/sdl " 		# enable Security Development Lifecycle checks
		"/W3 " 			# warning level
		"/WX " 			# linker warnings as errors
		"/TP " 			# treat all files on command line as C++ source files
		"/Zi " 			# separate PDB file will all symbolic debug info
		# TODO: set the exception unwinding feature for now
		"/EHsc " 		# standard C++ stack unwinding + assumes extern C functions don't throw
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
	)

	string(CONCAT exe_linker_flags
		"/DEBUG:FULL " # generate full debug info in single PDB
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

	set_target_properties("${PROJECT_NAME}" PROPERTIES
		MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
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
)

################################################################################
# Visual Studio configuration
################################################################################
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