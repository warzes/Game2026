#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

namespace RenderLib {

// Forward declare enums from LightDirectional.hpp
enum class ShadowMode;
enum class AtlasMode;

struct PointLight {
    glm::vec3 position{0.0f};
    glm::vec3 color{1.0f};
    float intensity{1.0f};
    float radius{10.0f};

    // shadow: cubemap
    GLuint depthCubeFBO{0};
    GLuint depthCubeMap{0};
    int shadowResolution{1024};
    
    // RSM support
    GLuint rsmCubeFBO{0};
    GLuint rsmCubeMap{0};         // RSM cubemap
    GLuint rsmColorCube{0};       // Color cubemap
    bool useRSM{false};
    
    // Atlas support
    bool useAtlas{false};
    glm::vec4 atlasRect{0.0f};

    void initShadowCube();
    void initRSM();
    void resizeShadow(int resolution);
    void resizeRSM(int resolution);
    void destroyRSM();
};

} // namespace RenderLib
