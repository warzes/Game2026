#version 330 core

#include "../utils.shader"
#include "../shadow.shader"
#include "../BlinnPhong.shader"

in VS_OUT {
	vec3 VertColor;
	vec3 FragPos;
	vec2 TexCoords;
	vec3 Normal;
	mat3 TBN;
#if defined(PARALLAX_MAPPING)
	vec3 TangentViewPos;
	vec3 TangentFragPos;
#endif
} fs_in;

uniform mat4 projectionMatrix;
uniform vec3 viewPos;

uniform sampler2D u_DiffuseMap;
uniform bool hasDiffuseMap;

uniform sampler2D u_SpecularMap;
uniform bool hasSpecularMap;

uniform vec4 u_Diffuse = vec4(1.0, 1.0, 1.0, 1.0);
uniform vec3 u_Specular = vec3(1.0, 1.0, 1.0);
uniform vec2 u_TextureTiling = vec2(1.0, 1.0);
uniform vec2 u_TextureOffset = vec2(0.0, 0.0);
uniform float u_Shininess = 100.0;

#if defined(PARALLAX_MAPPING)
uniform sampler2D u_HeightMap;
uniform int u_MinLayers = 8;
uniform int u_MaxLayers = 64;
uniform bool u_ParallaxClipEdges = false;
uniform float u_HeightScale = 0.05;
#endif

#if defined(NORMAL_MAPPING)
uniform sampler2D u_NormalMap;
uniform bool hasNormalMap;
#endif

const float alphaClippingThreshold = 0.1;

layout(location = 0) out vec4 FragColor;

vec3 ComputeNormal(vec2 texCoords, vec3 normal, sampler2D normalMap, mat3 TBN)
{
    normal = texture(normalMap, texCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    normal = normalize(TBN * normal);
    return normal;
}

void main()
{
	vec2 texCoords = TileAndOffsetTexCoords(fs_in.TexCoords, u_TextureTiling, u_TextureOffset);

#if defined(PARALLAX_MAPPING)
	texCoords = ApplyParallaxOcclusionMapping(texCoords, u_HeightMap, fs_in.TangentViewPos, fs_in.TangentFragPos, u_HeightScale, u_MinLayers, u_MaxLayers);

	if (u_ParallaxClipEdges && IsParallaxOutOfBounds(texCoords, u_TextureTiling, u_TextureOffset, projectionMatrix))
	{
		discard;
	}
#endif

	vec4 diffuse = u_Diffuse * vec4(fs_in.VertColor, 1.0);
	if (hasDiffuseMap)
	{
		diffuse = texture(u_DiffuseMap, texCoords) * diffuse;
	}

	// early discard
	if(diffuse.a < alphaClippingThreshold) discard;

	vec3 normal = normalize(fs_in.Normal);
#if defined(NORMAL_MAPPING)
	if (hasNormalMap)
	{
		normal = ComputeNormal(texCoords, fs_in.Normal, u_NormalMap, fs_in.TBN);
	}	
#endif

	// TODO: u_SpecularMap

	FragColor = ComputeBlinnPhongLighting(
		texCoords,
		normal,
		viewPos,
		fs_in.FragPos,
		diffuse,
		u_Specular,
		u_SpecularMap,
		u_Shininess
	);
}