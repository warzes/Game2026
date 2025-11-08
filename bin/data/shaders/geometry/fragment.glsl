#version 330 core

in VS_OUT {
	vec3 FragPos;
	vec2 TexCoords;
	mat3 TBN;
	vec3 Normal;
} fs_in;

struct Material
{
	vec3 color_diffuse;
	vec3 color_specular;
	vec3 color_ambient;
	float opacity;
	float shininess;
	int hasDiffuse;
	int hasSpecular;
	int hasNormal;
	int nbTextures;
};

uniform Material material;

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D normalTexture;

layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 Normal;

void main()
{
	gPosition.rgb = fs_in.FragPos;
	if(material.hasNormal == 1)
	{
		Normal = texture(normalTexture, fs_in.TexCoords).rgb;
		Normal = normalize(Normal * 2.0f - 1.0f);
		Normal = normalize(fs_in.TBN * Normal);
	}
	else
	{
		Normal = normalize(fs_in.Normal);
	}
}