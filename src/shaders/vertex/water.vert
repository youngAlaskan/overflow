#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

out vec3 vNormal;

void main()
{
    vNormal = aNormal;
    gl_Position = projection * view * vec4(aPos, 1.0);
}