#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <cmath>

// Константы
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;
const float PI = 3.14159265359f;

// Глобальные переменные
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Камера
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// Позиция подвижного объекта
float objectPositionX = 0.0f;
float objectMovementDirection = 1.0f;

// Объявление функций
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);
unsigned int loadCubemap(std::vector<std::string> faces);

// Шейдерная программа для генерации depth-карты
const char* depthVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}
)";

const char* depthFragmentShaderSource = R"(
#version 330 core
void main()
{
    // gl_FragDepth определяется автоматически
}
)";

// Шейдерная программа для рендеринга сцены с тенями
const char* sceneVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out vec3 WorldPos;  // Передаем worldPos во фрагментный шейдер

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);
    WorldPos = worldPos.xyz;  // Сохраняем worldPos для интерполяции
    FragPos = worldPos.xyz;
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;
    gl_Position = projection * view * worldPos;
}
)";

const char* sceneFragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec3 WorldPos;  // Получаем интерполированный worldPos

uniform mat4 lightSpaceMatrix;  // Матрица для преобразования в пространство света
uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;
uniform sampler2D shadowMap;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // Переводим координаты в диапазон [0, 1]
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    // Получаем ближайшее значение глубины из depth-карты
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // Получаем текущую глубину фрагмента
    float currentDepth = projCoords.z;
    
    // Проверяем, находится ли фрагмент в тени
    float bias = 0.005;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    
    return shadow;
}

void main()
{
    vec3 color = texture(diffuseTexture, TexCoord).rgb;
    vec3 normal = texture(normalTexture, TexCoord).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    
    // Вычисляем FragPosLightSpace во фрагментном шейдере
    vec4 fragPosLightSpace = lightSpaceMatrix * vec4(WorldPos, 1.0);
    float shadow = ShadowCalculation(fragPosLightSpace);
    
    // Тени накладываются как темные области
    vec3 lighting = color * (1.0 - shadow);
    
    FragColor = vec4(lighting, 1.0);
}
)";

int main()
{
    // Инициализация GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Создание окна GLFW
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Shadow Mapping", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Убираем курсор
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Загрузка OpenGL-функций через GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Конфигурация OpenGL
    glEnable(GL_DEPTH_TEST);

    // Компилируем шейдеры для генерации depth-карты
    unsigned int depthVertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(depthVertexShader, 1, &depthVertexShaderSource, NULL);
    glCompileShader(depthVertexShader);
    
    unsigned int depthFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(depthFragmentShader, 1, &depthFragmentShaderSource, NULL);
    glCompileShader(depthFragmentShader);
    
    unsigned int depthShaderProgram = glCreateProgram();
    glAttachShader(depthShaderProgram, depthVertexShader);
    glAttachShader(depthShaderProgram, depthFragmentShader);
    glLinkProgram(depthShaderProgram);

    // Компилируем шейдеры для рендеринга сцены
    unsigned int sceneVertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(sceneVertexShader, 1, &sceneVertexShaderSource, NULL);
    glCompileShader(sceneVertexShader);
    
    unsigned int sceneFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(sceneFragmentShader, 1, &sceneFragmentShaderSource, NULL);
    glCompileShader(sceneFragmentShader);
    
    unsigned int sceneShaderProgram = glCreateProgram();
    glAttachShader(sceneShaderProgram, sceneVertexShader);
    glAttachShader(sceneShaderProgram, sceneFragmentShader);
    glLinkProgram(sceneShaderProgram);

    // Освобождаем ресурсы шейдеров
    glDeleteShader(depthVertexShader);
    glDeleteShader(depthFragmentShader);
    glDeleteShader(sceneVertexShader);
    glDeleteShader(sceneFragmentShader);

    // Определяем вершины куба
    float cubeVertices[] = {
        // positions          // texture coords  // normals
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,       0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,       0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,       0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,       0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,       0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,       0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,       0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,       0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,       0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,       0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,       0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,       0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,      -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,      -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,      -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,      -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,      -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,      -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,       1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,       1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,       1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,       1.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,       1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,       1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,       0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,       0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,       0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,       0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,       0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,       0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,       0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,       0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,       0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,       0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,       0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,       0.0f,  1.0f,  0.0f
    };

    // Определяем вершины плоскости (пола)
    float planeVertices[] = {
        // positions            // texture coords  // normals
         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,  0.0f, 1.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,  0.0f, 1.0f, 0.0f,

         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,  0.0f, 1.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,  0.0f, 1.0f, 0.0f,
         5.0f, -0.5f, -5.0f,  2.0f, 2.0f,  0.0f, 1.0f, 0.0f
    };

    // Настройка VAO и VBO для куба
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Настройка VAO и VBO для плоскости
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);

    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);

    glBindVertexArray(planeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Создание текстур (в реальном приложении здесь загрузка из файлов)
    unsigned int diffuseTexture, normalTexture;
    glGenTextures(1, &diffuseTexture);
    glBindTexture(GL_TEXTURE_2D, diffuseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, 
                 new unsigned char[12]{255, 0, 0, 0, 255, 0, 0, 0, 255, 255, 255, 0});
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &normalTexture);
    glBindTexture(GL_TEXTURE_2D, normalTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, 
                 new unsigned char[12]{128, 128, 255, 128, 128, 255, 128, 128, 255, 128, 128, 255});
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Настройка framebuffer для генерации depth-карты
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
                 SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Позиция солнца (источника света)
    glm::vec3 lightPos = glm::vec3(-2.0f, 4.0f, -1.0f);

    // Цикл рендеринга
    while (!glfwWindowShouldClose(window))
    {
        // Логика подвижного объекта
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        objectPositionX += 1.0f * deltaTime * objectMovementDirection;
        if (objectPositionX > 2.0f || objectPositionX < -2.0f) {
            objectMovementDirection *= -1.0f;
        }

        // Обработка ввода
        processInput(window);

        // Обновление view и projection матриц
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        // Матрица вида и проекции для солнца (теневая матрица)
        float near_plane = 1.0f, far_plane = 7.5f;
        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;

        // Рендеринг depth-карты
        glViewport(0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        
        glUseProgram(depthShaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(depthShaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
        
        // Рендерим статическую геометрию (пол)
        glm::mat4 model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(depthShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glBindVertexArray(planeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        // Рендерим подвижный объект
        model = glm::translate(glm::mat4(1.0f), glm::vec3(objectPositionX, 0.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(depthShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Рендеринг сцены
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glUseProgram(sceneShaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(sceneShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(sceneShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(sceneShaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
        glUniform3fv(glGetUniformLocation(sceneShaderProgram, "lightPos"), 1, &lightPos[0]);
        glUniform3fv(glGetUniformLocation(sceneShaderProgram, "viewPos"), 1, &cameraPos[0]);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseTexture);
        glUniform1i(glGetUniformLocation(sceneShaderProgram, "diffuseTexture"), 0);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalTexture);
        glUniform1i(glGetUniformLocation(sceneShaderProgram, "normalTexture"), 1);
        
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glUniform1i(glGetUniformLocation(sceneShaderProgram, "shadowMap"), 2);

        // Рендерим пол
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(sceneShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glBindVertexArray(planeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        // Рендерим подвижный куб
        model = glm::translate(glm::mat4(1.0f), glm::vec3(objectPositionX, 0.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(sceneShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Обмен буферов и опрос событий
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Освобождение ресурсов
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &planeVBO);
    glDeleteProgram(depthShaderProgram);
    glDeleteProgram(sceneShaderProgram);

    glfwTerminate();
    return 0;
}

// Функции обратного вызова
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Обратный порядок, чтобы Y был положительным вверх

    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    float yaw = xoffset;
    float pitch = yoffset;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    // Не используется в этой реализации
}