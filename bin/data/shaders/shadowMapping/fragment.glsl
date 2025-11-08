#version 330 core

in vec2 fragTexCoord;

uniform sampler2D albedoTexture;
uniform int hasAlbedoMap;

void main()
{
	if(hasAlbedoMap == 1 && texture(albedoTexture, fragTexCoord).a < 0.1f)
		gl_FragDepth = 1.0f;
	else
		gl_FragDepth = gl_FragCoord.z;
}