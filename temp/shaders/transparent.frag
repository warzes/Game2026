#version 330 core

in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vTex;
in vec3 vTangent;

out vec4 FragColor;

struct DirLight {
    vec3 direction;
    float pad0;
    vec3 color;
    float intensity;
};

layout(std140) uniform Lights {
    DirLight dirLights[4];
    // ... other light arrays omitted for brevity
};

uniform sampler2D uDiffuse;
uniform sampler2D uSpecular;
uniform sampler2D uNormalMap;

uniform sampler2D dirShadowMaps[4];
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

void main() {
    vec4 texColor = texture(uDiffuse, vTex);
    
    // Early discard for fully transparent pixels
    if(texColor.a < 0.01) discard;
    
    vec3 albedo = texColor.rgb;
    vec3 specMap = texture(uSpecular, vTex).rgb;
    vec3 N = getNormalFromMap();
    vec3 V = normalize(viewPos - vWorldPos);
    
    vec3 result = vec3(0.0);
    
    // Simplified lighting
    for(int i = 0; i < 4; ++i) {
        vec3 L = normalize(-dirLights[i].direction);
        vec3 H = normalize(L + V);
        float diff = max(dot(N, L), 0.0);
        float spec = pow(max(dot(N, H), 0.0), 16.0) * specMap.r;
        result += dirLights[i].color * dirLights[i].intensity * (albedo * diff + spec);
    }
    
    result += vec3(0.03) * albedo;
    
    // Fog
    float distance = length(viewPos - vWorldPos);
    float fogFactor = clamp((distance - fogStart) / (fogEnd - fogStart), 0.0, 1.0);
    float heightFactor = smoothstep(fogHeightZone.x, fogHeightZone.y, vWorldPos.y);
    float finalFog = mix(0.0, fogFactor, heightFactor);
    
    vec3 color = mix(result, fogColor, finalFog);
    
    // Output with original alpha
    FragColor = vec4(color, texColor.a);
}
