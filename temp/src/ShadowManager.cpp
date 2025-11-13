#include "RenderLib/ShadowManager.hpp"
#include "RenderLib/LightDirectional.hpp"
#include "RenderLib/LightPoint.hpp"
#include "RenderLib/LightSpot.hpp"
#include "RenderLib/Shader.hpp"
#include "RenderLib/Mesh.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

namespace RenderLib {

ShadowManager::ShadowManager() = default;

ShadowManager::~ShadowManager() {
    destroy();
}

bool ShadowManager::init() {
    // Load shaders
    standardShadowShader_ = std::make_shared<Shader>(
        "shaders/shadow_dir.vert",
        "shaders/shadow_depth.frag"
    );

    rsmShadowShader_ = std::make_shared<Shader>(
        "shaders/shadow_rsm.vert",
        "shaders/shadow_rsm.frag"
    );

    atlasShadowShader_ = std::make_shared<Shader>(
        "shaders/shadow_atlas.vert",
        "shaders/shadow_atlas.frag"
    );

    if (!standardShadowShader_ || !rsmShadowShader_ || !atlasShadowShader_) {
        return false;
    }

    return true;
}

void ShadowManager::destroy() {
    if (shadowAtlasFBO_ != 0) {
        glDeleteFramebuffers(1, &shadowAtlasFBO_);
        shadowAtlasFBO_ = 0;
    }
    if (shadowAtlasTexture_ != 0) {
        glDeleteTextures(1, &shadowAtlasTexture_);
        shadowAtlasTexture_ = 0;
    }
    if (shadowAtlasColorTexture_ != 0) {
        glDeleteTextures(1, &shadowAtlasColorTexture_);
        shadowAtlasColorTexture_ = 0;
    }
}

void ShadowManager::allocateAtlasSpace(uint32_t lightCount, uint32_t resolution) {
    atlasResolution_ = resolution;
    
    // Calculate grid dimensions (prefer square)
    atlasTilesX_ = static_cast<uint32_t>(std::ceil(std::sqrt(lightCount)));
    atlasTilesY_ = (lightCount + atlasTilesX_ - 1) / atlasTilesX_;

    createAtlas_();
}

glm::vec4 ShadowManager::getAtlasRect(uint32_t lightIndex) const {
    uint32_t x = lightIndex % atlasTilesX_;
    uint32_t y = lightIndex / atlasTilesX_;

    float tileWidth = 1.0f / atlasTilesX_;
    float tileHeight = 1.0f / atlasTilesY_;

    return glm::vec4(
        x * tileWidth,
        y * tileHeight,
        tileWidth,
        tileHeight
    );
}

bool ShadowManager::createAtlas_() {
    // Create shadow atlas FBO
    glGenFramebuffers(1, &shadowAtlasFBO_);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowAtlasFBO_);

    uint32_t totalWidth = atlasResolution_ * atlasTilesX_;
    uint32_t totalHeight = atlasResolution_ * atlasTilesY_;

    // Depth texture
    glGenTextures(1, &shadowAtlasTexture_);
    glBindTexture(GL_TEXTURE_2D, shadowAtlasTexture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, totalWidth, totalHeight,
                 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                          shadowAtlasTexture_, 0);

    // Color texture (for RSM)
    glGenTextures(1, &shadowAtlasColorTexture_);
    glBindTexture(GL_TEXTURE_2D, shadowAtlasColorTexture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, totalWidth, totalHeight,
                 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                          shadowAtlasColorTexture_, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

void ShadowManager::renderDirectionalShadow(DirectionalLight* light,
                                           const glm::mat4& lightSpace,
                                           const std::vector<Mesh*>& meshes) {
    switch (atlasMode_) {
        case AtlasMode::Individual:
            if (shadowMode_ == ShadowMode::Standard) {
                renderToStandardShadow_(light->depthMapFBO, lightSpace, meshes);
            } else {
                renderToRSM_(light->rsmFBO, lightSpace, meshes);
            }
            break;

        case AtlasMode::Atlas:
            renderToAtlas_(0, lightSpace, meshes);  // Light index 0 for directional
            break;
    }
}

void ShadowManager::renderPointShadow(PointLight* light,
                                     const std::vector<glm::mat4>& faceMatrices,
                                     const std::vector<Mesh*>& meshes) {
    // For point lights with atlas, need to handle cubemap to atlas conversion
    // For now, use individual rendering
    if (atlasMode_ == AtlasMode::Individual) {
        if (shadowMode_ == ShadowMode::Standard) {
            // Render to cubemap depth
            for (int i = 0; i < 6; ++i) {
                glBindFramebuffer(GL_FRAMEBUFFER, light->cubeShadowFBO);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                      GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                                      light->cubeShadowMap, 0);

                glViewport(0, 0, light->shadowResolution, light->shadowResolution);
                glClear(GL_DEPTH_BUFFER_BIT);

                standardShadowShader_->use();
                standardShadowShader_->setMat4("lightSpaceMatrix", faceMatrices[i]);

                for (auto mesh : meshes) {
                    standardShadowShader_->setMat4("model", mesh->getModelMatrix());
                    mesh->render();
                }
            }
        } else {
            // RSM cubemap rendering
            for (int i = 0; i < 6; ++i) {
                glBindFramebuffer(GL_FRAMEBUFFER, light->rsmCubeFBO);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                      GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                                      light->rsmColorCube, 0);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
                                      GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                                      light->rsmCubeMap, 0);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                      GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                                      light->cubeShadowMap, 0);

                GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
                glDrawBuffers(2, attachments);

                glViewport(0, 0, light->shadowResolution, light->shadowResolution);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                rsmShadowShader_->use();
                rsmShadowShader_->setMat4("lightSpaceMatrix", faceMatrices[i]);

                for (auto mesh : meshes) {
                    rsmShadowShader_->setMat4("model", mesh->getModelMatrix());
                    mesh->render();
                }
            }
        }
    }
}

void ShadowManager::renderSpotShadow(SpotLight* light,
                                    const glm::mat4& lightSpace,
                                    const std::vector<Mesh*>& meshes) {
    if (atlasMode_ == AtlasMode::Individual) {
        if (shadowMode_ == ShadowMode::Standard) {
            renderToStandardShadow_(light->depthMapFBO, lightSpace, meshes);
        } else {
            renderToRSM_(light->rsmFBO, lightSpace, meshes);
        }
    } else {
        // Atlas mode - assign spot light to atlas tile
        renderToAtlas_(1, lightSpace, meshes);  // Spot lights start at index 1
    }
}

void ShadowManager::renderToStandardShadow_(GLuint targetFBO,
                                           const glm::mat4& lightSpace,
                                           const std::vector<Mesh*>& meshes) {
    glBindFramebuffer(GL_FRAMEBUFFER, targetFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    standardShadowShader_->use();
    standardShadowShader_->setMat4("lightSpaceMatrix", lightSpace);

    for (auto mesh : meshes) {
        standardShadowShader_->setMat4("model", mesh->getModelMatrix());
        mesh->render();
    }
}

void ShadowManager::renderToRSM_(GLuint targetFBO,
                               const glm::mat4& lightSpace,
                               const std::vector<Mesh*>& meshes) {
    glBindFramebuffer(GL_FRAMEBUFFER, targetFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    rsmShadowShader_->use();
    rsmShadowShader_->setMat4("lightSpaceMatrix", lightSpace);

    for (auto mesh : meshes) {
        rsmShadowShader_->setMat4("model", mesh->getModelMatrix());
        mesh->render();
    }

    // Restore single buffer mode
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
}

void ShadowManager::renderToAtlas_(uint32_t lightIndex,
                                  const glm::mat4& lightSpace,
                                  const std::vector<Mesh*>& meshes) {
    glBindFramebuffer(GL_FRAMEBUFFER, shadowAtlasFBO_);

    glm::vec4 atlasRect = getAtlasRect(lightIndex);

    uint32_t x = static_cast<uint32_t>(atlasRect.x * atlasResolution_ * atlasTilesX_);
    uint32_t y = static_cast<uint32_t>(atlasRect.y * atlasResolution_ * atlasTilesY_);
    uint32_t w = static_cast<uint32_t>(atlasRect.z * atlasResolution_ * atlasTilesX_);
    uint32_t h = static_cast<uint32_t>(atlasRect.w * atlasResolution_ * atlasTilesY_);

    glViewport(x, y, w, h);
    glScissor(x, y, w, h);
    glEnable(GL_SCISSOR_TEST);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    if (shadowMode_ == ShadowMode::Standard) {
        standardShadowShader_->use();
        standardShadowShader_->setMat4("lightSpaceMatrix", lightSpace);
    } else {
        rsmShadowShader_->use();
        rsmShadowShader_->setMat4("lightSpaceMatrix", lightSpace);

        GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glDrawBuffers(2, attachments);
    }

    for (auto mesh : meshes) {
        if (shadowMode_ == ShadowMode::Standard) {
            standardShadowShader_->setMat4("model", mesh->getModelMatrix());
        } else {
            rsmShadowShader_->setMat4("model", mesh->getModelMatrix());
        }
        mesh->render();
    }

    glDisable(GL_SCISSOR_TEST);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

}  // namespace RenderLib
