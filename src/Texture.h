#pragma once

#include "ShaderManager.hpp"
#include <glm/glm.hpp>

class Texture
{
public:
    Texture(unsigned int width, unsigned int height, GLuint textureFormat);
    ~Texture();

    GLuint getID();
    GLuint getFormat();
    unsigned int getWidth();
    unsigned int getHeight();

private:
    void create();

    unsigned int m_width;
    unsigned int m_height;
    GLuint m_textureID;
    GLuint m_textureFormat;

};