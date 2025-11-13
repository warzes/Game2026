#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>

namespace RenderLib {

class Shader;
class DirectionalLight;
class PointLight;
class SpotLight;

// Shadow rendering modes
enum class ShadowMode {
    Standard,       // Traditional depth PCF
    RSM,           // Reflective Shadow Maps (color + normal)
};

enum class AtlasMode {
    Individual,    // Separate shadow maps
    Atlas,         // Single texture atlas
};

/**
 * @brief Shadow rendering manager with RSM and Atlas support
 * 
 * Manages shadow map generation with:
 * - Standard (depth-only) mode
 * - RSM (color + normal) mode
 * - Individual textures or shadow atlas
 */
class ShadowManager {
public:
    ShadowManager();
    ~ShadowManager();

    // Initialize shadow rendering
    bool init();
    void destroy();

    // Set shadow modes
    void setShadowMode(ShadowMode mode) { shadowMode_ = mode; }
    void setAtlasMode(AtlasMode mode) { atlasMode_ = mode; }

    ShadowMode getShadowMode() const { return shadowMode_; }
    AtlasMode getAtlasMode() const { return atlasMode_; }

    // Render directional light shadow
    void renderDirectionalShadow(DirectionalLight* light, const glm::mat4& lightSpace,
                                const std::vector<Mesh*>& meshes);

    // Render point light shadow
    void renderPointShadow(PointLight* light, const std::vector<glm::mat4>& faceMatrices,
                          const std::vector<Mesh*>& meshes);

    // Render spot light shadow
    void renderSpotShadow(SpotLight* light, const glm::mat4& lightSpace,
                         const std::vector<Mesh*>& meshes);

    // Shadow atlas management
    void allocateAtlasSpace(uint32_t lightCount, uint32_t resolution);
    glm::vec4 getAtlasRect(uint32_t lightIndex) const;
    
    // RSM quality parameters
    void setRSMQuality(float smoothness, float bias, float radius, float intensity) {
        rsmSmoothness_ = smoothness;
        rsmBias_ = bias;
        rsmRadius_ = radius;
        rsmIntensity_ = intensity;
    }
    
    float getRSMSmoothness() const { return rsmSmoothness_; }
    float getRSMBias() const { return rsmBias_; }
    float getRSMRadius() const { return rsmRadius_; }
    float getRSMIntensity() const { return rsmIntensity_; }

private:
    ShadowMode shadowMode_ = ShadowMode::Standard;
    AtlasMode atlasMode_ = AtlasMode::Individual;
    
    // RSM quality parameters
    float rsmSmoothness_ = 1.0f;
    float rsmBias_ = 0.001f;
    float rsmRadius_ = 0.02f;
    float rsmIntensity_ = 1.0f;

    // Shadow atlas
    GLuint shadowAtlasFBO_ = 0;
    GLuint shadowAtlasTexture_ = 0;
    GLuint shadowAtlasColorTexture_ = 0;  // For RSM color
    uint32_t atlasTilesX_ = 0;
    uint32_t atlasTilesY_ = 0;
    uint32_t atlasResolution_ = 2048;

    // Shaders
    std::shared_ptr<Shader> standardShadowShader_;
    std::shared_ptr<Shader> rsmShadowShader_;
    std::shared_ptr<Shader> atlasShadowShader_;

    // Internal rendering
    void renderToStandardShadow_(GLuint targetFBO, const glm::mat4& lightSpace,
                               const std::vector<Mesh*>& meshes);
    void renderToRSM_(GLuint targetFBO, const glm::mat4& lightSpace,
                    const std::vector<Mesh*>& meshes);
    void renderToAtlas_(uint32_t lightIndex, const glm::mat4& lightSpace,
                       const std::vector<Mesh*>& meshes);

    bool createAtlas_();
};

}  // namespace RenderLib
