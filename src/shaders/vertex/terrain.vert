#version 330 core

layout (location = 0) in vec3 iPos;
layout (location = 1) in vec3 iNorm;
layout (location = 2) in vec2 iTexCoords;
layout (location = 3) in float iWaterLevel;

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
    float WaterLevel;
} oVertexData;

void main()
{
    oVertexData.FragPos = vec4(iPos + vec3(0.0, iWaterLevel, 0.0), 1.0f);
    oVertexData.Normal = normalize(iNorm);
    oVertexData.TexCoords = iTexCoords;
    oVertexData.WaterLevel = iWaterLevel;

	gl_Position = projection * view * oVertexData.FragPos;
}