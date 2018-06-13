cmake_minimum_required(VERSION 3.5)

project(OpenGL_Raytracer)

add_subdirectory(deps)

include_directories(${DEPENDENCY_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR})

set(SOURCE_FILES
    src/GLFWInput.cpp
    src/InputControl.cpp
    src/main.cpp
    src/OpenGLRaytracer.cpp
    src/rply.c
    src/gl3w.c
    src/SceneLoader.cpp
    src/SceneReader.cpp
    src/ShaderManager.cpp
    src/Texture.cpp
    src/TextureRenderer.cpp
)

set(HEADER_FILES
    src/Camera.h
    src/GLFWInput.h
    src/GLFWWindow.h
    src/InhomogenousContainer.h
    src/Input.h
    src/InputControl.h
    src/Lights.h
    src/OpenGLRaytracer.h
    src/Primitives.h
    src/Ray.h
    src/rply.h
    src/Scene.h
    src/SceneLoader.h
    src/SceneReader.h
    src/ShaderBaseModel.h
    src/ShaderManager.hpp
    src/Texture.h
    src/TextureRenderer.h
    src/Vertex.h
    src/ConfigLoader.h
)

add_executable(OpenGL_Raytracer ${SOURCE_FILES} ${HEADER_FILES})
add_dependencies(OpenGL_Raytracer glfw_ext glm_ext)
target_link_libraries(OpenGL_Raytracer PUBLIC glfw libstdc++)

