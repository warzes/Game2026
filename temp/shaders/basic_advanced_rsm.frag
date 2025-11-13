#version 450 core

// RSM (Reflective Shadow Map) - Advanced lighting with indirect shadows
// Uses color and normal information from shadow maps for enhanced shading

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

// RSM textures (color + normal from light viewpoint)
uniform sampler2D uRSMColorMap;      // Color information from light
uniform sampler2D uRSMNormalMap;     // Surface normal from light
uniform sampler2D uRSMDepthMap;      // Depth from light

uniform vec3 uLightPos;
uniform vec3 uViewPos;
uniform vec3 uLightColor;
uniform float uLightIntensity;

// RSM quality parameters
uniform float uRSMSmoothness;        // Smoothing radius (1-3)
uniform float uRSMBias;              // Depth bias
uniform float uRSMRadius;            // Sample radius for indirect
uniform float uRSMIntensity;         // Contribution intensity

out vec4 FragColor;

const float PI = 3.14159265359;

// Normal distribution function (GGX)
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

// Fresnel-Schlick approximation
vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// Smith's Schlick-GGX geometry function
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

// Decode normal from RSM
vec3 decodeRSMNormal(vec2 rsmTexCoord) {
    vec3 encoded = texture(uRSMNormalMap, rsmTexCoord).rgb;
    return normalize(encoded * 2.0 - 1.0);
}

// PCF filtering for RSM with smoothness parameter
float rsmPCF(vec3 lightSpacePos, vec2 baseCoord, float smoothness) {
    float depth = texture(uRSMDepthMap, baseCoord).r;
    float shadow = 0.0;
    float pixelSize = 1.0 / 2048.0;  // Assuming 2048x2048 RSM
    
    // Simple PCF with 3x3 kernel
    shadow = shadow + (lightSpacePos.z <= depth + uRSMBias ? 1.0 : 0.0);
    shadow = shadow + (lightSpacePos.z <= texture(uRSMDepthMap, baseCoord + vec2(pixelSize, 0.0) * smoothness).r + uRSMBias ? 1.0 : 0.0);
    shadow = shadow + (lightSpacePos.z <= texture(uRSMDepthMap, baseCoord + vec2(-pixelSize, 0.0) * smoothness).r + uRSMBias ? 1.0 : 0.0);
    shadow = shadow + (lightSpacePos.z <= texture(uRSMDepthMap, baseCoord + vec2(0.0, pixelSize) * smoothness).r + uRSMBias ? 1.0 : 0.0);
    shadow = shadow + (lightSpacePos.z <= texture(uRSMDepthMap, baseCoord + vec2(0.0, -pixelSize) * smoothness).r + uRSMBias ? 1.0 : 0.0);
    shadow = shadow + (lightSpacePos.z <= texture(uRSMDepthMap, baseCoord + vec2(pixelSize, pixelSize) * smoothness).r + uRSMBias ? 1.0 : 0.0);
    shadow = shadow + (lightSpacePos.z <= texture(uRSMDepthMap, baseCoord + vec2(-pixelSize, pixelSize) * smoothness).r + uRSMBias ? 1.0 : 0.0);
    shadow = shadow + (lightSpacePos.z <= texture(uRSMDepthMap, baseCoord + vec2(pixelSize, -pixelSize) * smoothness).r + uRSMBias ? 1.0 : 0.0);
    shadow = shadow + (lightSpacePos.z <= texture(uRSMDepthMap, baseCoord + vec2(-pixelSize, -pixelSize) * smoothness).r + uRSMBias ? 1.0 : 0.0);
    
    return shadow / 9.0;
}

// Sample RSM information at light space position with multiple samples
vec4 sampleRSMIndirect(vec3 lightSpacePos, vec3 worldNormal) {
    // Convert from light space [-1,1] to texture coordinates [0,1]
    vec2 rsmTexCoord = lightSpacePos.xy * 0.5 + 0.5;
    
    // Check bounds
    if (rsmTexCoord.x < 0.0 || rsmTexCoord.x > 1.0 ||
        rsmTexCoord.y < 0.0 || rsmTexCoord.y > 1.0) {
        return vec4(0.0);
    }
    
    // Depth check with bias
    float depth = texture(uRSMDepthMap, rsmTexCoord).r;
    if (lightSpacePos.z > depth + uRSMBias) {
        return vec4(0.0);
    }
    
    // Sample RSM color with PCF
    vec4 rsmColor = texture(uRSMColorMap, rsmTexCoord);
    vec3 rsmNormal = decodeRSMNormal(rsmTexCoord);
    
    // Apply PCF smoothness filtering
    float shadowFactor = rsmPCF(lightSpacePos, rsmTexCoord, uRSMSmoothness);
    
    // Modulate by normal correspondence
    float normalFactor = max(dot(worldNormal, rsmNormal), 0.0);
    
    // Return filtered RSM data
    return vec4(rsmColor.rgb * shadowFactor * normalFactor, shadowFactor);
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
    
    // Sample RSM for indirect lighting influence
    vec4 rsmData = sampleRSMIndirect(fs_in.FragPosLightSpace.xyz / fs_in.FragPosLightSpace.w, N);
    
    // Cook-Torrance PBR
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float HdotV = max(dot(H, V), 0.0);
    
    // Calculate PBR
    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    
    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = FresnelSchlick(HdotV, F0);
    
    vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);
    vec3 kS = F;
    
    vec3 specular = (NDF * G * F) / max(0.001, 4.0 * NdotV * NdotL + 0.0001);
    
    // Direct light with shadow
    vec3 Lo = (kD * albedo / PI + specular) * uLightColor * uLightIntensity * NdotL * rsmData.w;
    
    // Add RSM indirect lighting contribution (modulated by intensity)
    Lo = Lo + (rsmData.rgb * uRSMIntensity * 0.5);
    
    // Ambient
    vec3 ambient = albedo * 0.03;
    
    vec3 result = ambient + Lo;
    
    // Tone mapping
    result = result / (result + vec3(1.0));
    result = pow(result, vec3(1.0 / 2.2));
    
    FragColor = vec4(result, texColor.a);
}
