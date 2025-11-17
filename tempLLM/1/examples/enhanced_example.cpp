#include "../src/gl46.hpp"
#include "../src/vertex_attributes.hpp"
#include "../src/batch_operations.hpp"
#include "../src/concepts.hpp"
#include "../src/validation/validator.hpp"
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

// Расширенный пример использования нашей типобезопасной обертки OpenGL 4.6
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
    GLFWwindow* window = glfwCreateWindow(800, 600, "Enhanced OpenGL 4.6 Type-Safe Wrapper Example", nullptr, nullptr);
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

    // Использование пакетного буфера для оптимизации производительности
    gl46::batch::BatchBuffer<float> vertex_buffer(1000, gl46::BufferUsage::DynamicDraw);
    
    // Данные для нескольких треугольников
    std::vector<float> triangle_vertices = {
        // Первый треугольник
        -0.9f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.7f,  0.0f, 0.0f,
        
        // Второй треугольник
         0.5f, -0.5f, 0.0f,
         0.9f, -0.5f, 0.0f,
         0.7f,  0.0f, 0.0f,
         
        // Третий треугольник
        -0.2f,  0.5f, 0.0f,
         0.2f,  0.5f, 0.0f,
         0.0f,  0.9f, 0.0f
    };
    
    vertex_buffer.AddVector(triangle_vertices);

    // Создание VAO
    gl46::VertexArrayHandle vao = gl46::CreateVertexArray();
    gl46::BindVertexArray(vao);
    
    // Использование типобезопасных вершинных атрибутов
    auto position_attr = gl46::VertexAttribute<gl46::DataType::Float, 3>{0, 0};
    position_attr.ApplyTo(vao);
    
    gl46::VertexArrayVertexBuffer(vao, gl46::VertexBufferBinding{0}, vertex_buffer.GetHandle(), 0, 3 * sizeof(float));

    // Создание шейдеров с использованием RAII оберток
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

    // Использование RAII оберток для шейдеров
    gl46::Shader vertexShader(gl46::ShaderType::VertexShader, vertexShaderSource);
    gl46::Shader fragmentShader(gl46::ShaderType::FragmentShader, fragmentShaderSource);

    // Создание программы
    std::vector<gl46::ShaderHandle> shaders = {vertexShader.GetHandle(), fragmentShader.GetHandle()};
    gl46::Program shaderProgram(shaders);

    // Пакетная отрисовка
    gl46::batch::BatchRenderer batch_renderer;
    batch_renderer.AddDrawCommand(0, 3);    // Первый треугольник
    batch_renderer.AddDrawCommand(3, 3);    // Второй треугольник
    batch_renderer.AddDrawCommand(6, 3);    // Третий треугольник

    // Включение валидации (в реальном приложении можно отключить в релизе)
    #ifdef GL46_ENABLE_VALIDATION
    std::cout << "Runtime validation enabled" << std::endl;
    #endif

    // Основной цикл рендеринга
    while (!glfwWindowShouldClose(window)) {
        // Обработка событий
        glfwPollEvents();

        // Очистка буфера
        gl46::Context::Clear(gl46::ClearBufferMask::ColorBufferBit);

        try {
            // Использование шейдерной программы
            shaderProgram.Use();

            // Рендеринг через пакетный рендерер
            gl46::BindVertexArray(vao);
            batch_renderer.ExecuteDrawCommands(gl46::PrimitiveType::Triangles);

            // Проверка ошибок OpenGL (только если валидация включена)
            gl46::validation::Validator::CheckError("Render Loop");
        } catch (const gl46::exceptions::OpenGLException& e) {
            std::cerr << "OpenGL error: " << e.what() << std::endl;
            break;
        } catch (const std::exception& e) {
            std::cerr << "General error: " << e.what() << std::endl;
            break;
        }

        // Обмен буферов
        glfwSwapBuffers(window);
    }

    // Очистка
    gl46::DestroyVertexArray(vao);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}