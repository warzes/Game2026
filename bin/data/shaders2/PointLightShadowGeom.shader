#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

uniform mat4 cubeMatrices[6];

in VS_OUT{
	vec2 texCoord;
} gs_in[];

out GS_OUT{
	vec2 texCoord;
} gs_out;

out vec4 FragPos;

void main()
{
	for (int face = 0; face < 6; ++face)
	{
		gl_Layer = face; // built-in variable that specifies to which face we render.
		for (int i = 0; i < 3; ++i)
		{
			FragPos = gl_in[i].gl_Position;
			gl_Position = cubeMatrices[face] * FragPos;
			gs_out.texCoord = gs_in[i].texCoord;
			EmitVertex();
		}
		EndPrimitive();
	}
}