#version 330 core

struct Light
{
	vec3 position;
	vec3 direction;
	vec3 color;
	mat4 matrix;
	sampler2D shadowMapSampler;
};

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec2 vertexTexCoord;
layout(location = 4) in vec3 vertexTangent;
layout(location = 5) in vec3 vertexBitangent;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

uniform int lightCount;
uniform Light lights[MAX_LIGHTS];

out VS_OUT {
	vec3 VertColor;
	vec3 FragPos;
	vec2 TexCoords;
	mat3 TBN;
	vec3 Normal;
	vec4 FragPosLightSpace[MAX_LIGHTS];
} vs_out;

mat3 computeTBN()
{
	vec3 T = normalize(vec3(modelMatrix * vec4(vertexTangent,   0.0)));
	vec3 B = normalize(vec3(modelMatrix * vec4(vertexBitangent, 0.0)));
	vec3 N = normalize(vec3(modelMatrix * vec4(vertexNormal,    0.0)));

	return mat3(T, B, N);
}

void main()
{
	vs_out.VertColor = vertexColor;
	vs_out.FragPos = vec3(modelMatrix * vec4(vertexPosition, 1.0));
	vs_out.TexCoords = vertexTexCoord;
	vs_out.TBN = computeTBN();
	vs_out.Normal = mat3(transpose(inverse(modelMatrix))) * vertexNormal;

	for (int i = 0; i < lightCount; i++)
	{
		vs_out.FragPosLightSpace[i] = lights[i].matrix * vec4(vs_out.FragPos, 1.0);
	}

	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertexPosition, 1.0);
}