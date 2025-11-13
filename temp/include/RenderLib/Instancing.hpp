#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

namespace RenderLib {

// Instance data for SSBO-based rendering
struct InstanceData {
    glm::mat4 model;
    glm::vec4 color;  // or material index
};

class InstanceBuffer {
public:
    InstanceBuffer();
    ~InstanceBuffer();

    bool create(size_t maxInstances);
    void updateData(const std::vector<InstanceData>& instances);
    void bindSSBO(GLuint bindingPoint);
    
    size_t instanceCount() const { return instanceCount_; }
    GLuint ssboId() const { return ssbo_; }

private:
    GLuint ssbo_{0};
    size_t maxInstances_{0};
    size_t instanceCount_{0};
};

} // namespace RenderLib
