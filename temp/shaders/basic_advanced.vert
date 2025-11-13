#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTex;
layout(location = 3) in vec3 aTangent;

out vec3 vWorldPos;
out vec3 vNormal;
out vec2 vTex;
out vec3 vTangent;
out vec3 vViewDir;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;
uniform vec3 viewPos;

void main() {
    vec4 world = uModel * vec4(aPos, 1.0);
    vWorldPos = world.xyz;
    vNormal = mat3(transpose(inverse(uModel))) * aNormal;
    vTangent = mat3(uModel) * aTangent;
    vTex = aTex;
    vViewDir = viewPos - vWorldPos;
    gl_Position = uProj * uView * world;
}
