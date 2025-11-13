#version 450 core

// RSM Temporal Filter - Blends current and previous RSM data for temporal coherence
// Reduces flickering in indirect lighting calculations

in VS_OUT {
    vec2 TexCoords;
} fs_in;

// Current frame RSM maps
uniform sampler2D uRSMColorCurrent;
uniform sampler2D uRSMNormalCurrent;
uniform sampler2D uRSMDepthCurrent;

// Previous frame RSM maps (for temporal caching)
uniform sampler2D uRSMColorPrevious;
uniform sampler2D uRSMNormalPrevious;
uniform sampler2D uRSMDepthPrevious;

// Temporal blend factor (0.1 = keep 90% previous, 10% current for smooth transitions)
uniform float uTemporalBlend = 0.15f;

layout(location = 0) out vec4 outRSMColor;
layout(location = 1) out vec4 outRSMNormal;
layout(location = 2) out vec4 outRSMDepth;

void main() {
    // Sample both current and previous frames
    vec4 colorCurr = texture(uRSMColorCurrent, fs_in.TexCoords);
    vec4 colorPrev = texture(uRSMColorPrevious, fs_in.TexCoords);
    
    vec4 normalCurr = texture(uRSMNormalCurrent, fs_in.TexCoords);
    vec4 normalPrev = texture(uRSMNormalPrevious, fs_in.TexCoords);
    
    vec4 depthCurr = texture(uRSMDepthCurrent, fs_in.TexCoords);
    vec4 depthPrev = texture(uRSMDepthPrevious, fs_in.TexCoords);
    
    // Check for sudden changes (indicates motion or scene change)
    float depthDiff = abs(depthCurr.r - depthPrev.r);
    float colorDiff = length(colorCurr.rgb - colorPrev.rgb);
    float normalDiff = distance(normalCurr.rgb, normalPrev.rgb);
    
    // Adjust blend factor based on changes (more blending if stable)
    float blendFactor = uTemporalBlend;
    if (depthDiff > 0.01 || colorDiff > 0.3 || normalDiff > 0.3) {
        // Reduce temporal blending near edges/changes
        blendFactor = mix(uTemporalBlend, 0.5f, 0.5f);
    }
    
    // Temporal blending with exponential moving average
    outRSMColor = mix(colorPrev, colorCurr, blendFactor);
    outRSMNormal = mix(normalPrev, normalCurr, blendFactor);
    outRSMDepth = mix(depthPrev, depthCurr, blendFactor);
}
