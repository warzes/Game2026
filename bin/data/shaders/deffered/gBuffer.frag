#version 330 core

in vec3 fsPos;
in vec3 fsColor;
in vec3 fsNormal;
in vec2 fsTexCoord;
in vec3 fsTangent;
in vec3 fsBitangent;

uniform sampler2D diffuseTexture;

out vec4 geoData[5];

void main()
{
	vec4 diff = texture(diffuseTexture, fsTexCoord);
	if (diff.a == 0.0) discard;

	// output geometry.
	geoData[0] = vec4(diff.rgb * fsColor, 1);
	geoData[1] = vec4(fsNormal, 1);
	geoData[2] = vec4(fsPos, 1);
	geoData[3] = vec4(fsTangent, 1);
	geoData[4] = vec4(fsBitangent, 1);
}