#include "../src/gl46.hpp"
#include <GLFW/glfw3.h>
#include <iostream>

// Простой пример использования нашей типобезопасной обертки OpenGL 4.6
int main() {
    // Инициализация GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Настройка контекста OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Создание окна
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL 4.6 Type-Safe Wrapper Example", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Инициализация нашей обертки
    if (!gl46::Initialize()) {
        std::cerr << "Failed to initialize OpenGL 4.6 wrapper" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Проверка версии OpenGL
    if (!gl46::IsVersionSupported(4, 6)) {
        std::cerr << "OpenGL 4.6 not supported" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Установка Viewport
    gl46::Context::SetViewport(0, 0, 800, 600);

    // Установка цвета очистки
    gl46::Context::SetClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    // Создание буфера вершин
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    gl46::Buffer vbo(gl46::BufferType::ArrayBuffer, sizeof(vertices), vertices, gl46::BufferUsage::StaticDraw);

    // Создание VAO
    gl46::VertexArrayHandle vao = gl46::CreateVertexArray();
    gl46::BindVertexArray(vao);
    
    gl46::VertexArrayVertexBuffer(vao, gl46::VertexBufferBinding{0}, vbo.GetHandle(), 0, 3 * sizeof(float));
    gl46::EnableVertexArrayAttrib(vao, gl46::VertexAttribBinding{0});
    gl46::VertexArrayAttribFormat(vao, gl46::VertexAttribBinding{0}, 3, gl46::DataType::Float, false, 0);

    // Создание шейдеров
    const char* vertexShaderSource = R"(
        #version 460 core
        layout (location = 0) in vec3 aPos;
        void main() {
            gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
        }
    )";

    const char* fragmentShaderSource = R"(  
        #version 460 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
        }
    )";

    gl46::Shader vertexShader(gl46::ShaderType::VertexShader, vertexShaderSource);
    gl46::Shader fragmentShader(gl46::ShaderType::FragmentShader, fragmentShaderSource);

    // Создание программы
    std::vector<gl46::ShaderHandle> shaders = {vertexShader.GetHandle(), fragmentShader.GetHandle()};
    gl46::Program shaderProgram(shaders);

    // Основной цикл рендеринга
    while (!glfwWindowShouldClose(window)) {
        // Обработка событий
        glfwPollEvents();

        // Очистка буфера
        gl46::Context::Clear(gl46::ClearBufferMask::ColorBufferBit);

        // Использование шейдерной программы
        shaderProgram.Use();

        // Рендеринг треугольника
        gl46::BindVertexArray(vao);
        gl46::DrawArrays(gl46::PrimitiveType::Triangles, 0, 3);

        // Обмен буферов
        glfwSwapBuffers(window);
    }

    // Очистка
    gl46::DestroyVertexArray(vao);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}