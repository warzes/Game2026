#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace RenderLib {

// Shadow rendering modes
enum class ShadowMode {
    Standard,       // Traditional depth comparison PCF
    RSM,           // Reflective Shadow Maps (color + normal)
};

enum class AtlasMode {
    Individual,    // Separate shadow maps (current method)
    Atlas,         // Single atlas texture with all lights
};

struct DirectionalLight {
    glm::vec3 direction{0.0f, -1.0f, 0.0f};
    glm::vec3 color{1.0f};
    float intensity{1.0f};

    // shadow map
    GLuint depthMapFBO{0};
    GLuint depthMap{0};
    int shadowResolution{2048};
    
    // RSM support (Reflective Shadow Maps)
    GLuint rsmFBO{0};
    GLuint rsmNormalMap{0};       // Normal map from light space
    GLuint rsmColorMap{0};        // Color information
    GLuint rsmDepthMap{0};        // Depth for indirect light
    bool useRSM{false};           // Enable RSM
    
    // RSM parameters for quality control
    float rsmSmoothness{1.0f};    // Smoothing radius for RSM (1-3 recommended)
    float rsmBias{0.001f};        // Depth bias to reduce shadow acne
    float rsmRadius{0.02f};       // Sample radius for indirect lighting
    float rsmIntensity{1.0f};     // Indirect lighting contribution intensity
    
    // Atlas support
    bool useAtlas{false};         // Enable shadow atlas
    glm::vec4 atlasRect{0.0f};    // Atlas UV rect (x, y, width, height)

    void initShadowMap();
    void initRSM();              // Initialize RSM textures
    void resizeShadow(int resolution);
    void resizeRSM(int resolution);
    
    // Cleanup
    void destroyRSM();
};

} // namespace RenderLib
