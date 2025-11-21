#version 330 core

struct Material
{
	sampler2D albedoMap;
};

uniform bool hasAlbedoMap;

uniform Material material;


const float alphaTestThreshold = 0.1;

in VS_OUT {
	vec3 VertColor;
	vec3 WorldPos;
	vec2 TexCoords;
	mat3 TBN;
	vec3 Normal;
} fs_in;

layout(location = 0) out vec4 FragColor;

void main()
{
	vec4 albedo = vec4(fs_in.VertColor, 1.0);
	if (hasAlbedoMap)
	{
		albedo = texture(material.albedoMap, fs_in.TexCoords) * albedo;
	}
	// early discard
	if(albedo.a < alphaTestThreshold) discard;	

	FragColor = albedo;
}