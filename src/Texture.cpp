#include "Texture.h"

Texture::Texture(unsigned int width, unsigned int height, GLuint textureFormat) :
    m_width(width),
    m_height(height),
    m_textureFormat(textureFormat)
{
    create();
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_textureID);
}

void Texture::create()
{
    assert(glGetError() == GL_NO_ERROR);

    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &m_textureID);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, m_textureFormat, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    if(glGetError() != GL_NO_ERROR){
        throw std::runtime_error("ERROR: Could not generate texture!");
    }
}

GLuint Texture::getFormat()
{
    return m_textureFormat;
}

GLuint Texture::getID()
{
    return m_textureID;
}

unsigned int Texture::getWidth()
{
    return m_width;
}

unsigned int Texture::getHeight()
{
    return m_height;
}