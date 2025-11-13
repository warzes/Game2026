#version 330 core

in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vTex;
in vec3 vTangent;

// Output to MRT (Multiple Render Targets)
layout(location = 0) out vec4 colorOut;      // RGB = emitted color, A = depth
layout(location = 1) out vec4 normalOut;     // Normal map from light space

uniform sampler2D uDiffuse;
uniform sampler2D uNormalMap;

// Build TBN for normal mapping
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
    
    // Compute normal in world space
    mat3 tbn = getTBN();
    vec3 normal = normalize(tbn * texNormal);
    
    // Store color (weighted by diffuse)
    colorOut = vec4(albedo * 0.8, gl_FragCoord.z);
    
    // Store normal (encode to [0,1] range)
    normalOut = vec4(normal * 0.5 + 0.5, 1.0);
}
