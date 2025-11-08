#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec2 vertexTexCoord;
layout(location = 4) in vec3 vertexTangent;
layout(location = 5) in vec3 vertexBitangent;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

out VS_OUT {
	vec3 FragPos;
	vec2 TexCoords;
	mat3 TBN;
	vec3 Normal;
} vs_out;

mat3 computeTBN()
{
	vec3 T = normalize(vec3(viewMatrix * modelMatrix * vec4(vertexTangent,   0.0)));
	vec3 B = normalize(vec3(viewMatrix * modelMatrix * vec4(vertexBitangent, 0.0)));
	vec3 N = normalize(vec3(viewMatrix * modelMatrix * vec4(vertexNormal,    0.0)));

	return mat3(T, B, N);
}

void main()
{
	vs_out.FragPos = vec3(viewMatrix * modelMatrix * vec4(vertexPosition, 1.0f));
	vs_out.TexCoords = vertexTexCoord;
	
	// compute TBN matrix
	vs_out.TBN = computeTBN();

	vs_out.Normal = mat3(transpose(inverse(viewMatrix * modelMatrix))) * vertexNormal;

	gl_Position  = projectionMatrix * viewMatrix * modelMatrix * vec4(vertexPosition, 1.0);
}