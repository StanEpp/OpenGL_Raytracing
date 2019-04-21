#pragma once

#include "Texture.hpp"
#include "ShaderManager.hpp"

class Screenquad
{
public:
    Screenquad(ShaderManager& shManager);
    ~Screenquad();

    void draw(Texture& textureToRender);

private:
    GLuint m_vertexUVBufferID;
    GLuint m_VertexArrayID;
    GLuint m_shaderProgramID;
};