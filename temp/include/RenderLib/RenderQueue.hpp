#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

namespace RenderLib {

struct MeshInstance {
    class Mesh* mesh;
    class Material* material;
    glm::mat4 transform;
    bool transparent;
    float depth;  // for sorting
};

class RenderQueue {
public:
    RenderQueue() = default;

    void clear();
    void submit(Mesh* mesh, Material* material, const glm::mat4& transform, bool transparent = false);
    
    // Sort transparent meshes by depth (back-to-front)
    void sortTransparent(const glm::vec3& cameraPos);

    const std::vector<MeshInstance>& opaqueQueue() const { return opaqueQueue_; }
    const std::vector<MeshInstance>& transparentQueue() const { return transparentQueue_; }

private:
    std::vector<MeshInstance> opaqueQueue_;
    std::vector<MeshInstance> transparentQueue_;
};

} // namespace RenderLib
