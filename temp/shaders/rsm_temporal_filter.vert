#version 450 core

layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec2 aTexCoord;

out VS_OUT {
    vec2 TexCoords;
} vs_out;

void main() {
    gl_Position = vec4(aPosition, 0.0, 1.0);
    vs_out.TexCoords = aTexCoord;
}
