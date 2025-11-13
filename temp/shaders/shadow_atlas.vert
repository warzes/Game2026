#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 tangent;

out vec3 vWorldPos;
out vec3 vNormal;
out vec2 vTex;
out vec3 vTangent;
out vec4 vAtlasCoord;  // Atlas UV coordinates

uniform mat4 lightSpaceMatrix;
uniform mat4 model;
uniform vec4 atlasRect;  // Atlas position (x, y) and size (z, w)

void main() {
    vWorldPos = (model * vec4(position, 1.0)).xyz;
    vNormal = normalize((model * vec4(normal, 0.0)).xyz);
    vTex = texCoord;
    vTangent = normalize((model * vec4(tangent, 0.0)).xyz);
    
    // Transform to light space
    vec4 lightSpacePos = lightSpaceMatrix * vec4(vWorldPos, 1.0);
    
    // Perspective divide
    lightSpacePos.xyz /= lightSpacePos.w;
    
    // Transform from [-1, 1] to [0, 1]
    vec2 atlasUV = lightSpacePos.xy * 0.5 + 0.5;
    
    // Map to atlas region
    vAtlasCoord.xy = atlasRect.xy + atlasUV * atlasRect.zw;
    vAtlasCoord.z = lightSpacePos.z * 0.5 + 0.5;  // Depth
    vAtlasCoord.w = 1.0;
    
    gl_Position = lightSpacePos;
}
