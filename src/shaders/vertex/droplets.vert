#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aOffset;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

out lowp float WorldHeight;
out vec2 ViewportCoord;

void main()
{
	gl_Position = projection * view * vec4(aPos + aOffset, 1.0f);
    WorldHeight = aOffset.y;
    vec3 ndc = gl_Position.xyz / gl_Position.w;
    ViewportCoord = ndc.xy * 0.5 + 0.5 * vec2(400, 400);
}