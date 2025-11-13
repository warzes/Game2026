#include "RenderLib/LightDirectional.hpp"
#include <iostream>

namespace RenderLib {

void DirectionalLight::initShadowMap() {
    if(depthMapFBO) return; // already initialized
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
        std::cerr << "Shadow FBO incomplete for directional light\n";
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DirectionalLight::resizeShadow(int resolution) {
    if(resolution == shadowResolution) return;
    shadowResolution = resolution;
    if(depthMap) glDeleteTextures(1, &depthMap);
    if(depthMapFBO) glDeleteFramebuffers(1, &depthMapFBO);
    depthMap = 0; depthMapFBO = 0;
    initShadowMap();
}

void DirectionalLight::initRSM() {
    if(rsmFBO) return;  // Already initialized
    
    glGenFramebuffers(1, &rsmFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, rsmFBO);
    
    // Normal map (RGB = normal, A = unused)
    glGenTextures(1, &rsmNormalMap);
    glBindTexture(GL_TEXTURE_2D, rsmNormalMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, shadowResolution, shadowResolution, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rsmNormalMap, 0);
    
    // Color map (RGB = emitted color)
    glGenTextures(1, &rsmColorMap);
    glBindTexture(GL_TEXTURE_2D, rsmColorMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, shadowResolution, shadowResolution, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, rsmColorMap, 0);
    
    // Depth map
    glGenTextures(1, &rsmDepthMap);
    glBindTexture(GL_TEXTURE_2D, rsmDepthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, shadowResolution, shadowResolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, rsmDepthMap, 0);
    
    // Setup draw buffers
    GLuint attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "RSM FBO incomplete for directional light\n";
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DirectionalLight::resizeRSM(int resolution) {
    if(resolution == shadowResolution) return;
    shadowResolution = resolution;
    destroyRSM();
    initRSM();
}

void DirectionalLight::destroyRSM() {
    if(rsmNormalMap) glDeleteTextures(1, &rsmNormalMap);
    if(rsmColorMap) glDeleteTextures(1, &rsmColorMap);
    if(rsmDepthMap) glDeleteTextures(1, &rsmDepthMap);
    if(rsmFBO) glDeleteFramebuffers(1, &rsmFBO);
    rsmNormalMap = 0;
    rsmColorMap = 0;
    rsmDepthMap = 0;
    rsmFBO = 0;
}

} // namespace RenderLib
