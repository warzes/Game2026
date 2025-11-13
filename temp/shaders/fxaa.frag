#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform vec2 texelSize;

#define FXAA_REDUCE_MIN   (1.0/128.0)
#define FXAA_REDUCE_MUL   (1.0/8.0)
#define FXAA_SPAN_MAX     8.0

vec3 fxaa(sampler2D tex, vec2 fragCoord) {
    vec3 colorCenter = texture(tex, fragCoord).rgb;
    
    vec3 colorLeft = texture(tex, fragCoord - vec2(texelSize.x, 0.0)).rgb;
    vec3 colorRight = texture(tex, fragCoord + vec2(texelSize.x, 0.0)).rgb;
    vec3 colorUp = texture(tex, fragCoord + vec2(0.0, texelSize.y)).rgb;
    vec3 colorDown = texture(tex, fragCoord - vec2(0.0, texelSize.y)).rgb;
    
    float lumaCenter = dot(colorCenter, vec3(0.299, 0.587, 0.114));
    float lumaLeft = dot(colorLeft, vec3(0.299, 0.587, 0.114));
    float lumaRight = dot(colorRight, vec3(0.299, 0.587, 0.114));
    float lumaUp = dot(colorUp, vec3(0.299, 0.587, 0.114));
    float lumaDown = dot(colorDown, vec3(0.299, 0.587, 0.114));
    
    float lumaMin = min(lumaCenter, min(min(lumaLeft, lumaRight), min(lumaUp, lumaDown)));
    float lumaMax = max(lumaCenter, max(max(lumaLeft, lumaRight), max(lumaUp, lumaDown)));
    
    float lumaDelta = lumaMax - lumaMin;
    float lumaRange = max(FXAA_REDUCE_MIN, lumaMax * FXAA_REDUCE_MUL);
    
    if(lumaDelta < lumaRange) return colorCenter;
    
    float stepLength = 1.0;
    
    if(lumaLeft + lumaRight < lumaUp + lumaDown) {
        stepLength = texelSize.y;
        if(lumaLeft + lumaCenter > lumaRight + lumaDown) {
            stepLength = -stepLength;
        }
    } else {
        stepLength = texelSize.x;
        if(lumaUp + lumaCenter > lumaDown + lumaCenter) {
            stepLength = -stepLength;
        }
    }
    
    vec3 rgbA = 0.5 * (
        texture(tex, fragCoord + stepLength * vec2(-1.0, -1.0)).rgb +
        texture(tex, fragCoord + stepLength * vec2(1.0, 1.0)).rgb);
    vec3 rgbB = rgbA * 0.5 + 0.25 * (
        texture(tex, fragCoord + stepLength * vec2(-2.0, -2.0)).rgb +
        texture(tex, fragCoord + stepLength * vec2(2.0, 2.0)).rgb);
    
    return rgbB;
}

void main()
{
    FragColor = vec4(fxaa(screenTexture, TexCoords), 1.0);
}
