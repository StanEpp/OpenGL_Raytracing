#pragma once

#include <cmath>

#include "GLFWInput.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:

    Camera(int width, int height, float fovY, float sensitivity,
           glm::vec3 pos, glm::vec3 lookAt, glm::vec3 up)
    :   m_width(width),
        m_height(height),
        m_sensitivity(sensitivity),
        m_pos(pos)
    {
        m_lookDir = glm::normalize(lookAt - pos);
        m_right = glm::normalize(glm::cross(up, m_lookDir));
        m_up = glm::cross(m_right, m_lookDir);

        m_fovY = std::tan(fovY * std::acos(-1) / 180.f / 2.0f);
        m_fovX = (static_cast<float>(m_width) * m_fovY)/static_cast<float>(m_height);
    }

    int width() const{ return m_width; }
    int height() const { return m_height; }
    float fovY() const{ return m_fovY; }
    float fovX() const { return m_fovX; }
    const glm::vec3& pos() const { return m_pos; }
    const glm::vec3& lookDir() const { return m_lookDir; }
    const glm::vec3& up() const { return m_up; }
    const glm::vec3& right() const { return m_right; }

    void update(const GLFWInput &input, float dt)
    {
        float dz, dx, dy;
        dz = dx = dy = 0.f;

        if (input.isKeyPressed(GLFW_KEY_W)) {
            dz = m_sensitivity * dt;
        } else if (input.isKeyPressed(GLFW_KEY_S)) {
            dz = -1.f * m_sensitivity * dt;
        }

        if (input.isKeyPressed(GLFW_KEY_D)) {
            dx = m_sensitivity * dt;
        } else if (input.isKeyPressed(GLFW_KEY_A)) {
            dx = -1.f * m_sensitivity * dt;
        }

        m_accRotY += input.getXPosDiff() * m_sensitivity * dt;
        m_accRotX += input.getYPosDiff() * m_sensitivity * dt;
        m_accRotY = std::fmod(m_accRotY, 360);
        m_accRotX = std::fmod(m_accRotX, 360);

        auto rotX = glm::rotate(glm::mat4(1.f), m_accRotX, glm::vec3(1.f, 0.f, 0.f));
        auto rotY = glm::rotate(glm::mat4(1.f), m_accRotY, glm::vec3(0.f, 1.f, 0.f));

        auto rotMatrix = rotY * rotX;

        m_lookDir  = glm::normalize(glm::vec3(rotMatrix * glm::vec4(0.f, 0.f, -1.f, 0.f)));
        m_up  = glm::normalize(glm::vec3(rotMatrix * glm::vec4(0.f, 1.f, 0.f, 0.f)));
        m_right = glm::normalize(glm::cross(m_lookDir, m_up));

        m_pos += m_movSpeed * ( dz * m_lookDir + dx * m_right);
    }

private:
    int	m_width = 1024;
    int m_height = 768;
    float m_fovY = 0.f;
    float m_fovX = 0.f;
    float m_accRotX = 0.f;
    float m_accRotY = 0.f;
    float m_sensitivity  = 1.f;
    float m_movSpeed = 1.f;

    glm::vec3 m_pos{ 0.f, 0.f, 0.f };
    glm::vec3 m_lookDir{ 0.f, 0.f, -1.f };
    glm::vec3 m_up{ 0.f, 1.f, 0.f };
    glm::vec3 m_right{ 1.f, 0.f, 0.f };
};