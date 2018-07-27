#pragma once

#define GLM_FORCE_RADIANS

#include <memory>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "ShaderManager.hpp"
#include "Texture.h"

class TextureRenderer
{
public:
    TextureRenderer(int width, int height,
                    const std::shared_ptr<Texture> &texture,
                    const std::shared_ptr<ShaderManager> &shManager);
    ~TextureRenderer();

    void draw();
private:
    void initialize();

    int m_width;
    int m_height;
    std::string m_shaderProgName;
    GLuint m_vertexUVBufferID;
    GLuint m_VertexArrayID;
    std::shared_ptr<Texture> m_texture;
    std::shared_ptr<ShaderManager> m_shManager;

};