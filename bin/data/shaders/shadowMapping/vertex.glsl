#version 330 core

layout(location = 0) in vec3 vertexPosition;
//layout(location = 1) in vec3 vertexColor;
//layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec2 vertexTexCoord;
//layout(location = 4) in vec3 vertexTangent;
//layout(location = 5) in vec3 vertexBitangent;

uniform mat4 mvpMatrix;

out vec2 fragTexCoord;

void main()
{
	fragTexCoord = vertexTexCoord;
	gl_Position = mvpMatrix * vec4(vertexPosition, 1.0f);
}