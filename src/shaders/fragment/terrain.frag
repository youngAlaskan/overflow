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
    vec3 color = vec3(0.5f, 0.9f, 0.1f);
    FragColor = vec4(color * (iVertexData.Normal * 0.5f + 0.5f), 1.0f);
}