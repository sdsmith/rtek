cmake_minimum_required(VERSION 3.15.0 FATAL_ERROR)

# TODO: setup for cross compilation
#set(CMAKE_SYSTEM_VERSION 10.0.18362.0 CACHE STRING "" FORCE)

project(rtek CXX)

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

################################################################################
# Global compiler options
################################################################################
# TODO: check if all the overriding is necessary
if(MSVC)
    # remove default flags provided with CMake for MSVC
    set(CMAKE_CXX_FLAGS "")
    set(CMAKE_CXX_FLAGS_DEBUG "")
    set(CMAKE_CXX_FLAGS_DEVELOP "")
    set(CMAKE_CXX_FLAGS_RELEASE "")
endif()

################################################################################
# Global linker options
################################################################################
if(MSVC)
    # remove default flags provided with CMake for MSVC
    set(CMAKE_EXE_LINKER_FLAGS "")
    set(CMAKE_MODULE_LINKER_FLAGS "")
    set(CMAKE_SHARED_LINKER_FLAGS "")
    set(CMAKE_STATIC_LINKER_FLAGS "")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_DEBUG "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_DEVELOP "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_DEVELOP "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_DEVELOP "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_DEVELOP "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_RELEASE "${CMAKE_STATIC_LINKER_FLAGS}")
endif()

################################################################################
# Common utils
################################################################################
# TODO: investigate
include(CMake/Utils.cmake)

################################################################################
# Use solution folders feature
################################################################################
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################################################################
# Sub-projects
################################################################################
# NOTE: Must be an absolute path
set(INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/external2) # TODO: change

# Add external projects directory to the module search path
list(APPEND CMAKE_PREFIX_PATH "${INSTALL_PREFIX}")

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
	set(dep_source_dir ${INSTALL_PREFIX}/src/${dep_name})
	FetchContent_Declare(
		${dep_name}
		GIT_REPOSITORY "${git_repo}"
		GIT_TAG "${git_tag}"
		GIT_SHALLOW 1
		GIT_PROGRESS 1
		UPDATE_COMMAND ""
		PATCH_COMMAND ""
		TEST_COMMAND ""
		DOWNLOAD_DIR ${INSTALL_PREFIX}/src
		SOURCE_DIR ${dep_source_dir}
		BINARY_DIR ${dep_source_dir}-build
		INSTALL_DIR ${INSTALL_PREFIX}
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
				-DCMAKE_INSTALL_PREFIX=${INSTALL_PREFIX}
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
	DOWNLOAD_DIR ${INSTALL_PREFIX}/src
	SOURCE_DIR ${INSTALL_PREFIX}/src/fmt
	BINARY_DIR ${INSTALL_PREFIX}/src/fmt-build
)

set(FMT_DOC OFF CACHE BOOL "fmt generate docs")
set(FMT_TEST OFF CACHE BOOL "fmt build tests")
set(FMT_INSTALL OFF CACHE BOOL "fmt generate install target")
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(
	spdlog
	GIT_REPOSITORY "https://github.com/gabime/spdlog"
	GIT_TAG "v1.8.2"
	GIT_SHALLOW 1
	GIT_PROGRESS 1
	UPDATE_COMMAND ""
	PATCH_COMMAND ""
	TEST_COMMAND ""
	DOWNLOAD_DIR ${INSTALL_PREFIX}/src
	SOURCE_DIR ${INSTALL_PREFIX}/src/spdlog
	BINARY_DIR ${INSTALL_PREFIX}/src/spdlog-build
)

set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "spdlog build example")
set(SPDLOG_BUILD_BENCH OFF CACHE BOOL "spdlog build benchmarks")
set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "spdlog build tests")
set(SPDLOG_INSTALL OFF CACHE BOOL "spdlog generate install target")

#set(SPDLOG_BUILD_WARNINGS ON CACHE BOOL "Enable spdlog compiler warnings")
#set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "spdlog Use external fmt library instead of bundled")
#set(SPDLOG_NO_EXCEPTIONS ON CACHE BOOL "spdlog Compile with -fno-exceptions. Call abort() on any spdlog exceptions") # TODO: use at own peril!
#set(CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO DEVELOP)
FetchContent_MakeAvailable(spdlog)

# TODO: 
# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)
# # CYGWIN and MSYS require compile specific extensions to behave as expected.
# # => -std=c++17 -> -std=gnu++17, for example
# if(CMAKE_SYSTEM_NAME MATCHES "CYGWIN" OR CMAKE_SYSTEM_NAME MATCHES "MSYS")
#     set(CMAKE_CXX_EXTENSIONS ON)
# endif()




















set(PROJECT_NAME rtek)

################################################################################
# Source groups
################################################################################
#set(no_group_source_files
#    ".clang-format"
#    ".clang-tidy"
#)
#source_group("" FILES ${no_group_source_files})

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

#set(Project_Files
#    ".doxyfile"
#    ".editorconfig"
#    "README.md"
#)
#source_group("Project Files" FILES ${Project_Files})

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

set(ALL_FILES
#    ${no_group_source_files}
    ${Header_Files}
#    ${Project_Files}
    ${Source_Files}
)

################################################################################
# Target
################################################################################

add_executable(${PROJECT_NAME} ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE rtek)

set_target_properties(${PROJECT_NAME} PROPERTIES 
	DEBUG_POSTFIX d
)

# Set rtek as the startup project in Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

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
	"${INSTALL_PREFIX}/include"
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
#	"SPDLOG_COMPILED_LIB"
)

################################################################################
# Compile and link options
################################################################################
if(MSVC)
	# TODO: set the exception unwinding feature for now
	target_compile_options(${PROJECT_NAME} PRIVATE
		$<$<CONFIG:Debug>:
			/Od;
			/WX;
			/TP
		>
		$<$<CONFIG:Develop>:
			/GL;
			/O2;
			/Oi;
			/Gy
		>
		$<$<CONFIG:Release>:
			/O2;
			/Oi;
			/Gy
		>
		/permissive-;
		/std:c++17;
		/sdl;
		/W3;
		${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
		${DEFAULT_CXX_EXCEPTION_HANDLING}
	)
	target_link_options(${PROJECT_NAME} PRIVATE
		$<$<CONFIG:Develop>:
			/OPT:REF;
			/OPT:ICF
		>
		$<$<CONFIG:Release>:
			/OPT:REF;
			/OPT:ICF
		>
		/DEBUG:FULL
	)

# TODO: non-MSVC builds
endif()

################################################################################
# Dependencies
################################################################################
add_dependencies(${PROJECT_NAME} spdlog fmt)

#include(${INSTALL_PREFIX}/lib/cmake/spdlog/spdlogConfigTargets.cmake)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog fmt::fmt)
