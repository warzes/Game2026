cmake_minimum_required(VERSION 3.20)
project(OpenGLTypeSafeWrapper)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Include GLEW or glad for OpenGL function loading
find_package(glad QUIET)
if(NOT glad_FOUND)
    find_package(GLEW QUIET)
    if(NOT GLEW_FOUND)
        message(FATAL_ERROR "Neither glad nor GLEW found. Please install one of them.")
    endif()
endif()

# Add executable
add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/opengl_types.hpp
    src/handles.hpp
    src/functions.hpp
    src/core.hpp
    src/buffer.hpp
    src/vertex_array.hpp
    src/shader.hpp
    src/program.hpp
    src/texture.hpp
    src/framebuffer.hpp
    src/renderbuffer.hpp
    src/pipeline.hpp
    src/query.hpp
    src/sampler.hpp
    src/transform_feedback.hpp
    src/vertex_specification.hpp
)

# Link libraries
target_link_libraries(${PROJECT_NAME} 
    OpenGL::GL
    ${GL_LIBRARIES}
)

if(glad_FOUND)
    target_link_libraries(${PROJECT_NAME} glad)
elseif(GLEW_FOUND)
    target_link_libraries(${PROJECT_NAME} GLEW::GLEW)
endif()

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${OPENGL_INCLUDE_DIR}
)