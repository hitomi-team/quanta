cmake_minimum_required(VERSION 3.1)
project(interspace VERSION 0.1 LANGUAGES C CXX)

macro (propagate_sources)
	set (SRCS ${SRCS} PARENT_SCOPE)
endmacro()

find_path(SDL2_INCLUDE_DIR SDL.h PATH_SUFFIXES SDL2)
find_library(SDL2_LIBRARY SDL2)
find_library(SDL2MAIN_LIBRARY SDL2main)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SDL2 DEFAULT_MSG SDL2_LIBRARY SDL2_INCLUDE_DIR)
include_directories(${SDL2_INCLUDE_DIR})

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

macro (add_sources)
	file (RELATIVE_PATH _relPath "${PROJECT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
	foreach (_src ${ARGN})
		if (_relPath)
			list (APPEND SRCS "${_relPath}/${_src}")
		else()
			list (APPEND SRCS "${_src}")
		endif()
	endforeach()
	if (_relPath)
		# propagate SRCS to parent directory
		set (SRCS ${SRCS} PARENT_SCOPE)
	endif()
endmacro()

find_package(Vulkan REQUIRED)
find_program(GLSLC_EXECUTABLE NAMES glslc PATHS $ENV{VULKAN_SDK}/bin NO_CMAKE_FIND_ROOT_PATH)
message(STATUS "Found glslc: ${GLSLC_EXECUTABLE}")

macro(compile_shader SHADERSRC)
	set(SHADERSRC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/shaders/${SHADERSRC})

	get_filename_component(SHADERSRC_NAME ${SHADERSRC} NAME)
	set(SHADER_HEX_FILE ${CMAKE_BINARY_DIR}/dep/${SHADERSRC}.spv.h)

	add_custom_command(OUTPUT ${SHADER_HEX_FILE}
		COMMAND ${GLSLC_EXECUTABLE}
		ARGS -mfmt=c -c -o ${SHADER_HEX_FILE} -c ${SHADERSRC_PATH}
		DEPENDS ${SHADERSRC_PATH}
		COMMENT "Building SPIR-V module header ${SHADERSRC}.spv.h"
		VERBATIM
	)

	set_source_files_properties(${SHADER_HEX_FILE} PROPERTIES GENERATED TRUE)
	list(APPEND SHADER_FILES ${SHADER_HEX_FILE})
endmacro()

macro (add_shaders)
	file (RELATIVE_PATH _relPath "${PROJECT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
	foreach (_src ${ARGN})
		compile_shader("${_relPath}/${_src}")
	endforeach()
	if (_relPath)
		# propagate SRCS to parent directory
		set (SRCS ${SRCS} PARENT_SCOPE)
	endif()
endmacro()

add_subdirectory(src)
if (WIN32)
	set(SYSTEM_LIBRARIES ws2_32 winmm setupapi version imm32)
else()
	set(SYSTEM_LIBRARIES m rt pthread)
endif()

add_custom_target(shaders ALL DEPENDS ${SHADER_FILES})
add_executable(interspace ${SRCS} ${SHADER_FILES})

target_link_libraries(interspace
	${SYSTEM_LIBRARIES}
	${SDL2_LIBRARY}
	${SDL2MAIN_LIBRARY}
)

if (MSVC)
	target_compile_definitions(interspace PRIVATE -D_CRT_SECURE_NO_WARNINGS)
endif()

target_compile_definitions(interspace PRIVATE -DGLM_DEPTH_ZERO_TO_ONE -DGLM_FORCE_RADIANS)
target_precompile_headers(interspace PUBLIC "$<$<COMPILE_LANGUAGE:CXX>:${PCH_FILE}>")

target_include_directories(interspace PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_BINARY_DIR}/dep
)

target_link_directories(interspace PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib)

set_property(TARGET interspace PROPERTY CXX_STANDARD 17)
set_property(TARGET interspace PROPERTY C_STANDARD 99)
