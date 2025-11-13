#version 330 core

in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vTex;
in vec3 vTangent;
in vec3 vViewDir;

out vec4 FragColor;

// Light structures (std140 layout for UBO)
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
    mat4 cascadeMatrices[4][3];  // 3 cascades per directional light
};

uniform sampler2D uDiffuse;
uniform sampler2D uSpecular;
uniform sampler2D uNormalMap;

uniform sampler2D dirShadowMaps[4];
uniform sampler2D spotShadowMaps[4];
uniform samplerCube pointShadowCubes[4];

uniform sampler2D cascadeShadowMaps[4][3];  // cascade textures

uniform vec3 viewPos;

// Fog parameters
uniform vec3 fogColor;
uniform float fogStart;
uniform float fogEnd;
uniform vec2 fogHeightZone;

// Parallax mapping parameters
uniform float heightScale;
uniform int minLayers;
uniform int maxLayers;
uniform bool useParallax;

// Build TBN matrix for normal mapping and parallax
void setupTBN(out vec3 T, out vec3 B, out vec3 N) {
    N = normalize(vNormal);
    T = normalize(vTangent);
    T = normalize(T - dot(T, N) * N);
    B = cross(N, T);
}

// Parallax mapping: offset UV based on heightmap in normal texture alpha
vec2 parallaxMapping(vec2 texCoords, vec3 viewDir, out float parallaxHeight) {
    if(!useParallax) {
        parallaxHeight = 0.0;
        return texCoords;
    }
    
    vec3 T, B, N;
    setupTBN(T, B, N);
    mat3 TBN = mat3(T, B, N);
    vec3 V = normalize(TBN * viewDir);
    
    float layerCount = mix(float(maxLayers), float(minLayers), abs(dot(vec3(0,0,1), V)));
    float layerDepth = 1.0 / layerCount;
    float currentLayerDepth = 0.0;
    vec2 P = V.xy * heightScale;
    vec2 deltaTexCoords = P / layerCount;
    
    vec2 currentTexCoords = texCoords;
    float currentDepthMapValue = texture(uNormalMap, currentTexCoords).a;
    
    while(currentLayerDepth < currentDepthMapValue) {
        currentTexCoords -= deltaTexCoords;
        currentDepthMapValue = texture(uNormalMap, currentTexCoords).a;
        currentLayerDepth += layerDepth;
    }
    
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
    float afterDepth = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(uNormalMap, prevTexCoords).a - currentLayerDepth + layerDepth;
    
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = mix(currentTexCoords, prevTexCoords, weight);
    
    parallaxHeight = mix(currentLayerDepth, currentLayerDepth - layerDepth, weight);
    return finalTexCoords;
}

// Get normal from normal map with TBN
vec3 getNormalFromMap(vec2 texCoords) {
    vec3 T, B, N;
    setupTBN(T, B, N);
    
    vec3 texNormal = texture(uNormalMap, texCoords).rgb;
    texNormal = normalize(texNormal * 2.0 - 1.0);
    
    mat3 TBN = mat3(T, B, N);
    return normalize(TBN * texNormal);
}

// Cascade shadow selection based on fragment distance
int selectCascade(float fragDepth) {
    if(fragDepth < 0.25) return 0;
    if(fragDepth < 0.5) return 1;
    return 2;
}

// PCF for cascade shadow maps
float sampleCascadeShadow(int lightIdx, int cascade, vec4 fragPosLightSpace) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    if(projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || 
       projCoords.y < 0.0 || projCoords.y > 1.0) return 0.0;
    
    float shadow = 0.0;
    float bias = 0.005;
    vec2 texelSize = 1.0 / textureSize(cascadeShadowMaps[lightIdx][cascade], 0);
    
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(cascadeShadowMaps[lightIdx][cascade], projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += (projCoords.z - bias > pcfDepth ? 1.0 : 0.0);
        }
    }
    return shadow / 9.0;
}

// Standard PCF (for non-cascade lights)
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

float sampleSpotShadowPCF(int idx, vec4 fragPosLightSpace) {
    return sampleDirShadowPCF(idx, fragPosLightSpace);
}

float samplePointShadow(int idx, vec3 fragPos, vec3 lightPos) {
    vec3 fragToLight = fragPos - lightPos;
    float currentDepth = length(fragToLight);
    float shadow = 0.0;
    float bias = 0.15;
    float samples = 4.0;
    float offset = 0.1;
    
    for(float x = -offset; x < offset; x += offset / 2.0) {
        for(float y = -offset; y < offset; y += offset / 2.0) {
            for(float z = -offset; z < offset; z += offset / 2.0) {
                float pcfDepth = texture(pointShadowCubes[idx], fragToLight + vec3(x,y,z)).r;
                pcfDepth *= 25.0;
                shadow += (currentDepth - bias > pcfDepth ? 1.0 : 0.0);
            }
        }
    }
    return shadow / (samples * samples * samples);
}

void main() {
    // Apply parallax mapping to UV coords
    float parallaxHeight = 0.0;
    vec2 texCoords = parallaxMapping(vTex, vViewDir, parallaxHeight);
    
    // Early discard for out-of-bounds parallax
    if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
        discard;
    
    vec3 albedo = texture(uDiffuse, texCoords).rgb;
    vec3 specMap = texture(uSpecular, texCoords).rgb;
    vec3 N = getNormalFromMap(texCoords);
    vec3 V = normalize(viewPos - vWorldPos);
    
    vec3 result = vec3(0.0);
    
    // Directional lights with cascades
    for(int i = 0; i < 4; ++i) {
        vec3 L = normalize(-dirLights[i].direction);
        vec3 H = normalize(L + V);
        float diff = max(dot(N, L), 0.0);
        float spec = pow(max(dot(N, H), 0.0), 16.0) * specMap.r;
        
        // Use cascade shadow
        vec4 fragPosLightSpace = cascadeMatrices[i][0] * vec4(vWorldPos, 1.0);
        float shadow = sampleCascadeShadow(i, 0, fragPosLightSpace);
        
        result += (1.0 - shadow) * dirLights[i].color * dirLights[i].intensity * (albedo * diff + spec);
    }
    
    // Point lights
    for(int i = 0; i < 4; ++i) {
        vec3 L = normalize(pointLights[i].position - vWorldPos);
        float dist = length(pointLights[i].position - vWorldPos);
        float attenuation = 1.0 / (1.0 + 0.09 * dist + 0.032 * dist * dist);
        vec3 H = normalize(L + V);
        float diff = max(dot(N, L), 0.0);
        float spec = pow(max(dot(N, H), 0.0), 16.0) * specMap.r;
        float shadow = samplePointShadow(i, vWorldPos, pointLights[i].position);
        result += (1.0 - shadow) * pointLights[i].color * pointLights[i].intensity * attenuation * (albedo * diff + spec);
    }
    
    // Spot lights
    for(int i = 0; i < 4; ++i) {
        vec3 L = normalize(spotLights[i].position - vWorldPos);
        float theta = dot(L, normalize(-spotLights[i].direction));
        float epsilon = spotLights[i].innerCone - spotLights[i].outerCone;
        float intensity = clamp((theta - spotLights[i].outerCone) / epsilon, 0.0, 1.0);
        vec3 H = normalize(L + V);
        float diff = max(dot(N, L), 0.0);
        float spec = pow(max(dot(N, H), 0.0), 16.0) * specMap.r;
        vec4 fragPosLightSpace = spotLightMatrices[i] * vec4(vWorldPos, 1.0);
        float shadow = sampleSpotShadowPCF(i, fragPosLightSpace);
        result += (1.0 - shadow) * spotLights[i].color * spotLights[i].intensity * intensity * (albedo * diff + spec);
    }
    
    // Ambient
    result += vec3(0.03) * albedo;
    
    // Fog with height zone
    float distance = length(viewPos - vWorldPos);
    float fogFactor = clamp((distance - fogStart) / (fogEnd - fogStart), 0.0, 1.0);
    float heightFactor = smoothstep(fogHeightZone.x, fogHeightZone.y, vWorldPos.y);
    float finalFog = mix(0.0, fogFactor, heightFactor);
    
    vec3 color = mix(result, fogColor, finalFog);
    
    FragColor = vec4(color, 1.0);
}
