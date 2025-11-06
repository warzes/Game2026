#version 330 core

in vec3 fsColor;
in vec3 fsNormal;
in vec2 fsTexCoord;
in vec3 fsFragPos;
in mat3 fsTBN;

layout(location = 0) out vec4 fragColor;

void main()
{
	fragColor = vec4(0, 0, 1, 1);
}