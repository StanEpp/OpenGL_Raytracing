#pragma once

#include <GLFW/glfw3.h>

class GLFWTimer
{
private:
    double m_time{0};
    double m_oldTime{0};

public:
    //Receives time since glfwInit() call
    double getTime() const
    {
        return m_time + glfwGetTime();
    }

    //Returns the time difference since the last getTimeDiff() call
    double getTimeDiff()
    {
        double currTime = getTime();
        double timeDiff = currTime - m_oldTime;
        m_oldTime = currTime;

        return timeDiff;
    }

    //Returns the time difference since the last getTimeDiff() call
    double getTimeDiffWithoutActualization() const
    {
        double currTime = getTime();
        double timeDiff = currTime - m_oldTime;

        return timeDiff;
    }

    //Returns the time since the last resetTime() call
    double getRefreshedTime() const
    {
        return glfwGetTime();
    }

    //Resets the time to 0
    void resetTime()
    {
        m_time += glfwGetTime();
        glfwSetTime(0.0);
    }
};