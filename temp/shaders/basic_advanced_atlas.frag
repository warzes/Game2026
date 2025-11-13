#version 450 core

// Shadow Atlas - Multiple shadow maps consolidated into single texture

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
    mat3 TBN;
} fs_in;

uniform sampler2D uTexture;
uniform sampler2D uNormalMap;
uniform sampler2D uRoughnessMap;
uniform sampler2D uMetallicMap;

// Shadow atlas (all lights consolidated)
uniform sampler2D uShadowAtlas;
uniform sampler2D uShadowAtlasColor;  // Optional RSM color in atlas

uniform vec3 uLightPos;
uniform vec3 uViewPos;
uniform vec3 uLightColor;
uniform float uLightIntensity;

// Atlas rect: (x, y, width, height) in normalized coordinates
uniform vec4 uAtlasRect;

// Atlas quality parameters
uniform float uAtlasBias = 0.005;
uniform float uAtlasSmoothing = 1.0;
uniform bool uAtlasRSMEnabled = false;

out vec4 FragColor;

const float PI = 3.14159265359;

// PCF sampling with atlas coordinates and smoothing
float sampleShadowAtlasPCF(vec3 lightSpacePos, float smoothing) {
    // Convert light space position to texture coordinates [0,1]
    vec2 texCoord = lightSpacePos.xy * 0.5 + 0.5;
    
    // Map to atlas tile (denormalize by atlas rect)
    vec2 atlasCoord = uAtlasRect.xy + texCoord * uAtlasRect.zw;
    
    // Check bounds
    if (atlasCoord.x < uAtlasRect.x || atlasCoord.x > (uAtlasRect.x + uAtlasRect.z) ||
        atlasCoord.y < uAtlasRect.y || atlasCoord.y > (uAtlasRect.y + uAtlasRect.w)) {
        return 1.0;
    }
    
    float currentDepth = (lightSpacePos.z * 0.5 + 0.5);  // Normalize to [0,1]
    float pixelSize = 1.0 / 4096.0;  // Atlas texture size
    
    // Simple 3x3 PCF
    float shadow = 0.0;
    float closestDepth = texture(uShadowAtlas, atlasCoord).r;
    if (currentDepth - uAtlasBias <= closestDepth) { shadow = shadow + 1.0; }
    
    closestDepth = texture(uShadowAtlas, atlasCoord + vec2(pixelSize, 0.0) * smoothing).r;
    if (currentDepth - uAtlasBias <= closestDepth) { shadow = shadow + 1.0; }
    
    closestDepth = texture(uShadowAtlas, atlasCoord + vec2(-pixelSize, 0.0) * smoothing).r;
    if (currentDepth - uAtlasBias <= closestDepth) { shadow = shadow + 1.0; }
    
    closestDepth = texture(uShadowAtlas, atlasCoord + vec2(0.0, pixelSize) * smoothing).r;
    if (currentDepth - uAtlasBias <= closestDepth) { shadow = shadow + 1.0; }
    
    closestDepth = texture(uShadowAtlas, atlasCoord + vec2(0.0, -pixelSize) * smoothing).r;
    if (currentDepth - uAtlasBias <= closestDepth) { shadow = shadow + 1.0; }
    
    closestDepth = texture(uShadowAtlas, atlasCoord + vec2(pixelSize, pixelSize) * smoothing).r;
    if (currentDepth - uAtlasBias <= closestDepth) { shadow = shadow + 1.0; }
    
    closestDepth = texture(uShadowAtlas, atlasCoord + vec2(-pixelSize, pixelSize) * smoothing).r;
    if (currentDepth - uAtlasBias <= closestDepth) { shadow = shadow + 1.0; }
    
    closestDepth = texture(uShadowAtlas, atlasCoord + vec2(pixelSize, -pixelSize) * smoothing).r;
    if (currentDepth - uAtlasBias <= closestDepth) { shadow = shadow + 1.0; }
    
    closestDepth = texture(uShadowAtlas, atlasCoord + vec2(-pixelSize, -pixelSize) * smoothing).r;
    if (currentDepth - uAtlasBias <= closestDepth) { shadow = shadow + 1.0; }
    
    return shadow / 9.0;
}

// PBR functions
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return nom / denom;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

void main() {
    // Sample textures
    vec4 texColor = texture(uTexture, fs_in.TexCoords);
    vec3 normal = texture(uNormalMap, fs_in.TexCoords).rgb;
    normal = normalize(fs_in.TBN * (normal * 2.0 - 1.0));
    
    float roughness = texture(uRoughnessMap, fs_in.TexCoords).r;
    float metallic = texture(uMetallicMap, fs_in.TexCoords).r;
    
    vec3 albedo = texColor.rgb;
    
    // Lighting vectors
    vec3 N = normal;
    vec3 V = normalize(uViewPos - fs_in.FragPos);
    vec3 L = normalize(uLightPos - fs_in.FragPos);
    vec3 H = normalize(V + L);
    
    // Sample shadow from atlas with PCF
    vec3 lightSpacePos = fs_in.FragPosLightSpace.xyz / fs_in.FragPosLightSpace.w;
    float shadow = sampleShadowAtlasPCF(lightSpacePos, uAtlasSmoothing);
    
    // Cook-Torrance PBR
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float HdotV = max(dot(H, V), 0.0);
    
    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = FresnelSchlick(HdotV, F0);
    
    vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);
    vec3 kS = F;
    
    vec3 specular = (NDF * G * F) / max(0.001, 4.0 * NdotV * NdotL + 0.0001);
    
    // Direct light with atlas shadow
    vec3 Lo = (kD * albedo / PI + specular) * uLightColor * uLightIntensity * NdotL * shadow;
    
    // Ambient
    vec3 ambient = albedo * 0.03;
    
    vec3 result = ambient + Lo;
    
    // Tone mapping
    result = result / (result + vec3(1.0));
    result = pow(result, vec3(1.0 / 2.2));
    
    FragColor = vec4(result, texColor.a);
}
