#pragma once

#include <vector>
#include <string>

#include "GL/gl3w.h"
#include <GLFW/glfw3.h>

#include "ConfigLoader.hpp"
#include "GLFWWindow.hpp"
#include "ShaderManager.hpp"
#include "Texture.hpp"
#include "Screenquad.hpp"
#include "Camera.hpp"
#include "GLFWInput.hpp"


class Raytracer
{
private:
    Settings m_settings;
    GLFWWindow m_window;
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