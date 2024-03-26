#version 330 core

in VertexData
{
    vec4 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} iVertexData;

out vec4 FragColor;

void main()
{
   FragColor = vec4(iVertexData.Normal.xyz, 1.0f) * vec4(0.5f, 0.9f, 0.1f, 1.0f);
}