#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace RenderLib {

// Forward declare enums
enum class ShadowMode;
enum class AtlasMode;

struct SpotLight {
    glm::vec3 position{0.0f};
    glm::vec3 direction{0.0f, -1.0f, 0.0f};
    glm::vec3 color{1.0f};
    float intensity{1.0f};
    float innerCone{0.9f}, outerCone{0.75f};

    GLuint depthMapFBO{0};
    GLuint depthMap{0};
    int shadowResolution{2048};
    
    // RSM support
    GLuint rsmFBO{0};
    GLuint rsmNormalMap{0};
    GLuint rsmColorMap{0};
    bool useRSM{false};
    
    // RSM parameters for quality control
    float rsmSmoothness{1.0f};    // Smoothing radius for RSM
    float rsmBias{0.001f};        // Depth bias to reduce shadow acne
    float rsmRadius{0.02f};       // Sample radius for indirect lighting
    float rsmIntensity{1.0f};     // Indirect lighting contribution intensity
    
    // Atlas support
    bool useAtlas{false};
    glm::vec4 atlasRect{0.0f};

    void initShadowMap();
    void initRSM();
    void resizeShadow(int resolution);
    void resizeRSM(int resolution);
    void destroyRSM();
};

} // namespace RenderLib
