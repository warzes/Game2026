#include "RenderLib/LightPoint.hpp"
#include <iostream>

namespace RenderLib {

void PointLight::initShadowCube() {
    if(depthCubeFBO) return;
    glGenFramebuffers(1, &depthCubeFBO);
    glGenTextures(1, &depthCubeMap);

    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
    for(int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT32F,
                     shadowResolution, shadowResolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

    glBindFramebuffer(GL_FRAMEBUFFER, depthCubeFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Shadow cubeFBO incomplete for point light\n";
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PointLight::resizeShadow(int resolution) {
    if(resolution == shadowResolution) return;
    shadowResolution = resolution;
    if(depthCubeMap) glDeleteTextures(1, &depthCubeMap);
    if(depthCubeFBO) glDeleteFramebuffers(1, &depthCubeFBO);
    depthCubeMap = 0; depthCubeFBO = 0;
    initShadowCube();
}

void PointLight::initRSM() {
    if(rsmCubeFBO) return;
    
    glGenFramebuffers(1, &rsmCubeFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, rsmCubeFBO);
    
    // Create color cubemap (for lighting)
    glGenTextures(1, &rsmColorCube);
    glBindTexture(GL_TEXTURE_CUBE_MAP, rsmColorCube);
    for (int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA16F,
                     shadowResolution, shadowResolution, 0,
                     GL_RGBA, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    // Create normal cubemap (for RSM normal data)
    glGenTextures(1, &rsmCubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, rsmCubeMap);
    for (int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA16F,
                     shadowResolution, shadowResolution, 0,
                     GL_RGBA, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        glDeleteFramebuffers(1, &rsmCubeFBO);
        rsmCubeFBO = 0;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PointLight::resizeRSM(int resolution) {
    if(resolution == shadowResolution) return;
    shadowResolution = resolution;
    destroyRSM();
    initRSM();
}

void PointLight::destroyRSM() {
    if (rsmCubeFBO != 0) {
        glDeleteFramebuffers(1, &rsmCubeFBO);
        rsmCubeFBO = 0;
    }
    if (rsmColorCube != 0) {
        glDeleteTextures(1, &rsmColorCube);
        rsmColorCube = 0;
    }
    if (rsmCubeMap != 0) {
        glDeleteTextures(1, &rsmCubeMap);
        rsmCubeMap = 0;
    }
}

} // namespace RenderLib
