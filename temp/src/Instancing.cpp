#include "RenderLib/Instancing.hpp"

namespace RenderLib {

InstanceBuffer::InstanceBuffer() {}

InstanceBuffer::~InstanceBuffer() {
    if(ssbo_) glDeleteBuffers(1, &ssbo_);
}

bool InstanceBuffer::create(size_t maxInstances) {
    maxInstances_ = maxInstances;
    glGenBuffers(1, &ssbo_);
    glBindBuffer(GL_COPY_READ_BUFFER, ssbo_);
    glBufferData(GL_COPY_READ_BUFFER, maxInstances * sizeof(InstanceData), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_COPY_READ_BUFFER, 0);
    return true;
}

void InstanceBuffer::updateData(const std::vector<InstanceData>& instances) {
    instanceCount_ = glm::min(instances.size(), maxInstances_);
    glBindBuffer(GL_COPY_WRITE_BUFFER, ssbo_);
    glBufferSubData(GL_COPY_WRITE_BUFFER, 0, instanceCount_ * sizeof(InstanceData), instances.data());
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
}

void InstanceBuffer::bindSSBO(GLuint bindingPoint) {
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, ssbo_);
}

} // namespace RenderLib
