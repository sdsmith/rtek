cmake_minimum_required(VERSION 3.15.0 FATAL_ERROR)

# TODO: setup for cross compilation
#set(CMAKE_SYSTEM_VERSION 10.0.18362.0 CACHE STRING "" FORCE)

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

################################################################################
# Set target arch type if empty. Visual studio solution generator provides it.
################################################################################
# TODO: generalize
if(NOT CMAKE_VS_PLATFORM_NAME)
    set(CMAKE_VS_PLATFORM_NAME "x64")
endif()
message("${CMAKE_VS_PLATFORM_NAME} architecture in use")

if(NOT ("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32"
     OR "${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64"))
    message(FATAL_ERROR "${CMAKE_VS_PLATFORM_NAME} arch is not supported!")
endif()

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
# Use solution folders feature
################################################################################
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################################################################
# Sub-projects
################################################################################
# NOTE: Must be an absolute path
set(install_prefix ${CMAKE_SOURCE_DIR}/build/external)

# Add external projects directory to the prefix search path (for find_* functions)
list(APPEND CMAKE_PREFIX_PATH "${install_prefix}")

include(FetchContent)

# Used for dependencies that are not worked on in this project and can be 
# installed in a specific config once head of time for all builds.
#
# Usage:
# ```
# set(spdlog_cmake_args
# 	-DSPDLOG_BUILD_EXAMPLE=OFF
# 	-DSPDLOG_BUILD_BENCH=OFF
# 	-DSPDLOG_BUILD_TESTS=OFF
# 	-DSPDLOG_INSTALL=ON
# )
# install_at_config_time(spdlog 
# 	"https://github.com/gabime/spdlog" 
# 	"v1.8.2" 
# 	"Release"
# 	"${spd_log_cmake_args}")
# find_package(spdlog REQUIRED)
# ```
function(install_at_config_time dep_name git_repo git_tag build_config cmake_args)
	set(dep_source_dir ${install_prefix}/src/${dep_name})
	FetchContent_Declare(
		${dep_name}
		GIT_REPOSITORY "${git_repo}"
		GIT_TAG "${git_tag}"
		GIT_SHALLOW 1
		GIT_PROGRESS 1
		UPDATE_COMMAND ""
		PATCH_COMMAND ""
		TEST_COMMAND ""
		DOWNLOAD_DIR ${install_prefix}/src
		SOURCE_DIR ${dep_source_dir}
		BINARY_DIR ${dep_source_dir}-build
		INSTALL_DIR ${install_prefix}
	)
	FetchContent_GetProperties(${dep_name})
	if (NOT ${dep_name}_POPULATED)
		message(STATUS "Populating ${dep_name}...")
		FetchContent_Populate(${dep_name})
		
		set(dep_config_logfile "${${dep_name}_BINARY_DIR}/cmake_configure_output.log")
		set(dep_build_logfile "${${dep_name}_BINARY_DIR}/cmake_build_output.log")
		
		message(STATUS "Configuring ${dep_name}...")
		execute_process(
			COMMAND ${CMAKE_COMMAND}
				-S ${${dep_name}_SOURCE_DIR}
				-B ${${dep_name}_BINARY_DIR}
				-DCMAKE_INSTALL_PREFIX=${install_prefix}
				${_cmake_args}
			WORKING_DIRECTORY ${${dep_name}_BINARY_DIR}
			COMMAND_ECHO STDOUT
			ECHO_OUTPUT_VARIABLE
			ECHO_ERROR_VARIABLE
			OUTPUT_FILE ${dep_config_logfile}
			ERROR_FILE  ${dep_config_logfile}
			RESULT_VARIABLE config_result
		)
		if (config_result)
			message(FATAL_ERROR "Failed to configure ${dep_name}. See ${dep_config_logfile}")
		endif()
		
		# TODO: make build type use configurable
		set(${dep_name}_CONFIG_TYPE ${build_config} CACHE INTERNAL "Config/build type for ${dep_name}")
		message(STATUS "Installing ${dep_name} in ${${dep_name}_CONFIG_TYPE} mode")
		execute_process(
			COMMAND ${CMAKE_COMMAND}
				--build ${${dep_name}_BINARY_DIR}
				--config ${${dep_name}_CONFIG_TYPE}
				--target install
			WORKING_DIRECTORY ${${dep_name}_BINARY_DIR}
			COMMAND_ECHO STDOUT
			ECHO_OUTPUT_VARIABLE
			ECHO_ERROR_VARIABLE
			OUTPUT_FILE ${dep_build_logfile}
			ERROR_FILE ${dep_build_logfile}
			RESULT_VARIABLE build_result
		)
		if (build_result)
			message(FATAL_ERROR "Failed to build ${dep_name}. See ${dep_build_logfile}")
		endif()
		
		# NOTE: Can find_package now
	endif()
endfunction()

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

#set(SPDLOG_BUILD_WARNINGS ON CACHE BOOL "Enable spdlog compiler warnings")
#set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "spdlog Use external fmt library instead of bundled")
#set(SPDLOG_NO_EXCEPTIONS ON CACHE BOOL "spdlog Compile with -fno-exceptions. Call abort() on any spdlog exceptions") # TODO: use at own peril!
FetchContent_MakeAvailable(spdlog)
set_target_properties(spdlog PROPERTIES
	MAP_IMPORTED_CONFIG_DEVELOP RelWithDebInfo
)

################################################################################
# Source groups
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
# Target
################################################################################

add_executable(${PROJECT_NAME} ${ALL_CODE_FILES})

#use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}") # TODO: may have already replaced this
set(ROOT_NAMESPACE rtek)

set_target_properties(${PROJECT_NAME} PROPERTIES 
	DEBUG_POSTFIX d
)

# Set rtek as the startup project in Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

set_target_properties("${PROJECT_NAME}" PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")


################################################################################
# Output directory
################################################################################
# TODO: OUTPUT_DIRECTORY_* properties seem to need a prefix. are they not given one here?
set_target_properties(${PROJECT_NAME} PROPERTIES
	OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/build/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
	OUTPUT_DIRECTORY_DEVELOP "${CMAKE_SOURCE_DIR}/build/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
	OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/build/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
)
set_target_properties(${PROJECT_NAME} PROPERTIES
	INTERPROCEDURAL_OPTIMIZATION_DEVELOP "TRUE"
	INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
)

################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/src"
	"${install_prefix}/include"
)

################################################################################
# Compile definitions
################################################################################
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
		"TARGET_ARCH_X86"
	">"
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

# TODO: non-msvc builds
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${cxx_flags}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${cxx_flags_debug}")
set(CMAKE_CXX_FLAGS_DEVELOP "${CMAKE_CXX_FLAGS_DEVELOP} ${cxx_flags_develop}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${cxx_flags_release}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${exe_linker_flags}")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${exe_linker_flags_debug}")
set(CMAKE_EXE_LINKER_FLAGS_DEVELOP "${CMAKE_EXE_LINKER_FLAGS_DEVELOP} ${exe_linker_flags_develop}")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} ${exe_linker_flags_release}")

################################################################################
# Dependencies
################################################################################
add_dependencies(${PROJECT_NAME} spdlog fmt)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog fmt::fmt)
