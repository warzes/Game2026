#version 330 core

in vec4 FragPos;

in GS_OUT{
	vec2 texCoord;
} fs_in;

uniform vec3 lightPos;
uniform float farPlane;

uniform sampler2D diffuseTexture;
uniform int hasDiffuseMap;

void main()
{
	if(hasDiffuseMap == 1 && texture(diffuseTexture, fs_in.texCoord).a < 0.1f)
	{
		gl_FragDepth = 1.0f;
		return;
	}
	else
	{
		float lightDistance = length(FragPos.xyz - lightPos);
		lightDistance = lightDistance / farPlane;

		gl_FragDepth = lightDistance;
	}
} 