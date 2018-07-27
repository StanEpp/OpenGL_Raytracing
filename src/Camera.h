#pragma once

#include <glm/glm.hpp>
#include <cmath>

class Camera
{
public:

    Camera(int width, int height, float fovY, glm::vec3 pos, glm::vec3 lookAt, glm::vec3 up) :
        m_width(width),
        m_height(height),
        m_pos(pos)
    {
        m_dir = glm::normalize(lookAt-pos);
        m_xAxis = glm::normalize(glm::cross(up, m_dir));
        m_yAxis = glm::cross(-m_dir, m_xAxis);

        m_tanFovY = std::tan(fovY * std::acos(-1) / 180.f / 2.0f);
        m_tanFovX = (static_cast<float>(m_width)*m_tanFovY)/static_cast<float>(m_height);
    }

    int getWidth() const{ return m_width; }
    int getHeight() const { return m_height; }
    float getTanFovY() const{ return m_tanFovY; }
    float getTanFovX() const { return m_tanFovX; }
    glm::vec3 getPos() const { return m_pos; }
    glm::vec3 getDir() const { return m_dir; }
    glm::vec3 getXAxis() const { return m_xAxis; }
    glm::vec3 getYAxis() const { return m_yAxis; }

    void setTransformTranslate(glm::mat4 transf)
    {
        m_transfT = transf;
    }

    void setTransformRotate(glm::mat4 transf)
    {
        m_transfR = transf;
    }

    void update()
    {
        glm::vec4 temp = m_transfT*glm::vec4(m_pos, 1);
        m_pos = glm::vec3(temp.x, temp.y, temp.z);

        temp = m_transfR*glm::vec4(m_dir, 0);
        m_dir = glm::vec3(temp.x, temp.y, temp.z);

        temp = m_transfR*glm::vec4(m_xAxis, 0);
        m_xAxis = glm::vec3(temp.x, temp.y, temp.z);

        temp = m_transfR*glm::vec4(m_yAxis, 0);
        m_yAxis = glm::vec3(temp.x, temp.y, temp.z);
    }

private:
    int	m_width;
    int m_height;
    float m_tanFovY;
    int m_tanFovX;

    glm::vec3 m_pos;
    glm::vec3 m_dir;
    glm::vec3 m_xAxis;
    glm::vec3 m_yAxis;
    glm::mat4 m_transfT;
    glm::mat4 m_transfR;
};