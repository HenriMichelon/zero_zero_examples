###### Auto detect Vulkan SDK, required to compile the shaders
find_package(Vulkan REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})

###### Using GLM for maths
FetchContent_Declare(
        fetch_glm
        GIT_REPOSITORY https://github.com/g-truc/glm
        GIT_TAG        1.0.1
)
FetchContent_MakeAvailable(fetch_glm)
target_link_libraries(${PROJECT_NAME} glm::glm)

