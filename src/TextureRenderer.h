#pragma once

#include <memory>

#include "ShaderManager.hpp"
#include "Texture.h"

class TextureRenderer
{
public:
    TextureRenderer(const std::shared_ptr<Texture> &texture,
                    const std::shared_ptr<ShaderManager> &shManager);
    ~TextureRenderer();

    void draw();
private:
    void initialize();

    std::string m_shaderProgName;
    GLuint m_vertexUVBufferID;
    GLuint m_VertexArrayID;
    std::shared_ptr<Texture> m_texture;
    std::shared_ptr<ShaderManager> m_shManager;

};