#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec2 vertexTexCoord;
layout(location = 4) in vec3 vertexTangent;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 normalMatrix;

out vec3 fsColor;
out vec3 fsNormal;
out vec2 fsTexCoord;
out vec3 fsFragPos;
out mat3 fsTBN;

void main()
{
	gl_Position  = projectionMatrix * viewMatrix * modelMatrix * vec4(vertexPosition, 1.0);
	fsColor    = vertexColor;
	fsNormal   = mat3(normalMatrix) * vertexNormal;
	fsTexCoord = vertexTexCoord;

	fsFragPos = vec3(modelMatrix * vec4(vertexPosition, 1.0f));

	// compute TBN matrix
	// TODO: может bitangent хранить в вершине?
	vec3 bitangent = cross(vertexNormal, vertexTangent);

	vec3 T = normalize(vec3(modelMatrix * vec4(vertexTangent, 0.0f)));
	vec3 B = normalize(vec3(modelMatrix * vec4(bitangent, 0.0f)));
	vec3 N = normalize(vec3(modelMatrix * vec4(vertexNormal, 0.0f)));

	T = normalize(T - dot(T, N) * N);
	B = cross(N, T);
	fsTBN = transpose(mat3(T, B, N));
}