#include "RenderLib/ShadowManager.hpp"
#include "RenderLib/Renderer.hpp"
#include "RenderLib/Shader.hpp"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>

/**
 * @brief Example demonstrating RSM and Shadow Atlas modes
 * 
 * Press keys to toggle between shadow rendering modes:
 * - '1': Standard shadow mapping (depth-only PCF)
 * - '2': Reflective Shadow Maps (RSM with color + normal)
 * 
 * Press keys for atlas mode:
 * - 'I': Individual shadow maps (separate per light)
 * - 'A': Shadow atlas (consolidated texture)
 */

class ShadowModeExample {
public:
    ShadowModeExample()
        : window_(nullptr), renderer_(nullptr), shadowManager_(nullptr) {}

    bool init() {
        // Initialize window and renderer
        if (!glfwInit()) {
            std::cerr << "GLFW initialization failed\n";
            return false;
        }

        window_ = glfwCreateWindow(1280, 720, "RenderLib - Shadow Modes", nullptr, nullptr);
        if (!window_) {
            std::cerr << "Window creation failed\n";
            glfwTerminate();
            return false;
        }

        glfwMakeContextCurrent(window_);
        glfwSetWindowUserPointer(window_, this);
        glfwSetKeyCallback(window_, keyCallback);

        // Initialize renderer
        renderer_ = std::make_unique<Renderer>();
        if (!renderer_->init(1280, 720)) {
            std::cerr << "Renderer initialization failed\n";
            return false;
        }

        // Initialize shadow manager
        shadowManager_ = std::make_unique<ShadowManager>();
        if (!shadowManager_->init()) {
            std::cerr << "ShadowManager initialization failed\n";
            return false;
        }

        // Allocate shadow atlas for 4 lights
        shadowManager_->allocateAtlasSpace(4, 1024);

        std::cout << "\n=== RenderLib Shadow Modes Example ===\n";
        std::cout << "Controls:\n";
        std::cout << "  '1' - Standard shadow mapping (PCF depth-only)\n";
        std::cout << "  '2' - Reflective Shadow Maps (RSM with color+normal)\n";
        std::cout << "  'I' - Individual shadow maps mode\n";
        std::cout << "  'A' - Shadow atlas consolidated mode\n";
        std::cout << "  'ESC' - Exit\n\n";

        updateModeDisplay();
        return true;
    }

    void run() {
        while (!glfwWindowShouldClose(window_)) {
            handleInput();
            update();
            render();
            glfwSwapBuffers(window_);
            glfwPollEvents();
        }
    }

    void cleanup() {
        shadowManager_.reset();
        renderer_.reset();
        if (window_) {
            glfwDestroyWindow(window_);
            window_ = nullptr;
        }
        glfwTerminate();
    }

private:
    GLFWwindow* window_;
    std::unique_ptr<Renderer> renderer_;
    std::unique_ptr<ShadowManager> shadowManager_;

    void handleInput() {
        if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window_, true);
        }
    }

    void update() {
        // Update scene
        renderer_->update(0.016f);  // ~60 FPS
    }

    void render() {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render shadow maps based on current mode
        auto meshes = renderer_->getMeshes();

        // Example: Render directional light shadow
        auto dirLight = renderer_->getDirectionalLight();
        if (dirLight) {
            glm::mat4 lightSpace = glm::ortho(-50.f, 50.f, -50.f, 50.f, 0.1f, 100.f) *
                                  glm::lookAt(glm::vec3(10.f, 20.f, 10.f),
                                            glm::vec3(0.f),
                                            glm::vec3(0.f, 1.f, 0.f));

            shadowManager_->renderDirectionalShadow(dirLight, lightSpace, meshes);
        }

        // Render scene with appropriate shadow shader
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, 1280, 720);

        Shader* shadowShader = nullptr;
        switch (shadowManager_->getShadowMode()) {
            case ShadowMode::Standard:
                shadowShader = renderer_->getShader("shadow_standard");
                break;
            case ShadowMode::RSM:
                shadowShader = renderer_->getShader("shadow_rsm");
                break;
        }

        if (shadowShader) {
            shadowShader->use();
            for (auto mesh : meshes) {
                mesh->render();
            }
        }
    }

    static void keyCallback(GLFWwindow* window, int key, int scancode,
                           int action, int mods) {
        auto* self = static_cast<ShadowModeExample*>(glfwGetWindowUserPointer(window));
        if (action != GLFW_PRESS) return;

        switch (key) {
            case GLFW_KEY_1:
                self->shadowManager_->setShadowMode(ShadowMode::Standard);
                self->updateModeDisplay();
                break;

            case GLFW_KEY_2:
                self->shadowManager_->setShadowMode(ShadowMode::RSM);
                self->updateModeDisplay();
                break;

            case GLFW_KEY_I:
                self->shadowManager_->setAtlasMode(AtlasMode::Individual);
                self->updateModeDisplay();
                break;

            case GLFW_KEY_A:
                self->shadowManager_->setAtlasMode(AtlasMode::Atlas);
                self->updateModeDisplay();
                break;

            default:
                break;
        }
    }

    void updateModeDisplay() {
        const char* shadowMode = (shadowManager_->getShadowMode() == ShadowMode::Standard)
                                    ? "Standard PCF"
                                    : "Reflective Shadow Maps (RSM)";
        const char* atlasMode = (shadowManager_->getAtlasMode() == AtlasMode::Individual)
                                   ? "Individual maps"
                                   : "Shadow atlas";

        std::cout << "Shadow Mode: " << shadowMode << " | Atlas: " << atlasMode << "\n";
    }
};

int main() {
    ShadowModeExample example;

    if (!example.init()) {
        std::cerr << "Failed to initialize example\n";
        return -1;
    }

    example.run();
    example.cleanup();

    return 0;
}
