#version 330 core

#include "../pbrCore.glsl"

struct Material
{
	sampler2D albedoSampler;
	sampler2D metalSampler;
	sampler2D roughnessSampler;
	sampler2D occlusionSampler;
	sampler2D emissiveSampler;
	sampler2D normalSampler;
};

struct Light
{
	vec3 position;
	vec3 direction;
	vec3 color;
	mat4 matrix;
	sampler2D shadowMapSampler;
};

uniform samplerCube skyboxSampler;
uniform samplerCube irradianceSampler;
uniform samplerCube radianceSampler;
uniform sampler2D brdfSampler;

uniform bool useIBL;
uniform bool useCheapIBL;

uniform bool hasAlbedo;
uniform bool hasMetal;
uniform bool hasRoughness;
uniform bool hasOcclusion;
uniform bool hasEmissive;
uniform bool hasNormal;

uniform Material material;
uniform vec3 viewPos;

uniform int lightCount;
uniform Light lights[MAX_LIGHTS];

const float alphaTestThreshold = 0.0;

in VS_OUT {
	vec3 VertColor;
	vec3 FragPos;
	vec2 TexCoords;
	mat3 TBN;
	vec3 Normal;
	vec4 FragPosLightSpace[MAX_LIGHTS];
} fs_in;

layout(location = 0) out vec4 FragColor;

void main()
{
	vec3 normal = normalize(fs_in.Normal);
	if (hasNormal) {
		normal = texture(material.normalSampler, fs_in.TexCoords).rgb;
		normal = normalize(normal * 2.0 - 1.0);
		normal = normalize(fs_in.TBN * normal);
	}

	vec4 albedo = vec4(fs_in.VertColor, 1.0);
	if (hasAlbedo)
	{
		albedo = texture(material.albedoSampler, fs_in.TexCoords) * albedo;
	}
	// early discard
	if(albedo.a == alphaTestThreshold) discard;


	FragColor = albedo;
}