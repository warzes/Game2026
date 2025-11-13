#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace RenderLib {

// Simple parallax mapping without self-shadowing
// Uses heightmap from normal texture's alpha channel
struct ParallaxSettings {
    float heightScale{0.1f};
    int minLayers{8};
    int maxLayers{32};
};

// Variance shadow mapping data: stores mean and mean-of-squares
struct VarianceShadowMap {
    GLuint fbo{0};
    GLuint colorTex{0};  // RG32F: (depth, depth^2)
    int resolution{2048};
    
    void create(int res);
    void destroy();
};

} // namespace RenderLib
