#version 330 core

in vec2 fragTexCoord;

uniform sampler2D diffuseTexture;
uniform int hasDiffuseMap;

void main()
{
	if(hasDiffuseMap == 1 && texture(diffuseTexture, fragTexCoord).a < 0.1f)
		gl_FragDepth = 1.0f;
	else
		gl_FragDepth = gl_FragCoord.z;
}