cmake_minimum_required(VERSION 3.29)
cmake_policy(SET CMP0002 NEW)

project(examples)
include(.env.cmake)

# Engine sources
add_subdirectory(${Z0_PROJECT_DIR} external_lib_build)
set(Z0_INCLUDE_DIR ${Z0_PROJECT_DIR}/include)
set(Z0_SHADERS_DIR ${Z0_PROJECT_DIR}/src/z0/shaders)
set(Z0_SHADERS_BUILD_DIR ${Z0_PROJECT_DIR}/shaders)
set(Z0_SHARED_LIB ZeroZero)

# Project sources
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SHADERS_DIR ${SRC_DIR}/shaders)
set(SHADERS_BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
file(MAKE_DIRECTORY ${SHADERS_BUILD_DIR})

include(FetchContent)
include(cmake/shaders.cmake)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_COMPILE_WARNING_AS_ERROR OFF) # disabled due to fastgltf and VMA
set(CMAKE_CXX_EXTENSIONS OFF)
if(MINGW)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
endif()
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")

# compile GLSL shaders
file(GLOB_RECURSE GLSL_SOURCE_FILES
		"${SHADERS_DIR}/*.frag"
		"${SHADERS_DIR}/*.vert"
)
add_shaders(${PROJECT_NAME}_shaders ${SHADERS_BUILD_DIR}/examples ${Z0_SHADERS_DIR} ${GLSL_SOURCE_FILES})
add_custom_target(AlwaysExecute ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${Z0_SHADERS_BUILD_DIR} ${SHADERS_BUILD_DIR} 
)
add_dependencies(AlwaysExecute ${PROJECT_NAME}_shaders)

add_executable(${PROJECT_NAME}
		${SRC_DIR}/example.cpp
		${SRC_DIR}/topbar.cpp
		${SRC_DIR}/scenes/triangle.cpp
		${SRC_DIR}/scenes/add_remove_child.cpp
		${SRC_DIR}/scenes/ui.cpp
		${SRC_DIR}/scenes/physics.cpp
		${SRC_DIR}/scenes/terrain.cpp
		${SRC_DIR}/nodes/player.cpp
		${SRC_DIR}/nodes/crate.h
		${SRC_DIR}/nodes/terrain.h
)

add_dependencies(${PROJECT_NAME} ${Z0_SHARED_LIB} AlwaysExecute)
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIR} ${Z0_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${Z0_SHARED_LIB})
add_compile_definitions(JPH_DISABLE_CUSTOM_ALLOCATOR)
#add_compile_definitions(NDEBUG)
#add_compile_definitions(USE_PCH)
target_precompile_headers(${PROJECT_NAME} PRIVATE
		${Z0_INCLUDE_DIR}/z0/libraries.h
		${Z0_INCLUDE_DIR}/z0/z0.h
)
set(SOURCE_FILE "${CMAKE_BINARY_DIR}/external_lib_build/lib${Z0_SHARED_LIB}.dll")
add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE} ${CMAKE_BINARY_DIR}
)

if(WIN32)
	set_target_properties(${PROJECT_NAME} PROPERTIES
			WIN32_EXECUTABLE TRUE
	)
endif()

include(cmake/jolt.cmake)
include(cmake/libraries.cmake)