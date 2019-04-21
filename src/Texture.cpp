#include "Texture.hpp"
#include <stdexcept>
#include <cassert>

Texture::Texture(GLuint width, GLuint height) :
    m_width(width),
    m_height(height)
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

    if (glGetError() != GL_NO_ERROR) {
        throw std::runtime_error("ERROR: Could not generate texture!");
    }
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_textureID);
}