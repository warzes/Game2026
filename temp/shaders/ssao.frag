#version 330 core
out float FragColor;
in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D ssaoNoise;

uniform mat4 projection;

// SSAO kernel: 16 sample points around the fragment
uniform vec3 samples[16];
uniform float radius = 0.5;
uniform float bias = 0.025;

void main()
{
    vec3 fragPos = texture(gPosition, TexCoords).xyz;
    vec3 normal = normalize(texture(gNormal, TexCoords).rgb);
    
    // Get random vector for sample rotation
    vec2 noiseScale = vec2(800.0/4.0, 600.0/4.0);
    vec3 randomVec = normalize(texture(ssaoNoise, TexCoords * noiseScale).xyz);
    
    // Create TBN
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    
    float occlusion = 0.0;
    for(int i = 0; i < 16; ++i)
    {
        // Get sample position in view space
        vec3 samplePos = fragPos + TBN * samples[i] * radius;
        
        // Project to screen space
        vec4 offset = projection * vec4(samplePos, 1.0);
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;
        
        // Get depth at sample position
        float sampleDepth = texture(gPosition, offset.xy).z;
        
        // Range check
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
    }
    occlusion = 1.0 - (occlusion / 16.0);
    
    FragColor = occlusion;
}
