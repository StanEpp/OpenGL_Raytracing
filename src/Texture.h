#pragma once

#include "ShaderManager.hpp"
#include <glm/glm.hpp>

class Texture
{
public:
    Texture(int width, int height, GLuint textureFormat);
    ~Texture();

    GLuint getID();
    GLuint getFormat();
    int getWidth();
    int getHeight();

private:
    void create();

    int m_width;
    int m_height;
    GLuint m_textureID;
    GLuint m_textureFormat;

};