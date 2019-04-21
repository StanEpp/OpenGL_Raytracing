#pragma once

#include "GL/gl3w.h"
#include <GLFW/glfw3.h>

#include <cassert>
#include <memory>
#include "GLFWWindow.hpp"
#include "ShaderManager.hpp"
#include "GLFWTimer.hpp"
#include "SceneManager.hpp"
#include "GLFWInput.hpp"
#include "ConfigLoader.hpp"
#include "Screenquad.hpp"
#include "Texture.hpp"
#include "Camera.hpp"


class Raytracer
{
private:
    Settings m_settings;
    std::unique_ptr<GLFWWindow> m_window;
    ShaderManager m_shManager;
    Texture m_renderedToTexture;
    Screenquad m_screenquad;
    Camera m_camera;
    GLFWInput m_inputControl;

    GLuint m_computeShaderID = 0;
    GLuint* m_storageBufferIDs = nullptr;

    void createComputeShader();
    void raytraceScene();
    void renderTexture();

public:
    Raytracer(const std::string& settingsFile, const std::vector<std::string> &sceneFiles);
    ~Raytracer();

    void run();
};