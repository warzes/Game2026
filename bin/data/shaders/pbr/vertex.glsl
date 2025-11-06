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

out vec3 fsColor;
out vec3 fsNormal;
out vec2 fsTexCoord;
out vec3 fsFragPos;
out mat3 fsTBN;

mat3 computeTBN()
{
	vec3 T = normalize(mat3(modelMatrix) * vertexTangent);
	vec3 N = normalize(mat3(modelMatrix) * vertexNormal);
	vec3 B = normalize(mat3(modelMatrix) * vertexBitangent);
	//vec3 B = normalize(cross(N, T)); // правая система => N*T

	return transpose(mat3(T, B, N));
}

void main()
{
	gl_Position  = projectionMatrix * viewMatrix * modelMatrix * vec4(vertexPosition, 1.0);
	fsColor      = vertexColor;
	fsNormal     = mat3(transpose(inverse(modelMatrix))) * vertexNormal;

	fsTexCoord   = vertexTexCoord;
	fsFragPos    = vec3(modelMatrix * vec4(vertexPosition, 1.0f));

	// compute TBN matrix
	fsTBN = computeTBN();
}