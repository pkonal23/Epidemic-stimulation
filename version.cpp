#include <GLFW/glfw3.h>
#include <iostream>

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Version Query", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Print OpenGL version
    const char* gl_version = (const char*)glGetString(GL_VERSION);
    std::cout << "OpenGL Version: " << gl_version << std::endl;

    // Query and print GLSL version
    const char* glsl_version = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
    std::cout << "GLSL Version: " << glsl_version << std::endl;

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
