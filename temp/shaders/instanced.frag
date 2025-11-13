#version 330 core

in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vTex;
in vec3 vTangent;
in flat int vInstanceID;

out vec4 FragColor;

// Same light structures as basic.frag
struct DirLight {
    vec3 direction;
    float pad0;
    vec3 color;
    float intensity;
};

struct PointLight {
    vec3 position;
    float pad0;
    vec3 color;
    float intensity;
    float radius;
    float pad1, pad2, pad3;
};

struct SpotLight {
    vec3 position;
    float pad0;
    vec3 direction;
    float pad1;
    vec3 color;
    float intensity;
    float innerCone;
    float outerCone;
    float pad2, pad3;
};

layout(std140) uniform Lights {
    DirLight dirLights[4];
    PointLight pointLights[4];
    SpotLight spotLights[4];
    mat4 dirLightMatrices[4];
    mat4 spotLightMatrices[4];
};

uniform sampler2D uDiffuse;
uniform sampler2D uSpecular;
uniform sampler2D uNormalMap;

uniform sampler2D dirShadowMaps[4];
uniform sampler2D spotShadowMaps[4];
uniform samplerCube pointShadowCubes[4];

uniform vec3 viewPos;
uniform vec3 fogColor;
uniform float fogStart;
uniform float fogEnd;
uniform vec2 fogHeightZone;

vec3 getNormalFromMap() {
    vec3 N = normalize(vNormal);
    vec3 T = normalize(vTangent);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    
    vec3 texNormal = texture(uNormalMap, vTex).rgb;
    texNormal = normalize(texNormal * 2.0 - 1.0);
    
    mat3 TBN = mat3(T, B, N);
    return normalize(TBN * texNormal);
}

float sampleDirShadowPCF(int idx, vec4 fragPosLightSpace) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    if(projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || 
       projCoords.y < 0.0 || projCoords.y > 1.0) return 0.0;
    
    float shadow = 0.0;
    float bias = 0.005;
    vec2 texelSize = 1.0 / textureSize(dirShadowMaps[idx], 0);
    
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(dirShadowMaps[idx], projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += (projCoords.z - bias > pcfDepth ? 1.0 : 0.0);
        }
    }
    return shadow / 9.0;
}

void main() {
    vec3 albedo = texture(uDiffuse, vTex).rgb;
    vec3 specMap = texture(uSpecular, vTex).rgb;
    vec3 N = getNormalFromMap();
    vec3 V = normalize(viewPos - vWorldPos);
    
    vec3 result = vec3(0.0);
    
    // Simplified lighting (same as basic, but using instance color modulation)
    for(int i = 0; i < 4; ++i) {
        vec3 L = normalize(-dirLights[i].direction);
        vec3 H = normalize(L + V);
        float diff = max(dot(N, L), 0.0);
        float spec = pow(max(dot(N, H), 0.0), 16.0) * specMap.r;
        
        vec4 fragPosLightSpace = dirLightMatrices[i] * vec4(vWorldPos, 1.0);
        float shadow = sampleDirShadowPCF(i, fragPosLightSpace);
        
        result += (1.0 - shadow) * dirLights[i].color * dirLights[i].intensity * (albedo * diff + spec);
    }
    
    result += vec3(0.03) * albedo;
    
    float distance = length(viewPos - vWorldPos);
    float fogFactor = clamp((distance - fogStart) / (fogEnd - fogStart), 0.0, 1.0);
    float heightFactor = smoothstep(fogHeightZone.x, fogHeightZone.y, vWorldPos.y);
    float finalFog = mix(0.0, fogFactor, heightFactor);
    
    vec3 color = mix(result, fogColor, finalFog);
    
    FragColor = vec4(color, 1.0);
}
