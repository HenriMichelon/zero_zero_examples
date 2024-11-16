cmake_minimum_required(VERSION 3.29)
cmake_policy(SET CMP0002 NEW)

project(examples)
include(.env.cmake)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

# Auto detect Vulkan SDK, required to compile the shaders
find_package(Vulkan REQUIRED)
#add_compile_definitions(JPH_DEBUG_RENDERER)
add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)
add_compile_definitions(GLM_ENABLE_CXX_20 ON)

# Engine sources
add_subdirectory(${Z0_PROJECT_DIR} external_lib_build)
set(Z0_INCLUDE_DIR ${Z0_PROJECT_DIR}/include)
set(Z0_SHADERS_DIR ${Z0_PROJECT_DIR}/src/z0/shaders)
set(Z0_SHADERS_BUILD_DIR ${Z0_PROJECT_DIR}/shaders)
set(Z0_TARGET "ZeroZero")

# Project sources
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SHADERS_DIR ${SRC_DIR}/shaders)
set(SHADERS_BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
set(SHADERS_PROJECT_BUILD_DIR ${SHADERS_BUILD_DIR}/examples)
file(MAKE_DIRECTORY ${SHADERS_PROJECT_BUILD_DIR})

include(FetchContent)
include(cmake/shaders.cmake)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE GLSL_SOURCE_FILES
		"${SHADERS_DIR}/*.frag"
		"${SHADERS_DIR}/*.vert"
)
add_executable(${PROJECT_NAME}
		${SRC_DIR}/main.cpp
		${SRC_DIR}/ui/topbar.cpp
		${SRC_DIR}/scenes/add_remove_child.cpp
		${SRC_DIR}/scenes/example.cpp
		${SRC_DIR}/scenes/physics.cpp
		${SRC_DIR}/scenes/terrain.cpp
		${SRC_DIR}/scenes/triangle.cpp
		${SRC_DIR}/nodes/crate.cpp
		${SRC_DIR}/nodes/player.cpp
		${SRC_DIR}/nodes/terrain.cpp
)
target_sources(${PROJECT_NAME}
	PUBLIC
	FILE_SET CXX_MODULES
	FILES
		${SRC_DIR}/layers.cppm
		${SRC_DIR}/ui/topbar.cppm
		${SRC_DIR}/scenes/add_remove_child.cppm
		${SRC_DIR}/scenes/example.cppm
		${SRC_DIR}/scenes/physics.cppm
		${SRC_DIR}/scenes/terrain.cppm
		${SRC_DIR}/scenes/triangle.cppm
		${SRC_DIR}/nodes/crate.cppm
		${SRC_DIR}/nodes/platform_player.cppm
		${SRC_DIR}/nodes/player.cppm
		${SRC_DIR}/nodes/terrain.cppm
)
compile_options(${PROJECT_NAME})

add_shaders(${PROJECT_NAME}_shaders ${SHADERS_PROJECT_BUILD_DIR} ${Z0_SHADERS_DIR} ${GLSL_SOURCE_FILES})
add_custom_target(post_build_shaders ALL
		COMMENT "Copying Shaders [${Z0_TARGET}]"
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${Z0_SHADERS_BUILD_DIR} ${SHADERS_BUILD_DIR}
		BYPRODUCTS ${SHADERS_BUILD_DIR}/default.vert.spv
)
add_dependencies(${PROJECT_NAME} post_build_shaders)
add_dependencies(post_build_shaders ${PROJECT_NAME}_shaders)
add_dependencies(${PROJECT_NAME}_shaders ${Z0_TARGET})
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIR} ${Z0_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${Z0_TARGET})

if(WIN32)
	if(MINGW)
		target_link_options(${PROJECT_NAME} PRIVATE "-mwindows")
	endif()
	set_target_properties(${PROJECT_NAME} PROPERTIES
		WIN32_EXECUTABLE TRUE
	)
endif()

