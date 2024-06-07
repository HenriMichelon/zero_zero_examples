cmake_minimum_required(VERSION 3.29)
cmake_policy(SET CMP0002 NEW)

project(examples)
include(.env.cmake)

# Engine sources
add_subdirectory(${Z0_PROJECT_DIR} external_lib_build)
set(Z0_INCLUDE_DIR ${Z0_PROJECT_DIR}/include)
set(Z0_SHADERS_DIR ${Z0_PROJECT_DIR}/src/z0/shaders)
set(Z0_SHADERS_BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
set(Z0_SHARED_LIB ZeroZero)
file(MAKE_DIRECTORY ${Z0_SHADERS_BUILD_DIR})

# Project sources
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SHADERS_DIR ${SRC_DIR}/shaders)
set(SHADERS_BUILD_DIR ${Z0_SHADERS_BUILD_DIR}/examples)
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
add_shaders(${PROJECT_NAME}_shaders  ${SHADERS_BUILD_DIR} ${Z0_SHADERS_DIR} ${GLSL_SOURCE_FILES})

add_executable(${PROJECT_NAME}
		${INCLUDE_DIR}/example.h
		${SRC_DIR}/example.cpp

		${INCLUDE_DIR}/triangle.h
		${SRC_DIR}/triangle.cpp

		${INCLUDE_DIR}/add_remove_child.h
		${SRC_DIR}/add_remove_child.cpp

		${INCLUDE_DIR}/physics.h
		${SRC_DIR}/physics.cpp

		${INCLUDE_DIR}/ui.h
		${SRC_DIR}/ui.cpp

		${INCLUDE_DIR}/topbar.h
		${SRC_DIR}/topbar.cpp

		${INCLUDE_DIR}/player.h
		${SRC_DIR}/player.cpp

		${INCLUDE_DIR}/raycast.h
		${SRC_DIR}/raycast.cpp

		${INCLUDE_DIR}/crate.h
		${SRC_DIR}/crate.cpp

		${INCLUDE_DIR}/layers.h
		${INCLUDE_DIR}/includes.h


		${INCLUDE_DIR}/z0/raycast.h
		${SRC_DIR}/z0/raycast.cpp
)

add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_shaders ${Z0_SHARED_LIB})
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIR} ${Z0_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${Z0_SHARED_LIB} Xinput dinput8 dxguid)
add_compile_definitions(USE_PCH)
target_precompile_headers(${PROJECT_NAME} PRIVATE
		${Z0_INCLUDE_DIR}/z0/z0.h
)


if(WIN32)
	set_target_properties(${PROJECT_NAME} PROPERTIES
			WIN32_EXECUTABLE TRUE
	)
endif()

include(cmake/jolt.cmake)
include(cmake/libraries.cmake)