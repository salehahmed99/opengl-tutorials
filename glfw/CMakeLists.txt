cmake_minimum_required(VERSION 3.10.0)

project(create_window_example VERSION 0.1.0)


include_directories(
    glfw-3.4/include
    glad/include)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)        # Don't build Documentation
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)       # Don't build Tests
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)    # Don't build Examples
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)           # Don't build Installation Information
set(GLFW_USE_HYBRID_HPG ON CACHE BOOL "" FORCE)     # Set GLFW_USE_HYBRID_HPG variable to use High Performance Graphics Card if available


add_subdirectory(glfw-3.4)

add_executable(main main.cpp glad/src/glad.c)

target_link_libraries(main glfw)
