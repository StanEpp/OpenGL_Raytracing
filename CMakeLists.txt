cmake_minimum_required(VERSION 3.5)

project(OpenGL_Raytracer)

add_subdirectory(deps)

include_directories(${DEPENDENCY_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR})

set(SOURCE_FILES
    src/main.cpp
    src/gl3w.c
    src/ShaderManager.cpp
    src/Texture.cpp
    src/Screenquad.cpp
    src/SceneManager.cpp
    src/Raytracer.cpp
)

set(HEADER_FILES
    src/Camera.hpp
    src/GLFWWindow.hpp
    src/GLFWInput.hpp
    src/Scene.hpp
    src/SceneManager.hpp
    src/ShaderBaseModel.hpp
    src/ShaderManager.hpp
    src/Texture.hpp
    src/Screenquad.hpp
    src/ConfigLoader.hpp
    src/Raytracer.hpp
)

set(SHADER_FILES
    shader/cs.glsl
)

set(EXAMPLE_FILES
    scenes/CubeScene.mtl
    scenes/CubeScene.obj
    scenes/MirrorRoom.mtl
    scenes/MirrorRoom.obj
)

foreach(SHADER ${SHADER_FILES})
configure_file(${SHADER} ${SHADER} COPYONLY)
endforeach()

foreach(EXAMPLE ${EXAMPLE_FILES})
configure_file(${EXAMPLE} ${EXAMPLE} COPYONLY)
endforeach()

configure_file(settings.txt settings.txt COPYONLY)
add_executable(OpenGL_Raytracer ${SOURCE_FILES} ${HEADER_FILES})
add_dependencies(OpenGL_Raytracer glfw_ext glm_ext objSpirit_ext)
target_link_libraries(OpenGL_Raytracer PUBLIC glfw objSpirit dl)
target_compile_options(OpenGL_Raytracer PUBLIC -Wall -g)
