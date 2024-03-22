#version 330 core

layout (location = 0) in vec4 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in uint aWaterLevel;

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
    oVertexData.FragPos = aPos;
    oVertexData.Normal = aNorm;
    oVertexData.TexCoords = aTexCoords;

	gl_Position = projection * view * aPos;
}