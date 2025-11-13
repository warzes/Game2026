#include "RenderLib/ShadowManager.hpp"
#include "RenderLib/LightDirectional.hpp"
#include "RenderLib/LightPoint.hpp"
#include "RenderLib/LightSpot.hpp"
#include "RenderLib/ShadowModeHelper.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace RenderLib {

/**
 * @brief Advanced example demonstrating full RSM and Shadow Atlas capabilities
 * 
 * This example shows:
 * - Runtime switching between Standard and RSM modes
 * - Runtime switching between Individual and Atlas modes
 * - Quality parameter adjustment
 * - Proper temporal filtering setup
 * - Integration with scene rendering
 */
class AdvancedShadowExample {
public:
    AdvancedShadowExample()
        : shadowManager_(std::make_unique<ShadowManager>()),
          modeHelper_(shadowManager_.get()),
          currentMode_(ShadowMode::Standard),
          currentAtlasMode_(AtlasMode::Individual) {
    }

    /**
     * Initialize shadow rendering system
     */
    bool initialize() {
        if (!shadowManager_->init()) {
            std::cerr << "Failed to initialize ShadowManager\n";
            return false;
        }

        // Setup initial quality parameters
        shadowManager_->setRSMQuality(
            1.5f,   // smoothness
            0.001f, // bias
            0.02f,  // radius
            1.0f    // intensity
        );

        std::cout << "Shadow system initialized successfully\n";
        return true;
    }

    /**
     * Create example lights for testing
     */
    void createTestLights() {
        // Directional light (sun)
        DirectionalLight dirLight;
        dirLight.direction = glm::normalize(glm::vec3(1.0f, -1.0f, 1.0f));
        dirLight.color = glm::vec3(1.0f, 0.95f, 0.8f);
        dirLight.intensity = 1.2f;
        dirLight.shadowResolution = 2048;
        dirLight.initShadowMap();
        dirLights_.push_back(dirLight);

        // Point lights (multiple for atlas testing)
        for (int i = 0; i < 4; ++i) {
            PointLight pointLight;
            pointLight.position = glm::vec3(
                -10.0f + i * 8.0f,
                5.0f,
                0.0f
            );
            pointLight.color = glm::vec3(
                0.5f + i * 0.1f,
                0.2f,
                0.8f - i * 0.1f
            );
            pointLight.intensity = 1.0f;
            pointLight.radius = 20.0f;
            pointLight.shadowResolution = 1024;
            pointLight.initShadowCube();
            pointLights_.push_back(pointLight);
        }

        // Spot lights
        SpotLight spotLight;
        spotLight.position = glm::vec3(0.0f, 10.0f, -10.0f);
        spotLight.direction = glm::normalize(glm::vec3(0.0f, -1.0f, 0.5f));
        spotLight.color = glm::vec3(1.0f, 1.0f, 0.5f);
        spotLight.intensity = 1.5f;
        spotLight.shadowResolution = 2048;
        spotLight.innerCone = 0.95f;
        spotLight.outerCone = 0.85f;
        spotLight.initShadowMap();
        spotLights_.push_back(spotLight);

        std::cout << "Created " << dirLights_.size() << " directional, "
                  << pointLights_.size() << " point, "
                  << spotLights_.size() << " spot lights\n";
    }

    /**
     * Switch shadow mode at runtime
     */
    void setShadowMode(ShadowMode mode) {
        if (currentMode_ == mode) return;

        currentMode_ = mode;
        shadowManager_->setShadowMode(mode);

        std::string modeName = (mode == ShadowMode::Standard) ? "Standard" : "RSM";
        std::cout << "Switched to shadow mode: " << modeName << "\n";

        // Initialize RSM for all lights if switching to RSM
        if (mode == ShadowMode::RSM) {
            for (auto& light : dirLights_) {
                if (!light.rsmFBO) {
                    light.initRSM();
                }
            }
            for (auto& light : pointLights_) {
                if (!light.rsmCubeFBO) {
                    light.initRSM();
                }
            }
            for (auto& light : spotLights_) {
                if (!light.rsmFBO) {
                    light.initRSM();
                }
            }
        }
    }

    /**
     * Switch atlas mode at runtime
     */
    void setAtlasMode(AtlasMode mode) {
        if (currentAtlasMode_ == mode) return;

        currentAtlasMode_ = mode;
        shadowManager_->setAtlasMode(mode);

        std::string modeName = (mode == AtlasMode::Individual) ? "Individual" : "Atlas";
        std::cout << "Switched to atlas mode: " << modeName << "\n";

        if (mode == AtlasMode::Atlas) {
            // Allocate space for all lights
            uint32_t totalLights = dirLights_.size() + pointLights_.size() + spotLights_.size();
            shadowManager_->allocateAtlasSpace(totalLights, 2048);
        }
    }

    /**
     * Set RSM quality parameters
     */
    void setRSMQuality(float smoothness, float bias, float radius, float intensity) {
        shadowManager_->setRSMQuality(smoothness, bias, radius, intensity);
        std::cout << "RSM quality set: smoothness=" << smoothness 
                  << ", bias=" << bias
                  << ", radius=" << radius 
                  << ", intensity=" << intensity << "\n";
    }

    /**
     * Preset quality levels for convenience
     */
    void setQualityPreset(const std::string& preset) {
        if (preset == "low") {
            setRSMQuality(1.0f, 0.002f, 0.015f, 0.5f);
        } else if (preset == "medium") {
            setRSMQuality(1.5f, 0.001f, 0.02f, 1.0f);
        } else if (preset == "high") {
            setRSMQuality(2.0f, 0.0005f, 0.025f, 1.5f);
        } else if (preset == "ultra") {
            setRSMQuality(3.0f, 0.0002f, 0.03f, 2.0f);
        } else {
            std::cerr << "Unknown preset: " << preset << "\n";
        }
    }

    /**
     * Main rendering loop with shadow rendering
     * This demonstrates how to integrate ShadowManager with scene rendering
     */
    void render(const std::vector<Mesh*>& meshes) {
        // 1. Render shadows for all lights
        renderShadows(meshes);

        // 2. Render scene with lighting
        renderScene(meshes);
    }

    /**
     * Render all shadow maps
     */
    void renderShadows(const std::vector<Mesh*>& meshes) {
        // Render directional light shadows
        uint32_t dirLightIdx = 0;
        for (auto& light : dirLights_) {
            glm::mat4 lightView = glm::lookAt(
                -light.direction * 50.0f,  // Position far along direction
                glm::vec3(0.0f),           // Target at origin
                glm::vec3(0.0f, 1.0f, 0.0f) // Up vector
            );
            glm::mat4 lightProj = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, 0.1f, 100.0f);
            glm::mat4 lightSpace = lightProj * lightView;

            shadowManager_->renderDirectionalShadow(&light, lightSpace, meshes);
            dirLightIdx++;
        }

        // Render point light shadows
        uint32_t pointLightIdx = 0;
        for (auto& light : pointLights_) {
            glm::vec3 pos = light.position;
            std::vector<glm::mat4> faceMatrices;

            // Create matrices for 6 cubemap faces
            glm::mat4 projection = glm::perspective(
                glm::radians(90.0f),
                1.0f,  // Aspect ratio
                0.1f,  // Near
                light.radius * 2.0f  // Far
            );

            // +X, -X, +Y, -Y, +Z, -Z
            glm::vec3 directions[] = {
                glm::vec3(1.0f, 0.0f, 0.0f),   // +X
                glm::vec3(-1.0f, 0.0f, 0.0f),  // -X
                glm::vec3(0.0f, 1.0f, 0.0f),   // +Y
                glm::vec3(0.0f, -1.0f, 0.0f),  // -Y
                glm::vec3(0.0f, 0.0f, 1.0f),   // +Z
                glm::vec3(0.0f, 0.0f, -1.0f)   // -Z
            };

            glm::vec3 ups[] = {
                glm::vec3(0.0f, -1.0f, 0.0f),  // +X
                glm::vec3(0.0f, -1.0f, 0.0f),  // -X
                glm::vec3(0.0f, 0.0f, 1.0f),   // +Y
                glm::vec3(0.0f, 0.0f, -1.0f),  // -Y
                glm::vec3(0.0f, -1.0f, 0.0f),  // +Z
                glm::vec3(0.0f, -1.0f, 0.0f)   // -Z
            };

            for (int i = 0; i < 6; ++i) {
                glm::mat4 view = glm::lookAt(pos, pos + directions[i], ups[i]);
                faceMatrices.push_back(projection * view);
            }

            shadowManager_->renderPointShadow(&light, faceMatrices, meshes);
            pointLightIdx++;
        }

        // Render spot light shadows
        uint32_t spotLightIdx = 0;
        for (auto& light : spotLights_) {
            glm::mat4 lightView = glm::lookAt(
                light.position,
                light.position + light.direction,
                glm::vec3(0.0f, 1.0f, 0.0f)
            );
            glm::mat4 lightProj = glm::perspective(
                glm::radians(45.0f),
                1.0f,
                0.1f,
                100.0f
            );
            glm::mat4 lightSpace = lightProj * lightView;

            shadowManager_->renderSpotShadow(&light, lightSpace, meshes);
            spotLightIdx++;
        }
    }

    /**
     * Render scene with lighting
     * This is where shaders use the shadow maps
     */
    void renderScene(const std::vector<Mesh*>& meshes) {
        // In a real implementation, this would:
        // 1. Bind appropriate shader (basic_advanced_rsm.frag or basic_advanced_atlas.frag)
        // 2. Bind shadow textures (depthMap/rsmColorMap/etc or shadowAtlas)
        // 3. Pass uniform parameters (atlasRect, rsmQuality params, etc)
        // 4. Render all meshes with proper lighting

        std::cout << "Rendering scene with " << meshes.size() << " meshes\n";

        // Example: Bind shadow textures
        if (currentMode_ == ShadowMode::RSM) {
            // Bind RSM textures
            for (size_t i = 0; i < dirLights_.size(); ++i) {
                // shader->setInt("uRSMColorMap", i);
                // glActiveTexture(GL_TEXTURE0 + i);
                // glBindTexture(GL_TEXTURE_2D, dirLights_[i].rsmColorMap);
            }
        } else {
            // Bind standard depth maps
            for (size_t i = 0; i < dirLights_.size(); ++i) {
                // shader->setInt("uShadowMap", i);
                // glActiveTexture(GL_TEXTURE0 + i);
                // glBindTexture(GL_TEXTURE_2D, dirLights_[i].depthMap);
            }
        }

        // Render meshes...
    }

    /**
     * Print current configuration
     */
    void printStatus() const {
        std::cout << "\n=== Shadow System Status ===\n";
        std::cout << "Shadow Mode: " 
                  << (currentMode_ == ShadowMode::Standard ? "Standard" : "RSM") << "\n";
        std::cout << "Atlas Mode: "
                  << (currentAtlasMode_ == AtlasMode::Individual ? "Individual" : "Atlas") << "\n";
        std::cout << "Directional lights: " << dirLights_.size() << "\n";
        std::cout << "Point lights: " << pointLights_.size() << "\n";
        std::cout << "Spot lights: " << spotLights_.size() << "\n";
        
        std::cout << "RSM Quality:\n";
        std::cout << "  Smoothness: " << shadowManager_->getRSMSmoothness() << "\n";
        std::cout << "  Bias: " << shadowManager_->getRSMBias() << "\n";
        std::cout << "  Radius: " << shadowManager_->getRSMRadius() << "\n";
        std::cout << "  Intensity: " << shadowManager_->getRSMIntensity() << "\n";
        std::cout << "\n";
    }

    /**
     * Cleanup resources
     */
    void cleanup() {
        if (shadowManager_) {
            shadowManager_->destroy();
        }

        // Cleanup lights
        for (auto& light : dirLights_) {
            light.destroyRSM();
        }
        for (auto& light : pointLights_) {
            light.destroyRSM();
        }
        for (auto& light : spotLights_) {
            light.destroyRSM();
        }
    }

private:
    std::unique_ptr<ShadowManager> shadowManager_;
    ShadowModeHelper modeHelper_;
    ShadowMode currentMode_;
    AtlasMode currentAtlasMode_;

    std::vector<DirectionalLight> dirLights_;
    std::vector<PointLight> pointLights_;
    std::vector<SpotLight> spotLights_;
};

} // namespace RenderLib

/**
 * Example usage
 */
void exampleUsage() {
    using namespace RenderLib;

    // Create example system
    AdvancedShadowExample shadowSystem;
    if (!shadowSystem.initialize()) {
        return;
    }

    // Create test lights
    shadowSystem.createTestLights();
    shadowSystem.printStatus();

    // Example 1: Use standard shadow mode with individual textures
    shadowSystem.setShadowMode(ShadowMode::Standard);
    shadowSystem.setAtlasMode(AtlasMode::Individual);

    // Example 2: Switch to RSM with high quality
    shadowSystem.setShadowMode(ShadowMode::RSM);
    shadowSystem.setQualityPreset("high");
    shadowSystem.printStatus();

    // Example 3: Switch to atlas for optimization
    shadowSystem.setAtlasMode(AtlasMode::Atlas);
    shadowSystem.printStatus();

    // Example 4: Fine-tune RSM parameters
    shadowSystem.setRSMQuality(2.5f, 0.0008f, 0.028f, 1.8f);
    shadowSystem.printStatus();

    // In real usage, in render loop:
    // std::vector<Mesh*> meshes = scene.getMeshes();
    // shadowSystem.render(meshes);

    // Cleanup
    shadowSystem.cleanup();
}
