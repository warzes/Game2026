#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>

namespace RenderLib {

class Mesh;
class Material;
class Frustum;
class RenderQueue;
class InstanceBuffer;

class ForwardRenderer {
public:
    ForwardRenderer();
    ~ForwardRenderer();

    bool init(int width, int height);
    void resize(int w, int h);

    void beginFrame();
    void endFrame();

    // Standard rendering with optional transparency
    void submitMesh(const Mesh& mesh, const Material& mat, const glm::mat4& model, bool transparent = false);
    
    // Instanced rendering
    void submitInstanced(const Mesh& mesh, const Material& mat, 
                        const std::vector<glm::mat4>& transforms);

    // Frustum culling
    void enableFrustumCulling(bool enable) { frustumCullingEnabled_ = enable; }
    void updateFrustum(const glm::mat4& viewProj);
    bool isMeshInFrustum(const glm::vec3& aabbMin, const glm::vec3& aabbMax) const;

    int width() const { return width_; }
    int height() const { return height_; }
    GLuint hdrColorBuffer() const { return colorBuffer_.id(); }
    
    // Access to internal systems
    RenderQueue* getRenderQueue() { return renderQueue_.get(); }
    Frustum* getFrustum() { return frustum_.get(); }

private:
    int width_{0}, height_{0};
    GLuint hdrFBO_{0};
    class Texture2D colorBuffer_;
    GLuint lightsUBO_{0};
    
    std::unique_ptr<RenderQueue> renderQueue_;
    std::unique_ptr<Frustum> frustum_;
    std::unique_ptr<InstanceBuffer> instanceBuffer_;
    
    bool frustumCullingEnabled_{false};
    
    void renderQueue();
};

struct Material {
    class Texture2D* diffuse{nullptr};
    class Texture2D* specular{nullptr};
    class Texture2D* normal{nullptr};
    float shininess{32.0f};
};

struct Mesh {
    class Mesh* mesh_ptr;
};

} // namespace RenderLib
