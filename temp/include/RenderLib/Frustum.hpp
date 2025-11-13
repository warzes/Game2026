#pragma once
#include <glm/glm.hpp>
#include <array>

namespace RenderLib {

// Frustum plane (normal + distance)
struct Plane {
    glm::vec3 normal;
    float distance;

    float distanceToPoint(const glm::vec3& point) const {
        return glm::dot(normal, point) + distance;
    }
};

// 6 planes: near, far, left, right, top, bottom
class Frustum {
public:
    Frustum() = default;

    // Extract frustum planes from combined view-projection matrix
    void extractFromMatrix(const glm::mat4& viewProj);

    // Test AABB against frustum (all 6 planes)
    bool isAABBInside(const glm::vec3& aabbMin, const glm::vec3& aabbMax) const;

    // Test sphere against frustum
    bool isSphereInside(const glm::vec3& center, float radius) const;

    const std::array<Plane, 6>& planes() const { return planes_; }

private:
    std::array<Plane, 6> planes_;
};

} // namespace RenderLib
