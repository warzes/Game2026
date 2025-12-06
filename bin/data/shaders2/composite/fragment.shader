#version 330 core

#include "../tonemap.shader"

uniform sampler2D colorInput;
uniform sampler2D brightInput;

uniform bool bloom;

uniform sampler2D ssaoSampler;
uniform bool useSSAO;

in vec2 TexCoords;

layout(location = 0) out vec4 FragColor;

void main()
{
	vec3 hdrColor = texture(colorInput, TexCoords).rgb;
	//vec3 bloomColor = texture(brightInput, TexCoords).rgb;

	// Lets just multiply the LDR scene with the SSAO, it really should be applied to the ambient parts only, but needs a deferred pipeline for that
	if (useSSAO) {
		hdrColor *= texture(ssaoSampler, TexCoords).r;
	}

	vec3 result = hdrColor;
	//if (bloom)
	//{
	//	result += bloomColor; // additive blending
	//}

	//result = tonemapAuto(result);
	FragColor = vec4(result, 1.0);
}