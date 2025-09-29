#version 330 core

struct Light
{
	int type; // 0 => directional, 1 => spot
	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;
	vec3 ambientStrength;
	vec3 diffuseStrength;
	vec3 specularStrength;
	mat4 lightSpaceMatrix;
};

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

struct Camera
{
	vec3 viewPos;
};

in vec3 fsColor;
in vec3 fsNormal;
in vec2 fsTexCoord;
in vec3 fragPos;
in mat3 TBN;

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D normalTexture;

uniform Camera cam;

uniform int solid;
uniform int shadowOn;
uniform int shadowMethod;
uniform float bias;
uniform int lightCount;
uniform Light light[10];
uniform sampler2D depthMap[10];

uniform Material material;

layout(location = 0) out vec4 fragColor;

const float alphaTestThreshold = 0.1f;

void main()
{
	// early discard
	vec4 albedo = texture(diffuseTexture, fsTexCoord);
	if (albedo.a <= alphaTestThreshold) discard;

	// start
	vec3 color;

	fragColor = albedo;
}