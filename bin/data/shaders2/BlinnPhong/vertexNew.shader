#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec2 vertexTexCoord;
layout(location = 4) in vec3 vertexTangent;
layout(location = 5) in vec3 vertexBitangent;

uniform mat4 modelMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 modelViewProjMatrix;

uniform float TileU;
uniform float TileV;

out VS_OUT {
	vec3 vertColor;
	vec3 pos;
	vec3 modelPos;
	vec2 texCoords;
	mat3 TBN;
	vec3 normal;
} vs_out;

void main()
{
	vs_out.vertColor = vertexColor;

	vs_out.texCoords = vertexTexCoord;
	vs_out.texCoords.x *= TileU;
	vs_out.texCoords.y *= TileV;

	vs_out.pos = (modelViewMatrix * vec4(vertexPosition, 1.0)).xyz;
	vs_out.modelPos = (modelMatrix * vec4(vertexPosition, 1.0)).xyz;

	vec3 T = -normalize(vec3(modelViewMatrix * vec4(vertexTangent, 0.0)));
	vec3 N = normalize(vec3(modelViewMatrix * vec4(vertexNormal, 0.0)));
	vec3 B = cross(N, T);

	vs_out.TBN = mat3(T, B, N);

	vs_out.normal = mat3(transpose(inverse(modelViewMatrix))) * vertexNormal;

	gl_Position = modelViewProjMatrix * vec4(vertexPosition, 1.0f);
}