cmake_minimum_required(VERSION 3.15.0 FATAL_ERROR)

set(CMAKE_SYSTEM_VERSION 10.0.18362.0 CACHE STRING "" FORCE)

project(rtek CXX)

################################################################################
# Set target arch type if empty. Visual studio solution generator provides it.
################################################################################
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
    CACHE STRING "" FORCE
)

################################################################################
# Global compiler options
################################################################################
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
# Nuget packages function stub.
################################################################################
function(use_package TARGET PACKAGE VERSION)
    message(WARNING "No implementation of use_package. Create yours. "
                    "Package \"${PACKAGE}\" with version \"${VERSION}\" "
                    "for target \"${TARGET}\" is ignored!")
endfunction()

################################################################################
# Common utils
################################################################################
include(CMake/Utils.cmake)

################################################################################
# Additional Global Settings(add specific info there)
################################################################################
include(CMake/GlobalSettingsInclude.cmake OPTIONAL)

################################################################################
# Use solution folders feature
################################################################################
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################################################################
# Sub-projects
################################################################################
add_subdirectory(external/fmt/_build)
add_subdirectory(external/spdlog/_build)



























set(PROJECT_NAME rtek)

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

set(Project_Files
    ".doxyfile"
    ".editorconfig"
    "README.md"
)
source_group("Project Files" FILES ${Project_Files})

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
    ${no_group_source_files}
    ${Header_Files}
    ${Project_Files}
    ${Source_Files}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE rtek)

################################################################################
# Output directory
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/build/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
        OUTPUT_DIRECTORY_DEVELOP "${CMAKE_SOURCE_DIR}/build/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
        OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/build/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/build/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
        OUTPUT_DIRECTORY_DEVELOP "${CMAKE_SOURCE_DIR}/build/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
        OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/build/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
    )
endif()
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_DEVELOP "TRUE"
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_DEVELOP "TRUE"
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
endif()
################################################################################
# Include directories
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/external/spdlog/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/external/spdlog/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
    )
endif()

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "RK_ASSERTIONS_ENABLED;"
            "RK_TARGET_ARCH_X86"
        ">"
        "$<$<CONFIG:Develop>:"
            "RK_ASSERTIONS_ENABLED;"
            "RK_TARGET_ARCH_X86"
        ">"
        "$<$<CONFIG:Release>:"
            "TARGET_ARCH_X86"
        ">"
        "SPDLOG_COMPILED_LIB;"
        "_MBCS"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "RK_ASSERTIONS_ENABLED;"
            "RK_TARGET_ARCH_X86"
        ">"
        "$<$<CONFIG:Develop>:"
            "RK_ASSERTIONS_ENABLED;"
            "RK_TARGET_ARCH_X86"
        ">"
        "$<$<CONFIG:Release>:"
            "TARGET_ARCH_X86"
        ">"
        "SPDLOG_COMPILED_LIB;"
        "_MBCS"
    )
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /Od
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
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
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
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
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
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
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
    endif()
endif()

################################################################################
# Dependencies
################################################################################
add_dependencies(${PROJECT_NAME}
    fmt
    spdlog
)

# Link with other targets.
target_link_libraries(${PROJECT_NAME} PRIVATE
    fmt
    spdlog
)

