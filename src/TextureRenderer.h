#pragma once

#define GLM_FORCE_RADIANS

#include <memory>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "ShaderManager.hpp"
#include "Texture.h"
#include "Vertex.h"
#include "Camera.h"
#include "Ray.h"

class TextureRenderer
{
private:
    int _width, _height;

    std::string _shaderProgName;
    GLuint _vertexUVBufferID, _VertexArrayID;
    std::shared_ptr<Texture> _texture;
    std::shared_ptr<ShaderManager> m_shManager;

    void initialize();
public:
    TextureRenderer(int width, int height,
                    const std::shared_ptr<Texture> &texture,
                    const std::shared_ptr<ShaderManager> &shManager);
    ~TextureRenderer();

    void draw();
};