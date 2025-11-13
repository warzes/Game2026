#include "RenderLib/Renderer.hpp"
#include "RenderLib/Shader.hpp"
#include "RenderLib/UBO.hpp"
#include "RenderLib/Mesh.hpp"
#include "RenderLib/Frustum.hpp"
#include "RenderLib/RenderQueue.hpp"
#include "RenderLib/Instancing.hpp"
#include <iostream>

namespace RenderLib {

ForwardRenderer::ForwardRenderer() 
    : renderQueue_(std::make_unique<RenderQueue>()),
      frustum_(std::make_unique<Frustum>()),
      instanceBuffer_(std::make_unique<InstanceBuffer>()) {}

ForwardRenderer::~ForwardRenderer() {}

bool ForwardRenderer::init(int width, int height) {
    width_ = width; height_ = height;
    
    // Create HDR framebuffer
    glGenFramebuffers(1, &hdrFBO_);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO_);
    colorBuffer_.createEmpty(width, height, GL_RGBA16F, GL_RGBA, GL_FLOAT);
    colorBuffer_.bind(0);
    
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "HDR FBO incomplete\n";
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return false;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // Setup UBOs
    UBO lights;
    lights.create("Lights", 1, 16 * 1024);
    
    // Setup instancing buffer
    instanceBuffer_->create(10000);  // max 10k instances
    
    return true;
}

void ForwardRenderer::resize(int w, int h) {
    width_ = w;
    height_ = h;
}

void ForwardRenderer::beginFrame() {
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO_);
    glViewport(0, 0, width_, height_);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderQueue_->clear();
}

void ForwardRenderer::endFrame() {
    // Render opaque queue first
    glDisable(GL_BLEND);
    // ... render opaque meshes
    
    // Sort and render transparent queue back-to-front
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);  // Don't write to depth for transparency
    
    glm::vec3 cameraPos(0.0f, 0.0f, 0.0f);  // TODO: get from view matrix
    renderQueue_->sortTransparent(cameraPos);
    // ... render transparent meshes
    
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ForwardRenderer::submitMesh(const Mesh& mesh, const Material& mat, 
                                const glm::mat4& model, bool transparent) {
    renderQueue_->submit((Mesh*)mesh.mesh_ptr, (Material*)&mat, model, transparent);
}

void ForwardRenderer::submitInstanced(const Mesh& mesh, const Material& mat,
                                     const std::vector<glm::mat4>& transforms) {
    std::vector<InstanceData> instances;
    for(const auto& transform : transforms) {
        InstanceData data{transform, glm::vec4(1.0f)};
        instances.push_back(data);
    }
    instanceBuffer_->updateData(instances);
    instanceBuffer_->bindSSBO(3);
    // Render with glDrawArraysInstanced
}

void ForwardRenderer::updateFrustum(const glm::mat4& viewProj) {
    frustum_->extractFromMatrix(viewProj);
}

bool ForwardRenderer::isMeshInFrustum(const glm::vec3& aabbMin, const glm::vec3& aabbMax) const {
    return frustum_->isAABBInside(aabbMin, aabbMax);
}

void ForwardRenderer::renderQueue() {
    // TODO: implement proper queue rendering
}
