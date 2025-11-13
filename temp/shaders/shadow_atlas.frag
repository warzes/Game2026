#version 330 core

in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vTex;
in vec3 vTangent;
in vec4 vAtlasCoord;

out vec4 FragColor;

uniform sampler2D uDiffuse;
uniform sampler2D uNormalMap;
uniform sampler2D atlasDepthMap;  // Single atlas depth texture

// Build TBN
mat3 getTBN() {
    vec3 N = normalize(vNormal);
    vec3 T = normalize(vTangent);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    return mat3(T, B, N);
}

void main() {
    // Sample textures
    vec3 albedo = texture(uDiffuse, vTex).rgb;
    vec3 texNormal = texture(uNormalMap, vTex).rgb;
    texNormal = normalize(texNormal * 2.0 - 1.0);
    
    // Compute normal
    mat3 tbn = getTBN();
    vec3 normal = normalize(tbn * texNormal);
    
    // Store depth in atlas at correct UV coordinates
    gl_FragDepth = vAtlasCoord.z;
    
    // Output color for color atlas (if using dual atlas)
    FragColor = vec4(albedo, 1.0);
}
