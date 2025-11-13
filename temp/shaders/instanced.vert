#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTex;
layout(location = 3) in vec3 aTangent;

out vec3 vWorldPos;
out vec3 vNormal;
out vec2 vTex;
out vec3 vTangent;
out flat int vInstanceID;

uniform mat4 uView;
uniform mat4 uProj;
uniform vec3 viewPos;

// Instance data from SSBO
struct InstanceData {
    mat4 model;
    vec4 color;
};

layout(std430, binding = 3) readonly buffer InstanceBuffer {
    InstanceData instances[];
};

void main() {
    InstanceData inst = instances[gl_InstanceID];
    
    vec4 world = inst.model * vec4(aPos, 1.0);
    vWorldPos = world.xyz;
    vNormal = mat3(transpose(inverse(inst.model))) * aNormal;
    vTangent = mat3(inst.model) * aTangent;
    vTex = aTex;
    vInstanceID = gl_InstanceID;
    
    gl_Position = uProj * uView * world;
}
