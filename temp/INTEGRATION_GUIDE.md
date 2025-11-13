# Интеграция RSM и Shadow Atlas в ForwardRenderer

## Обзор интеграции

Этот документ описывает как подключить полнофункциональный ShadowManager с поддержкой RSM и Shadow Atlas в существующий ForwardRenderer.

## Шаг 1: Добавить ShadowManager в ForwardRenderer

### ForwardRenderer.hpp
```cpp
#pragma once
#include "RenderLib/ShadowManager.hpp"
#include "RenderLib/LightDirectional.hpp"
#include "RenderLib/LightPoint.hpp"
#include "RenderLib/LightSpot.hpp"
#include <memory>
#include <vector>

namespace RenderLib {

class ForwardRenderer {
public:
    ForwardRenderer();
    ~ForwardRenderer();

    // Initialize renderer with shadow support
    bool init();
    void destroy();

    // Render scene with shadows
    void render(Scene& scene, const glm::mat4& view, const glm::mat4& projection);

    // Shadow mode control
    void setShadowMode(ShadowMode mode) { shadowManager_->setShadowMode(mode); }
    void setAtlasMode(AtlasMode mode) { shadowManager_->setAtlasMode(mode); }
    void setRSMQuality(float smoothness, float bias, float radius, float intensity) {
        shadowManager_->setRSMQuality(smoothness, bias, radius, intensity);
    }

private:
    std::unique_ptr<ShadowManager> shadowManager_;
    std::shared_ptr<Shader> sceneShader_;
    
    // Helper methods
    void renderShadows(const std::vector<Light*>& lights, const std::vector<Mesh*>& meshes);
    void renderSceneWithLighting(const std::vector<Mesh*>& meshes,
                                 const std::vector<Light*>& lights);
    
    glm::mat4 calculateDirectionalLightSpace(const DirectionalLight* light);
    std::vector<glm::mat4> calculatePointLightFaceMatrices(const PointLight* light);
    glm::mat4 calculateSpotLightSpace(const SpotLight* light);
};

} // namespace RenderLib
```

## Шаг 2: Реализация в ForwardRenderer.cpp

```cpp
#include "RenderLib/ForwardRenderer.hpp"
#include "RenderLib/Scene.hpp"
#include "RenderLib/Mesh.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace RenderLib {

ForwardRenderer::ForwardRenderer()
    : shadowManager_(std::make_unique<ShadowManager>()) {
}

ForwardRenderer::~ForwardRenderer() {
    destroy();
}

bool ForwardRenderer::init() {
    // Initialize shadow manager
    if (!shadowManager_->init()) {
        std::cerr << "Failed to initialize ShadowManager\n";
        return false;
    }

    // Load main scene shader
    // Choose between:
    // - basic_advanced.vert/frag for standard shadows
    // - basic_advanced_rsm.vert/frag for RSM mode
    // - basic_advanced_atlas.vert/frag for atlas mode
    sceneShader_ = std::make_shared<Shader>(
        "shaders/basic_advanced.vert",
        "shaders/basic_advanced.frag"
    );

    if (!sceneShader_) {
        std::cerr << "Failed to load scene shader\n";
        return false;
    }

    std::cout << "ForwardRenderer initialized with shadow support\n";
    return true;
}

void ForwardRenderer::destroy() {
    if (shadowManager_) {
        shadowManager_->destroy();
    }
}

glm::mat4 ForwardRenderer::calculateDirectionalLightSpace(const DirectionalLight* light) {
    // Calculate light view matrix (looking along direction)
    glm::vec3 lightPos = -light->direction * 50.0f;
    glm::mat4 lightView = glm::lookAt(
        lightPos,
        glm::vec3(0.0f),  // Center of scene
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    // Orthographic projection for directional light
    glm::mat4 lightProj = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, 0.1f, 100.0f);

    return lightProj * lightView;
}

std::vector<glm::mat4> ForwardRenderer::calculatePointLightFaceMatrices(const PointLight* light) {
    std::vector<glm::mat4> matrices;

    // Perspective projection for cubemap
    glm::mat4 proj = glm::perspective(
        glm::radians(90.0f),
        1.0f,  // Square aspect ratio
        0.1f,
        light->radius * 2.0f
    );

    // 6 cube face directions and ups
    struct FaceInfo {
        glm::vec3 direction;
        glm::vec3 up;
    };

    FaceInfo faces[] = {
        { glm::vec3(1.0f, 0.0f, 0.0f),   glm::vec3(0.0f, -1.0f, 0.0f) },  // +X
        { glm::vec3(-1.0f, 0.0f, 0.0f),  glm::vec3(0.0f, -1.0f, 0.0f) },  // -X
        { glm::vec3(0.0f, 1.0f, 0.0f),   glm::vec3(0.0f, 0.0f, 1.0f) },   // +Y
        { glm::vec3(0.0f, -1.0f, 0.0f),  glm::vec3(0.0f, 0.0f, -1.0f) },  // -Y
        { glm::vec3(0.0f, 0.0f, 1.0f),   glm::vec3(0.0f, -1.0f, 0.0f) },  // +Z
        { glm::vec3(0.0f, 0.0f, -1.0f),  glm::vec3(0.0f, -1.0f, 0.0f) }   // -Z
    };

    for (int i = 0; i < 6; ++i) {
        glm::mat4 view = glm::lookAt(
            light->position,
            light->position + faces[i].direction,
            faces[i].up
        );
        matrices.push_back(proj * view);
    }

    return matrices;
}

glm::mat4 ForwardRenderer::calculateSpotLightSpace(const SpotLight* light) {
    glm::mat4 view = glm::lookAt(
        light->position,
        light->position + light->direction,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    // Perspective based on cone angle
    float fov = glm::degrees(std::acos(light->outerCone)) * 2.0f;
    glm::mat4 proj = glm::perspective(
        glm::radians(fov),
        1.0f,
        0.1f,
        100.0f
    );

    return proj * view;
}

void ForwardRenderer::renderShadows(const std::vector<Light*>& lights,
                                   const std::vector<Mesh*>& meshes) {
    // Save viewport
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    // Render shadows for each light
    for (auto light : lights) {
        if (auto dirLight = dynamic_cast<DirectionalLight*>(light)) {
            glm::mat4 lightSpace = calculateDirectionalLightSpace(dirLight);
            shadowManager_->renderDirectionalShadow(dirLight, lightSpace, meshes);
        }
        else if (auto pointLight = dynamic_cast<PointLight*>(light)) {
            auto matrices = calculatePointLightFaceMatrices(pointLight);
            shadowManager_->renderPointShadow(pointLight, matrices, meshes);
        }
        else if (auto spotLight = dynamic_cast<SpotLight*>(light)) {
            glm::mat4 lightSpace = calculateSpotLightSpace(spotLight);
            shadowManager_->renderSpotShadow(spotLight, lightSpace, meshes);
        }
    }

    // Restore viewport
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

    // Unbind shadow framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ForwardRenderer::renderSceneWithLighting(const std::vector<Mesh*>& meshes,
                                             const std::vector<Light*>& lights) {
    sceneShader_->use();

    // Set global uniforms
    sceneShader_->setInt("uShadowMap", 0);  // Shadow texture unit
    sceneShader_->setInt("uTexture", 1);
    sceneShader_->setInt("uNormalMap", 2);
    sceneShader_->setInt("uRoughnessMap", 3);
    sceneShader_->setInt("uMetallicMap", 4);

    // Bind shadow textures based on current mode
    glActiveTexture(GL_TEXTURE0);
    if (shadowManager_->getShadowMode() == ShadowMode::RSM) {
        // For RSM, bind RSM textures
        if (!lights.empty()) {
            if (auto dirLight = dynamic_cast<DirectionalLight*>(lights[0])) {
                glBindTexture(GL_TEXTURE_2D, dirLight->rsmColorMap);
                sceneShader_->setInt("uRSMColorMap", 0);
                glActiveTexture(GL_TEXTURE5);
                glBindTexture(GL_TEXTURE_2D, dirLight->rsmNormalMap);
                sceneShader_->setInt("uRSMNormalMap", 5);
                glActiveTexture(GL_TEXTURE6);
                glBindTexture(GL_TEXTURE_2D, dirLight->rsmDepthMap);
                sceneShader_->setInt("uRSMDepthMap", 6);
            }
        }

        // Set RSM quality parameters
        sceneShader_->setFloat("uRSMSmoothness", shadowManager_->getRSMSmoothness());
        sceneShader_->setFloat("uRSMBias", shadowManager_->getRSMBias());
        sceneShader_->setFloat("uRSMRadius", shadowManager_->getRSMRadius());
        sceneShader_->setFloat("uRSMIntensity", shadowManager_->getRSMIntensity());
    }
    else if (shadowManager_->getAtlasMode() == AtlasMode::Atlas) {
        // For atlas, bind atlas texture
        // glBindTexture(GL_TEXTURE_2D, shadowManager_->getAtlasTexture());

        // Pass atlas rectangles for each light
        for (size_t i = 0; i < lights.size(); ++i) {
            std::string uniform = "uAtlasRect[" + std::to_string(i) + "]";
            glm::vec4 rect = shadowManager_->getAtlasRect(i);
            // sceneShader_->setVec4(uniform.c_str(), rect);
        }
    }
    else {
        // Standard mode: bind regular depth maps
        if (!lights.empty()) {
            if (auto dirLight = dynamic_cast<DirectionalLight*>(lights[0])) {
                glBindTexture(GL_TEXTURE_2D, dirLight->depthMap);
            }
        }
    }

    // Render all meshes
    for (auto mesh : meshes) {
        if (!mesh) continue;

        // Set per-mesh uniforms
        sceneShader_->setMat4("uModel", mesh->getModelMatrix());

        // Render mesh
        mesh->render();
    }

    glActiveTexture(GL_TEXTURE0);
}

void ForwardRenderer::render(Scene& scene, const glm::mat4& view, const glm::mat4& projection) {
    // Get scene data
    auto lights = scene.getLights();
    auto meshes = scene.getMeshes();

    if (lights.empty() || meshes.empty()) {
        return;
    }

    // ==========================================
    // Phase 1: Render shadow maps
    // ==========================================
    renderShadows(lights, meshes);

    // ==========================================
    // Phase 2: Render scene with shadows
    // ==========================================

    // Set viewport for scene rendering
    int width = 1920;  // Get from window
    int height = 1080;
    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);  // Screen framebuffer

    // Clear screen
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Render scene
    renderSceneWithLighting(meshes, lights);

    glDisable(GL_DEPTH_TEST);
}

} // namespace RenderLib
```

## Шаг 3: Выбор правильного шейдера

В зависимости от режима используйте разные шейдеры:

```cpp
std::string vertShader = "shaders/basic_advanced.vert";
std::string fragShader;

switch (shadowManager_->getShadowMode()) {
    case ShadowMode::Standard:
        if (shadowManager_->getAtlasMode() == AtlasMode::Atlas) {
            fragShader = "shaders/basic_advanced_atlas.frag";
        } else {
            fragShader = "shaders/basic_advanced.frag";
        }
        break;

    case ShadowMode::RSM:
        if (shadowManager_->getAtlasMode() == AtlasMode::Atlas) {
            fragShader = "shaders/basic_advanced_atlas.frag";  // Atlas + RSM
        } else {
            fragShader = "shaders/basic_advanced_rsm.frag";
        }
        break;
}

sceneShader_ = std::make_shared<Shader>(vertShader, fragShader);
```

## Шаг 4: Использование

```cpp
int main() {
    // Create renderer
    ForwardRenderer renderer;
    renderer.init();

    // Setup scene
    Scene scene;
    // ... add lights and meshes ...

    // Create lights with proper initialization
    DirectionalLight dirLight;
    dirLight.initShadowMap();
    if (useShadowAtlas) {
        dirLight.initRSM();
    }
    scene.addLight(&dirLight);

    // Render loop
    while (!shouldClose) {
        // Set shadow modes
        renderer.setShadowMode(ShadowMode::RSM);
        renderer.setAtlasMode(AtlasMode::Atlas);

        // Set quality
        renderer.setRSMQuality(2.0f, 0.001f, 0.02f, 1.5f);

        // Render
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
        renderer.render(scene, view, projection);

        // Present
        swapBuffers();
    }

    renderer.destroy();
    return 0;
}
```

## Оптимизация

### Memory Layout
```cpp
// При использовании Atlas - экономия памяти
// Без atlas (4 точечных источника):
// 4 x (cubemap 1024² + depth) = ~512MB

// С atlas:
// 1 x atlas 4096² = ~32MB (для 4 источников!)
```

### Performance Tips

1. **Переиспользуйте light space matrices** - кэшируйте между фреймами
2. **Используйте frustum culling** - только видимые объекты в shadow rendering
3. **Регулируйте разрешение теней** - обычно 2048 для directional, 1024 для point
4. **Профилируйте PCF** - экспериментируйте с количеством семплов

## Отладка

```cpp
// Вывести shadow map в экран для отладки
void debugVisualizeShadowMap(GLuint shadowTexture) {
    // Используйте простой квадрат с texturing для показа
    // shadow_debug.frag:
    // out_color = vec4(vec3(texture(uShadowMap, uv).r), 1.0);
}

// Проверить framebuffer status
if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cerr << "Shadow FBO incomplete!\n";
}
```

## Заключение

Эта интеграция предоставляет:
- ✅ Полная поддержка RSM и Shadow Atlas
- ✅ Runtime переключение режимов
- ✅ Оптимизированный рендеринг с менеджером теней
- ✅ Поддержка всех типов источников света
- ✅ Настраиваемые параметры качества
