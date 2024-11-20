#include <GL/gl.h>
#include <iostream>
#include <functional>

#include "desktop_display.h"

namespace display::desktop
{
    DesktopDisplay::DesktopDisplay(Options &options)
    {
        this->options = options;
    }

    DesktopDisplay::~DesktopDisplay()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void DesktopDisplay::framebufferSizeCallback(GLFWwindow *window, int width, int height)
    {
        DesktopDisplay *disp = reinterpret_cast<DesktopDisplay *>(glfwGetWindowUserPointer(window));
        disp->options.attr.width = width;
        disp->options.attr.height = height;
        glViewport(0, 0, width, height); // Update OpenGL viewport
    };

    void DesktopDisplay::init()
    {
        if (!glfwInit())
        {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            exit(1);
        }

        // Configure GLFW
        glfwDefaultWindowHints();
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        // glfwWindowHint(GLFW_SAMPLES, 16);
        glfwSwapInterval(this->options.vsync ? 1 : 0);

        // Create window
        window = glfwCreateWindow(this->options.attr.width, this->options.attr.height, this->options.title.c_str(), NULL, NULL);
        if (!window)
        {
            std::cerr << "Failed to create window" << std::endl;
            glfwTerminate();
            exit(1);
        }
        std::cout << "Window created" << std::endl;

        // Set minimum window size
        glfwSetWindowSizeLimits(window, 100, 100, GLFW_DONT_CARE, GLFW_DONT_CARE);

        // Set callbacks
        glfwSetWindowUserPointer(window, reinterpret_cast<void *>(this));
        glfwSetFramebufferSizeCallback(window, this->framebufferSizeCallback);

        // glad // todo no glad for now
        // if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        // {
        //     std::cerr << "Failed to initialize GLAD" << std::endl;
        //     exit(1);
        // }

        // load OpenGL functions
        // if (!gladLoadGL())
        // {
        //     std::cerr << "Failed to load OpenGL functions" << std::endl;
        //     exit(1);
        // }

        glViewport(0, 0, this->options.attr.width, this->options.attr.height);

        std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    }

    void DesktopDisplay::draw(int index, const void *buffer, size_t size)
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawPixels(this->options.attr.width, this->options.attr.height, GL_RGB, GL_UNSIGNED_BYTE, buffer);
    }

    Atrributes DesktopDisplay::displayAttributes(int index) const
    {
        return this->options.attr;
    }

    void DesktopDisplay::update()
    {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    void DesktopDisplay::show()
    {
        glfwShowWindow(window);
    }

    void DesktopDisplay::hide()
    {
        glfwHideWindow(window);
    }

    bool DesktopDisplay::shouldClose()
    {
        return glfwWindowShouldClose(window);
    }
}