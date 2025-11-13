#include "RenderLib/CascadeShadow.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace RenderLib {

CascadeShadowMap::CascadeShadowMap() {}

CascadeShadowMap::~CascadeShadowMap() { destroy(); }

bool CascadeShadowMap::init(int resolution, int cascadeCount) {
    resolution_ = resolution;
    cascadeCount_ = glm::min(cascadeCount, 4);

    for(int c = 0; c < cascadeCount_; ++c) {
        glGenFramebuffers(1, &cascadeFBOs_[c]);
        glGenTextures(1, &cascadeDepthMaps_[c]);

        glBindTexture(GL_TEXTURE_2D, cascadeDepthMaps_[c]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, resolution, resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

        glBindFramebuffer(GL_FRAMEBUFFER, cascadeFBOs_[c]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, cascadeDepthMaps_[c], 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "Cascade shadow FBO " << c << " incomplete\n";
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            return false;
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

void CascadeShadowMap::destroy() {
    for(int c = 0; c < 4; ++c) {
        if(cascadeDepthMaps_[c]) { glDeleteTextures(1, &cascadeDepthMaps_[c]); cascadeDepthMaps_[c] = 0; }
        if(cascadeFBOs_[c]) { glDeleteFramebuffers(1, &cascadeFBOs_[c]); cascadeFBOs_[c] = 0; }
    }
}

void CascadeShadowMap::updateMatrices(const glm::mat4& view, const glm::mat4& proj, 
                                     const std::array<float, 4>& cascadeSplits,
                                     const glm::vec3& lightDir) {
    // Extract frustum corners in world space for each cascade
    glm::vec3 lightDir_normalized = glm::normalize(lightDir);
    
    for(int c = 0; c < cascadeCount_; ++c) {
        // Approximate: create ortho light-space matrix for each cascade
        // This is simplified; production would compute tighter bounds from frustum corners
        
        float near = cascadeSplits[c];
        float far = cascadeSplits[c + 1];
        float range = far - near;
        
        // Simple approach: create a view matrix along light direction
        glm::vec3 lightPos = glm::vec3(10.0f, 10.0f, 10.0f);  // arbitrary; should be computed from scene bounds
        glm::mat4 lightView = glm::lookAt(lightPos, lightPos - lightDir_normalized, glm::vec3(0.0f, 1.0f, 0.0f));
        
        // Orthogonal projection for this cascade range
        glm::mat4 lightProj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near, far);
        
        cascadeMatrices_[c] = lightProj * lightView;
    }
}

} // namespace RenderLib
