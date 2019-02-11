#pragma once

#include "GLFWInput.hpp"

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

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
        m_up = glm::cross(m_lookDir, m_right);

        m_fovY = std::tan(fovY * std::acos(-1) / 180.f / 2.0f);
        m_fovX = (static_cast<float>(m_width)*m_fovY)/static_cast<float>(m_height);
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
        float dz, dx, dy, dyRot, dxRot, dzRot;
        dz = dx = dy = dyRot = dxRot = dzRot = 0.f;

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

        dyRot = -1.f * input.getXPosDiff() * m_sensitivity * dt;
        dxRot = -1.f * input.getYPosDiff() * m_sensitivity * dt;
        m_pos += 6.f * ( dz * m_lookDir + dx * m_right);
        m_right = glm::normalize(glm::rotate(m_right, dyRot, m_up));
        m_up = glm::normalize(glm::rotate(m_up, dxRot, m_right));
        m_lookDir = glm::normalize(glm::cross(m_right, m_up));

        // std::cout << "fovX " << m_fovX << " fovY " << m_fovY << std::endl;
        std::cout << m_pos.x << " " << m_pos.y << " " << m_pos.z << std::endl;
        // std::cout << m_dir.x << " " << m_dir.y << " " << m_dir.z << " " << std::endl;
        // std::cout << m_lookDir.x << " " << m_lookDir.y << " " << m_lookDir.z << " " << std::endl;
    }

private:
    int	m_width;
    int m_height;
    float m_fovY;
    float m_fovX;
    float m_sensitivity;
    glm::vec3 m_pos;
    glm::vec3 m_lookDir;
    glm::vec3 m_up;
    glm::vec3 m_right;
};