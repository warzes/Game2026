#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <array>

namespace RenderLib {

class CascadeShadowMap {
public:
    CascadeShadowMap();
    ~CascadeShadowMap();

    bool init(int resolution = 2048, int cascadeCount = 3);
    void destroy();

    // Update cascade matrices based on view/proj matrices and split distances
    void updateMatrices(const glm::mat4& view, const glm::mat4& proj, 
                        const std::array<float, 4>& cascadeSplits,  // split distances in depth
                        const glm::vec3& lightDir);

    GLuint cascadeFBO(int cascade) const { return cascadeFBOs_[cascade]; }
    GLuint cascadeDepthMap(int cascade) const { return cascadeDepthMaps_[cascade]; }
    
    glm::mat4 cascadeMatrix(int cascade) const { return cascadeMatrices_[cascade]; }
    
    int cascadeCount() const { return cascadeCount_; }
    int resolution() const { return resolution_; }

private:
    int resolution_{2048};
    int cascadeCount_{3};
    
    std::array<GLuint, 4> cascadeFBOs_{0, 0, 0, 0};
    std::array<GLuint, 4> cascadeDepthMaps_{0, 0, 0, 0};
    std::array<glm::mat4, 4> cascadeMatrices_;
};

} // namespace RenderLib
