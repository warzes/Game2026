#include "RenderLib/RenderQueue.hpp"
#include "RenderLib/Mesh.hpp"
#include "RenderLib/Renderer.hpp"
#include <algorithm>

namespace RenderLib {

void RenderQueue::clear() {
    opaqueQueue_.clear();
    transparentQueue_.clear();
}

void RenderQueue::submit(Mesh* mesh, Material* material, const glm::mat4& transform, bool transparent) {
    MeshInstance instance{mesh, material, transform, transparent, 0.0f};
    
    if(transparent) {
        transparentQueue_.push_back(instance);
    } else {
        opaqueQueue_.push_back(instance);
    }
}

void RenderQueue::sortTransparent(const glm::vec3& cameraPos) {
    // Calculate depth for each transparent mesh (distance to camera)
    for(auto& instance : transparentQueue_) {
        glm::vec3 meshPos = glm::vec3(instance.transform[3]);
        instance.depth = glm::distance(cameraPos, meshPos);
    }

    // Sort back-to-front (largest depth first)
    std::sort(transparentQueue_.begin(), transparentQueue_.end(),
              [](const MeshInstance& a, const MeshInstance& b) {
                  return a.depth > b.depth;
              });
}

} // namespace RenderLib
