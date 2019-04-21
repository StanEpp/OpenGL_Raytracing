#pragma once

#include <GL/gl3w.h>

class Texture
{
public:
    Texture(GLuint width, GLuint height);
    ~Texture();

    GLuint ID() const { return m_textureID; }
    GLuint format() const { return m_textureFormat; }
    GLuint width() const { return m_width; }
    GLuint height() const { return m_height; }
    GLuint bindPoint() const { return m_bindPoint; }

private:
    GLuint m_width = 0;
    GLuint m_height = 0;
    GLuint m_textureID = 0;
    GLuint m_bindPoint = 0;
    GLuint m_textureFormat = GL_RGBA8;
};