#version 450

layout (binding = 0) uniform Matrices
{
    mat4 test;
} matrices;

layout (location = 0) in vec2 inPos;
layout (location = 1) in vec2 inTexCoord;

layout (location = 0) out vec2 outTexCood;

void main()
{
    outTexCood = inTexCoord;
	gl_Position = vec4(inPos.xy, 0.0, 1.0) * matrices.test;
}
