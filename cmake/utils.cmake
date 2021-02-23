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