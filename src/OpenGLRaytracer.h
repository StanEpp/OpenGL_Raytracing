#pragma once

#define WORK_GROUP_SIZE 16

#include <memory>

#include "ShaderManager.hpp"
#include "Texture.h"
#include "Camera.h"
#include "Lights.h"
#include "Primitives.h"

class OpenGLRaytracer
{
public:
    OpenGLRaytracer(std::shared_ptr<Texture>& textureToRender, Camera&, int reflectionDepth, std::shared_ptr<ShaderManager>& shManager);
    ~OpenGLRaytracer();

    void renderScene(Camera& camera, int width, int height, int reflectionDepth);
    std::string getCompShaderProgName();
    GLuint* getStorageBufferIDs();

private:
    void createComputeshader(Camera&,int);

    GLuint* m_storageBufferIDs;
    std::string m_shaderProgName;
    std::shared_ptr<Texture> m_texture;
    std::shared_ptr<ShaderManager> m_shManager;
};