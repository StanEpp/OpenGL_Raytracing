#pragma once

#include <stdlib.h>
#include <stdexcept>

#include <GLFW/glfw3.h>

class GLFWWindow
{
public:
    GLFWWindow(int width, int height, const std::string& windowName, bool fullscreen) :
        m_width(width), m_height(height), m_windowName(windowName), m_fullscreen(fullscreen)
    {
        initialize();
        setVSync(false);
    }

    ~GLFWWindow()
    {
        glfwTerminate();
    }

    int getWidth() const { return m_width; }

    int getHeight() const { return m_height; }

    GLFWwindow* getGLFWwindow() const
    {
        return m_window;
    }

    void swapBuffers(){
        glfwSwapBuffers(m_window);
    }

    void setWindowTitle(const char *title)
    {
        glfwSetWindowTitle(m_window, title);
    }

    void setDefaultWindowTitle()
    {
        glfwSetWindowTitle(m_window, m_windowName.c_str());
    }

    void setVSync(bool enable)
    {
        glfwSwapInterval( enable?1:0);
    }

private:

    void initialize()
    {
        if(glfwInit() != GL_TRUE){
            throw std::runtime_error("Could not initialize GLFW!");
        }

        glfwDefaultWindowHints();
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

        m_window = glfwCreateWindow(m_width, m_height, m_windowName.c_str(), m_fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);

        if(!m_window){
            glfwTerminate();
            throw std::runtime_error("Could not open GLFW Window!");
        }

        setWindowTitle(m_windowName.c_str());

        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        glfwMakeContextCurrent(m_window);
    }

    int	m_width;
    int m_height;
    int	m_windowHandle;
    std::string	m_windowName;
    bool m_fullscreen;
    GLFWwindow*	m_window;
};