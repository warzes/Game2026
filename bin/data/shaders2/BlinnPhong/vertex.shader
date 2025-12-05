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
uniform vec3 viewPos;

out VS_OUT {
	vec3 VertColor;
	vec3 FragPos;
	vec2 TexCoords;
	vec3 Normal;
	mat3 TBN;
#if defined(PARALLAX_MAPPING)
	vec3 TangentViewPos;
	vec3 TangentFragPos;
#endif
} vs_out;

mat3 ConstructTBN(mat4 model, vec3 normal, vec3 tangent, vec3 bitangent)
{
   return mat3(
        normalize(vec3(model * vec4(tangent,   0.0))),
        normalize(vec3(model * vec4(bitangent, 0.0))),
        normalize(vec3(model * vec4(normal,    0.0)))
    );
}

void main()
{
	vs_out.VertColor = vertexColor;
	vs_out.FragPos   = vec3(modelMatrix * vec4(vertexPosition, 1.0));
	vs_out.TexCoords = vertexTexCoord;	
	vs_out.Normal    = normalize(mat3(transpose(inverse(modelMatrix))) * vertexNormal);
	vs_out.TBN       = ConstructTBN(modelMatrix, vertexNormal, vertexTangent, vertexBitangent);

#if defined(PARALLAX_MAPPING)
	vs_out.TangentViewPos = transpose(vs_out.TBN) * viewPos;
	vs_out.TangentFragPos = transpose(vs_out.TBN) * vs_out.FragPos;
#endif

	gl_Position = projectionMatrix * viewMatrix * vec4(vs_out.FragPos, 1.0);
}