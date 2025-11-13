#version 330 core
out float FragColor;
in vec2 TexCoords;

uniform sampler2D depthTexture;
uniform sampler2D ssaoNoise;

uniform mat4 projection;
uniform mat4 invProjection;
uniform mat4 invView;
uniform vec2 screenSize;

uniform float radius = 0.5;
uniform float bias = 0.025;
uniform int kernelSize = 16;

// Reconstruct view-space position from depth and screen UV
vec3 reconstructPosition(vec2 uv, float depth) {
    // Convert to NDC
    vec4 ndc = vec4(uv * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    
    // Unproject to view space
    vec4 viewPos = invProjection * ndc;
    viewPos /= viewPos.w;
    
    return viewPos.xyz;
}

// Reconstruct world-space normal from depth gradient
vec3 reconstructNormal(vec2 uv, float centerDepth) {
    float pixelSize = 1.0 / 512.0;  // adjust based on target resolution
    
    float depthUp = texture(depthTexture, uv + vec2(0.0, pixelSize)).r;
    float depthDown = texture(depthTexture, uv - vec2(0.0, pixelSize)).r;
    float depthLeft = texture(depthTexture, uv - vec2(pixelSize, 0.0)).r;
    float depthRight = texture(depthTexture, uv + vec2(pixelSize, 0.0)).r;
    
    vec3 posCenter = reconstructPosition(uv, centerDepth);
    vec3 posUp = reconstructPosition(uv + vec2(0.0, pixelSize), depthUp);
    vec3 posLeft = reconstructPosition(uv - vec2(pixelSize, 0.0), depthLeft);
    
    vec3 dx = posRight - posCenter;
    vec3 dy = posUp - posCenter;
    
    return normalize(cross(dy, dx));
}

void main()
{
    float centerDepth = texture(depthTexture, TexCoords).r;
    vec3 fragPos = reconstructPosition(TexCoords, centerDepth);
    vec3 normal = reconstructNormal(TexCoords, centerDepth);
    
    // Get random vector
    vec2 noiseScale = vec2(512.0/4.0, 512.0/4.0);
    vec3 randomVec = normalize(texture(ssaoNoise, TexCoords * noiseScale).xyz);
    
    // Create TBN
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    
    // Generate kernel samples (hardcoded for 16 samples)
    vec3 samples[16] = vec3[](
        vec3(0.7, 0.2, -0.5), vec3(-0.2, 0.8, -0.3), vec3(0.1, 0.3, 0.9),
        vec3(-0.8, 0.1, 0.2), vec3(0.5, -0.7, 0.4), vec3(-0.3, 0.2, 0.7),
        vec3(0.9, 0.1, 0.1), vec3(-0.5, -0.6, 0.5), vec3(0.2, 0.9, 0.1),
        vec3(-0.7, 0.3, 0.4), vec3(0.4, -0.2, 0.8), vec3(-0.1, 0.7, 0.6),
        vec3(0.6, 0.5, -0.3), vec3(-0.9, 0.1, 0.3), vec3(0.3, -0.8, 0.2),
        vec3(-0.4, 0.4, 0.9)
    );
    
    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i) {
        vec3 samplePos = fragPos + TBN * samples[i] * radius;
        
        // Project to screen space
        vec4 offset = projection * vec4(samplePos, 1.0);
        offset.xyz /= offset.w;
        offset.xy = offset.xy * 0.5 + 0.5;
        
        float sampleDepth = texture(depthTexture, offset.xy).r;
        vec3 sampleViewPos = reconstructPosition(offset.xy, sampleDepth);
        
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleViewPos.z));
        occlusion += (sampleViewPos.z >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
    }
    
    occlusion = 1.0 - (occlusion / float(kernelSize));
    FragColor = occlusion;
}
