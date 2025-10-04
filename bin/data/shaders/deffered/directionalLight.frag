#version 330 core

in vec2 fsUv;

uniform vec3 uCameraPos;

uniform sampler2D uColorTex;
uniform sampler2D uNormalTex;
uniform sampler2D uPositionTex;
uniform sampler2D uTangentTex;
uniform sampler2D uBitangentTex;

out vec4 outColor;

void main()
{
	vec3 albedo = texture(uColorTex, fsUv).xyz;
	vec3 n = normalize(texture(uNormalTex, fsUv).xyz);
	vec3 pos = texture(uPositionTex, fsUv).xyz;

	vec3 l = normalize(vec3(-0.7, 0.3, 0.1));
	vec3 v = normalize(uCameraPos - pos);
	vec3 h = normalize(l + v);

	vec3 color =
		// diffuse
		0.7 * albedo.xyz * max(0.0, dot(n.xyz, l)) +
		// specular
		0.4 * pow(max(0.0, dot(h, n)), 32.0) +
		// ambient.
		0.2 * albedo.xyz;

	outColor = vec4(color, 1.0);
}