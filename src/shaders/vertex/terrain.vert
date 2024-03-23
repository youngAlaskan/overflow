#version 330 core

layout (location = 0) in vec4 iPos;
layout (location = 1) in vec3 iNorm;
layout (location = 2) in vec2 iTexCoords;
layout (location = 3) in uint iWaterLevel;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

out VertexData
{
    vec4 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} oVertexData;

void main()
{
    oVertexData.FragPos = iPos;
    oVertexData.Normal = iNorm;
    oVertexData.TexCoords = iTexCoords;

	gl_Position = projection * view * iPos;
}