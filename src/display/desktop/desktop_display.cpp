#include <glad/glad.h>
#include <iostream>
#include <functional>

#include "desktop_display.h"

namespace display::desktop
{
    const char *vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
})";

    const char *fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
void main()
{
    FragColor = vec4(0.2f, 0.3f, 0.8f, 1.0f);  // Set the triangle color
})";
    float vertices[] = {
        // Positions        // Texture coordinates
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // Top-left
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // Bottom-left
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,  // Bottom-right
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f    // Top-right
    };

    unsigned int shaderProgram;
    unsigned int VAO, VBO;
    unsigned int texture;

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
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
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
        glfwMakeContextCurrent(window);

        // glad // todo no glad for now
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cerr << "Failed to initialize GLAD" << std::endl;
            exit(1);
        }

        if (!gladLoadGL())
        {
            std::cerr << "Failed to load OpenGL functions" << std::endl;
            exit(1);
        }

        glViewport(0, 0, this->options.attr.width, this->options.attr.height);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

        // shaders compile
        unsigned int vertexShader;
        unsigned int fragmentShader;

        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
        glCompileShader(vertexShader);

        int success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
            std::cerr << "ERROR: Vertex Shader Compilation Failed\n"
                      << infoLog << std::endl;
            exit(1);
        }

        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
        glCompileShader(fragmentShader);

        // Check fragment shader compilation
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
            std::cerr << "ERROR: Fragment Shader Compilation Failed\n"
                      << infoLog << std::endl;
            exit(1);
        }

        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        // Check linking errors
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
            std::cerr << "ERROR: Shader Program Linking Failed\n"
                      << infoLog << std::endl;
            exit(1);
        }

        // Clean up shaders as they are now linked
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        // vbaoaowvba
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0); // Position attribute
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float))); // Texture attribute
        glEnableVertexAttribArray(1);

        // Unbind VBO and VAO
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    }

    void DesktopDisplay::draw(int index, const void *buffer, size_t size)
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->options.attr.width, this->options.attr.height, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);
        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, texture);

        // Bind VAO and draw the fullscreen quad
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // Check for OpenGL errors
        GLint err = glGetError();
        if (err != GL_NO_ERROR)
        {
            std::cerr << "OpenGL Error: " << err << std::endl;
        }
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