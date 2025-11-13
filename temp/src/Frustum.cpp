#include "RenderLib/Frustum.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace RenderLib {

void Frustum::extractFromMatrix(const glm::mat4& viewProj) {
    // Extract frustum planes from view-projection matrix
    // Each row/column represents a plane
    
    // Left plane
    planes_[0].normal = glm::normalize(glm::vec3(viewProj[0][3] + viewProj[0][0],
                                                   viewProj[1][3] + viewProj[1][0],
                                                   viewProj[2][3] + viewProj[2][0]));
    planes_[0].distance = viewProj[3][3] + viewProj[3][0];

    // Right plane
    planes_[1].normal = glm::normalize(glm::vec3(viewProj[0][3] - viewProj[0][0],
                                                   viewProj[1][3] - viewProj[1][0],
                                                   viewProj[2][3] - viewProj[2][0]));
    planes_[1].distance = viewProj[3][3] - viewProj[3][0];

    // Bottom plane
    planes_[2].normal = glm::normalize(glm::vec3(viewProj[0][3] + viewProj[0][1],
                                                   viewProj[1][3] + viewProj[1][1],
                                                   viewProj[2][3] + viewProj[2][1]));
    planes_[2].distance = viewProj[3][3] + viewProj[3][1];

    // Top plane
    planes_[3].normal = glm::normalize(glm::vec3(viewProj[0][3] - viewProj[0][1],
                                                   viewProj[1][3] - viewProj[1][1],
                                                   viewProj[2][3] - viewProj[2][1]));
    planes_[3].distance = viewProj[3][3] - viewProj[3][1];

    // Near plane
    planes_[4].normal = glm::normalize(glm::vec3(viewProj[0][3] + viewProj[0][2],
                                                   viewProj[1][3] + viewProj[1][2],
                                                   viewProj[2][3] + viewProj[2][2]));
    planes_[4].distance = viewProj[3][3] + viewProj[3][2];

    // Far plane
    planes_[5].normal = glm::normalize(glm::vec3(viewProj[0][3] - viewProj[0][2],
                                                   viewProj[1][3] - viewProj[1][2],
                                                   viewProj[2][3] - viewProj[2][2]));
    planes_[5].distance = viewProj[3][3] - viewProj[3][2];
}

bool Frustum::isAABBInside(const glm::vec3& aabbMin, const glm::vec3& aabbMax) const {
    for(const auto& plane : planes_) {
        // Get closest point on AABB to plane
        glm::vec3 closest = aabbMin;
        if(plane.normal.x > 0.0f) closest.x = aabbMax.x;
        if(plane.normal.y > 0.0f) closest.y = aabbMax.y;
        if(plane.normal.z > 0.0f) closest.z = aabbMax.z;

        // If closest point is behind plane, AABB is outside
        if(plane.distanceToPoint(closest) < 0.0f) return false;
    }
    return true;
}

bool Frustum::isSphereInside(const glm::vec3& center, float radius) const {
    for(const auto& plane : planes_) {
        float dist = plane.distanceToPoint(center);
        if(dist < -radius) return false;
    }
    return true;
}

} // namespace RenderLib
