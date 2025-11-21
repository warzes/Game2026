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

out VS_OUT {
	vec3 VertColor;
	vec3 WorldPos;
	vec2 TexCoords;
	mat3 TBN;
	vec3 Normal;
} vs_out;

void main()
{
	// Calculate world position
	vec4 worldPos = modelMatrix * vec4(vertexPosition, 1.0);
	vs_out.WorldPos = worldPos.xyz;

	// Calculate normal in world space (support non-uniform scale)
	mat3 normalMatrix = mat3(transpose(inverse(modelMatrix))); 
	vs_out.Normal = normalize(normalMatrix * vertexNormal);

	// Calculate TBN matrix for normal mapping
	vec3 T = normalize(normalMatrix * vertexTangent);
	// Re-orthogalize T with respect to N (Gram-Schmidt process)
	T = normalize(T - dot(T, vs_out.Normal) * vs_out.Normal);
	// Calculate bitangent
	vec3 B = cross(vs_out.Normal, T);
	// TBN matrix for transforming from tangent to world space
	vs_out.TBN = mat3(T, B, vs_out.Normal);

	vs_out.VertColor = vertexColor;
	vs_out.TexCoords = vertexTexCoord;

	gl_Position = projectionMatrix * viewMatrix * worldPos;
}