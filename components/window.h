#ifndef WINDOW_H
#define WINDOW_H

#include <iostream>

class Window {
    public:
    
    struct WindowSize
    {
        unsigned int x;
        unsigned int y;
    };

    Window() = default;

    void create()  {
        window = glfwCreateWindow(size.x, size.y, "Hello", nullptr, nullptr);
        if (window == nullptr)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
        }
        glfwMakeContextCurrent(window);
        // glfwSetFramebufferSizeCallback(window, &(this->framebuffer_size_callback));
    }

    void on_resize(WindowSize size) {
        this->size = size;
        glViewport(0, 0, size.x, size.y);
    }

    bool running() {
        return !glfwWindowShouldClose(window);
    }

    const WindowSize& get_size() const { return size; }

    GLFWwindow* get_glfwwindow() { return window; }

private:
    GLFWwindow *window;
    
    WindowSize size = {800, 800};

};

#endif