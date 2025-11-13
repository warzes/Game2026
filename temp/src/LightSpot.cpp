#include "RenderLib/LightSpot.hpp"
#include <iostream>

namespace RenderLib {

void SpotLight::initShadowMap() {
    if(depthMapFBO) return;
    glGenFramebuffers(1, &depthMapFBO);
    glGenTextures(1, &depthMap);

    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, shadowResolution, shadowResolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Shadow FBO incomplete for spot light\n";
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SpotLight::resizeShadow(int resolution) {
    if(resolution == shadowResolution) return;
    shadowResolution = resolution;
    if(depthMap) glDeleteTextures(1, &depthMap);
    if(depthMapFBO) glDeleteFramebuffers(1, &depthMapFBO);
    depthMap = 0; depthMapFBO = 0;
    initShadowMap();
}

void SpotLight::initRSM() {
    if(rsmFBO) return;
    
    glGenFramebuffers(1, &rsmFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, rsmFBO);
    
    // Create color texture
    glGenTextures(1, &rsmColorMap);
    glBindTexture(GL_TEXTURE_2D, rsmColorMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, shadowResolution, shadowResolution, 0,
                 GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                          rsmColorMap, 0);
    
    // Create normal texture
    glGenTextures(1, &rsmNormalMap);
    glBindTexture(GL_TEXTURE_2D, rsmNormalMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, shadowResolution, shadowResolution, 0,
                 GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
                          rsmNormalMap, 0);
    
    // Create depth texture
    glGenTextures(1, &depthMapRSM);
    glBindTexture(GL_TEXTURE_2D, depthMapRSM);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, shadowResolution, shadowResolution, 0,
                 GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                          depthMapRSM, 0);
    
    GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        glDeleteFramebuffers(1, &rsmFBO);
        rsmFBO = 0;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SpotLight::resizeRSM(int resolution) {
    if(resolution == shadowResolution) return;
    shadowResolution = resolution;
    destroyRSM();
    initRSM();
}

void SpotLight::destroyRSM() {
    if (rsmFBO != 0) {
        glDeleteFramebuffers(1, &rsmFBO);
        rsmFBO = 0;
    }
    if (rsmColorMap != 0) {
        glDeleteTextures(1, &rsmColorMap);
        rsmColorMap = 0;
    }
    if (rsmNormalMap != 0) {
        glDeleteTextures(1, &rsmNormalMap);
        rsmNormalMap = 0;
    }
    if (depthMapRSM != 0) {
        glDeleteTextures(1, &depthMapRSM);
        depthMapRSM = 0;
    }
}

} // namespace RenderLib
