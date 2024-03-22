#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aOffset;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

void main()
{
	gl_Position = projection * view * vec4(aPos + aOffset, 1.0f);
}